#ifdef IBM
	#define _WINSOCKAPI_
#endif

#include <stdio.h>
#include <string.h>
#include <thread>
#include <future>
#include <cstdint>

#include <nlohmann/json.hpp>

#include <XPLM/XPLMDataAccess.h>
#include <XPLM/XPLMProcessing.h>
#include <XPLM/XPLMUtilities.h>
#include <XPLM/XPLMMenus.h>

#include "Datarefs/DatarefManager.h"
#include "Datarefs/Dataref.h"
#include "Network/UDPBeacon.h"
#include "Network/UDPServer.h"

static float InitalizerCallback(float elapsed, float elpasedFlightLoop, int counter, void* refcounter);
static float BeaconCallback(float elapsed, float elpasedFlightLoop, int counter, void* refcounter);
static float RunCallback(float elapsed, float elpasedFlightLoop, int counter, void* refcounter);
static float ResponseCallback(float elapsed, float elpasedFlightLoop, int counter, void* refcounter);
static void	MenuHandlerCallback(void* inMenuRef, void* inItemRef);

std::map<int, IPInfo> IPMap;
static UDPBeaon beacon;
static XPLMFlightLoopID initalizerCallbackId;
static XPLMFlightLoopID beaconCallbackId;
static Logger logger("XPLMServer.log", "MAIN", false);
static std::thread BeaconThread;
static UDPServer server;
static std::string AcfAuthor;
static std::string AcfDescription;
static int SimVersion, SDKVersion;
DatarefManager* manager;
static XPLMMenuID eSkyInstructorMenu;

PLUGIN_API int XPluginStart(char* outName, char* outSig, char* outDesc)
{
	/* Provide our plugin's profile to the plugin system. */
	std::string name = "XPLMServer";
	std::string sig = "eSkyStudio.Simulator.XPlaneServer";
	std::string description = "Server for Xplane";

#ifdef IBM
	strcpy_s(outName, 256, name.c_str());
	strcpy_s(outSig, 256, sig.c_str());
	strcpy_s(outDesc, 256, description.c_str());
#else
	strcpy(outName, name.c_str());
	strcpy(outSig, sig.c_str());
	strcpy(outDesc, description.c_str());
#endif

	eSkyInstructorMenu = XPLMCreateMenu("eSkyInstructor", nullptr, 0, MenuHandlerCallback, 0);
	if (eSkyInstructorMenu == NULL)
	{
		logger.Log("Unable to create eSkyInstructor Submenu");
	}
	XPLMCreateFlightLoop_t initalizerParameter;
	initalizerParameter.structSize = sizeof(XPLMCreateFlightLoop_t);
	initalizerParameter.phase = xplm_FlightLoop_Phase_BeforeFlightModel;
	initalizerParameter.refcon = nullptr;
	initalizerParameter.callbackFunc = InitalizerCallback;

	XPLMCreateFlightLoop_t beaconCallbackParameter;
	beaconCallbackParameter.structSize  = sizeof(XPLMCreateFlightLoop_t);
	beaconCallbackParameter.phase       = xplm_FlightLoop_Phase_BeforeFlightModel;
	beaconCallbackParameter.refcon      = nullptr;
	beaconCallbackParameter.callbackFunc = BeaconCallback;

	initalizerCallbackId = XPLMCreateFlightLoop(&initalizerParameter);
	beaconCallbackId = XPLMCreateFlightLoop(&beaconCallbackParameter);

	XPLMScheduleFlightLoop(initalizerCallbackId, -1, 0);
	return 1;
}

PLUGIN_API void	XPluginStop(void)
{
}

PLUGIN_API void XPluginDisable(void)
{
	XPLMDestroyFlightLoop(beaconCallbackId);
	XPLMDestroyFlightLoop(initalizerCallbackId);
}

PLUGIN_API int XPluginEnable(void)
{
	return 1;
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFromWho, int inMessage, void* inParam)
{
}

