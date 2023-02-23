#pragma once

#include "../Network/Message.h"

#include <string>
#include <vector>
#include <map>
#include <queue>
#include <mutex>

#include <XPLM/XPLMPlugin.h>
#include <nlohmann/json.hpp>

#include "AbstractDataref.h"
#include "FFA320Dataref.h"
#include "Dataref.h"
#include "../Tools/SharedValue.h"
#include "../Tools/Logger.h"

using json = nlohmann::json;

void Callback(double step, void* tag);

class DatarefManager{
public:
    DatarefManager(bool enableFlightFactorAPI=false);
    //std::size_t AddDataref(std::string name, AbstractDataref* dataref);
    //std::size_t AddDataref(std::string path,
    //               std::string name, 
    //               std::string conversionFactor = "1.0", 
    //               Dataref::Type type = Dataref::Type::Unknown);

    AbstractDataref* GetDatarefByName(std::string name);
    void AddDatarefToMap(std::string name, AbstractDataref* dataref);
    void AddMessageToQueue(Message m);
    Message GetNextMessage();
    std::size_t GetMessageQueueLenght();
    Message GetNextMessageOut();
    void AddMessageToOutQueue(Message m);
    std::size_t GetMessageOutQueueLenght();
    std::queue<Message> GetQueue();
    Logger GetLogger();
    bool isFF320Api();
    SharedValuesInterface* GetFF320Interface();

protected:
    Logger m_logger = Logger("XPLMServer.log", "DatarefManager", false);
    std::map<std::string, AbstractDataref*> m_datarefMap;
    SharedValuesInterface* m_ff320;
    bool m_isFF320Enable = false;
    std::queue<Message> m_messageQueue;
    std::queue<Message> m_messageOutQueue;
    std::mutex gLock;
};
