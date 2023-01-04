#ifdef IBM
	#define _WINSOCKAPI_
#endif

#include <stdio.h>
#include <string.h>
#include <thread>
#include <future>

#include <nlohmann/json.hpp>

#include <XPLM/XPLMDataAccess.h>
#include <XPLM/XPLMProcessing.h>
#include <XPLM/XPLMUtilities.h>

#include "Datarefs/Dataref.h"
#include "Network/UDPBeacon.h"

static float InitalizerCallback(float elapsed, float elpasedFlightLoop, int counter, void* refcounter);
static float BeaconCallback(float elapsed, float elpasedFlightLoop, int counter, void* refcounter);
static UDPBeaon beacon;
static XPLMFlightLoopID initalizerCallbackId;
static XPLMFlightLoopID beaconCallbackId;
static Logger logger("XPLMServer", "MAIN", false);
static std::thread BeaconThread;

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
	XPLMCreateFlightLoop_t initalizerParameter;
	//{
	//	sizeof(XPLMCreateFlightLoop_t), xplm_FlightLoop_Phase_BeforeFlightModel, nullptr, InitalizerCallback,
	//};
	initalizerParameter.structSize = sizeof(XPLMCreateFlightLoop_t);
	initalizerParameter.phase = xplm_FlightLoop_Phase_BeforeFlightModel;
	initalizerParameter.refcon = nullptr;
	initalizerParameter.callbackFunc = InitalizerCallback;

	XPLMCreateFlightLoop_t beaconCallbackParameter;
	/*{
		sizeof(XPLMCreateFlightLoop_t), xplm_FlightLoop_Phase_BeforeFlightModel, nullptr, BeaconCallback,
	};*/
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
	int res = beacon.Initalize();
	logger.Log("UDP Beacon initalizer returned " + std::to_string(res));
	XPLMScheduleFlightLoop(beaconCallbackId, -1, 0);
	return 0;
}

void SendBeacon(json message)
{
	beacon.SendMessage(message);
}

static float BeaconCallback(float elapsed, float elpasedFlightLoop, int counter, void* refcounter)
{
	Dataref d1;
	d1.DatarefType = DatarefType::XPLMDataref;
	d1.Load("sim/aircraft/view/acf_author");
	d1.SetType(Dataref::Type::Data);
	Dataref d2;
	d2.DatarefType = DatarefType::XPLMDataref;
	d2.Load("sim/aircraft/view/acf_descrip");
	d2.SetType(Dataref::Type::Data);
	int sim, sdk;
	XPLMHostApplicationID id;
	XPLMGetVersions(&sim, &sdk, &id);
	json j;
	j.emplace("Ops", "Beacon");
	j.emplace("Author", d1.GetValue());
	j.emplace("Description", d2.GetValue());
	j.emplace("Simulator", "X-Plane");
	j.emplace("SimulatorVersion", sim);
	j.emplace("SimulatorSDKVersion", sdk);

	auto futptr = std::make_shared<std::future<void>>();
	*futptr = std::async(std::launch::async,
			[futptr, j]() {
				SendBeacon(j);
			});
	return 1.0f;
}