static float InitalizerCallback(float elapsed, float elpasedFlightLoop, int counter, void* refcounter)
{
	Dataref d1;
	d1.TypeDataref = DatarefType::XPLMDataref;
	d1.Load("sim/aircraft/view/acf_author");
	d1.SetType(Dataref::Type::Data);
	
	Dataref d2;
	d2.TypeDataref = DatarefType::XPLMDataref;
	d2.Load("sim/aircraft/view/acf_descrip");
	d2.SetType(Dataref::Type::Data);

	std::vector<IPInfo> ips = FindIp();
	char menuId = 0;
	for (auto& ip : ips)
	{
		int res = XPLMAppendMenuItem(eSkyInstructorMenu, ip.str_ip.c_str(), (void*)menuId, 0);
		if (res < 0)
		{
			logger.Log("Unable to add IP : '" + ip.str_ip + "' to menu");
			continue;
		}
		XPLMCheckMenuItem(eSkyInstructorMenu, menuId, xplm_Menu_Unchecked);
		IPMap.emplace(res, ip);
		menuId += 1;
	}

	XPLMCheckMenuItem(eSkyInstructorMenu, 0, xplm_Menu_Checked);

	beacon.SetIPAddress(IPMap[0]);

	AcfAuthor = d1.GetValue();
	AcfDescription = d2.GetValue();
	XPLMHostApplicationID id;
	XPLMGetVersions(&SimVersion, &SDKVersion, &id);
	int res = beacon.Initalize();
	logger.Log("UDP Beacon initalizer returned " + std::to_string(res));
	XPLMScheduleFlightLoop(beaconCallbackId, -1, 0);
	manager = new DatarefManager(true);

	res = server.Initalize();
	logger.Log("UDP Server initalizer returned " + std::to_string(res));

	

	auto futptr = std::make_shared<std::future<void>>();
	*futptr = std::async(std::launch::async, [futptr]() 
	{
		server.ReceiveMessage(manager);
	});
	if(!manager->isFF320Api())
	{

		XPLMCreateFlightLoop_t callbackParameter;
		callbackParameter.structSize = sizeof(XPLMCreateFlightLoop_t);
		callbackParameter.phase = xplm_FlightLoop_Phase_BeforeFlightModel;
		callbackParameter.refcon = nullptr;
		callbackParameter.callbackFunc = RunCallback;
		XPLMFlightLoopID callbackId = XPLMCreateFlightLoop(&callbackParameter);
		XPLMScheduleFlightLoop(callbackId, -1.0f, 1);
	}

	XPLMCreateFlightLoop_t respCallbackParameter;
	respCallbackParameter.structSize = sizeof(XPLMCreateFlightLoop_t);
	respCallbackParameter.phase = xplm_FlightLoop_Phase_BeforeFlightModel;
	respCallbackParameter.refcon = nullptr;
	respCallbackParameter.callbackFunc = ResponseCallback;
	XPLMFlightLoopID respCallbackId = XPLMCreateFlightLoop(&respCallbackParameter);
	XPLMScheduleFlightLoop(respCallbackId, -1.0f, 1);

	return 0;
}
	
void SendBeacon(json message)
{
	beacon.SendMessage(message);
}

static float ResponseCallback(float elapsed, float elpasedFlightLoop, int counter, void* refcounter)
{
	while (manager->GetMessageOutQueueLenght() > 0)
	{
		server.SendMessage(manager->GetNextMessageOut());
	}
	return 0.1f;
}

static float BeaconCallback(float elapsed, float elpasedFlightLoop, int counter, void* refcounter)
{
	json j;
	j.emplace("Operation", "Beacon");
	j.emplace("Author", AcfAuthor);
	j.emplace("Description", AcfDescription);
	j.emplace("Simulator", "X-Plane");
	j.emplace("SimulatorVersion", SimVersion);
	j.emplace("SimulatorSDKVersion", SDKVersion);

	auto futptr = std::make_shared<std::future<void>>();
	*futptr = std::async(std::launch::async,
			[futptr, j]() {
				SendBeacon(j);
			});
	return 1.0f;
}

float RunCallback(float elapsed, float elpasedFlightLoop, int counter, void* refcounter)
{
	Callback(0.0, manager);
	return -1.0f;
}

void MenuHandlerCallback(void* inMenuRef, void* inItemRef)
{
	intptr_t id = (intptr_t)(inItemRef);
	for(auto &kv : IPMap)
	{
		XPLMCheckMenuItem(eSkyInstructorMenu, kv.first, xplm_Menu_Unchecked);
	}
	XPLMCheckMenuItem(eSkyInstructorMenu, id, xplm_Menu_Checked);
	beacon.SetIPAddress(IPMap[id]);
	return;
}
