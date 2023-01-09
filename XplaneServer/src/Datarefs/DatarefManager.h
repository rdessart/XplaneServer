#pragma once

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
    std::size_t AddDataref(std::string name, AbstractDataref* dataref);
    std::size_t AddDataref(std::string path,
                   std::string name, 
                   std::string conversionFactor = "1.0", 
                   Dataref::Type type = Dataref::Type::Unknown);

    AbstractDataref* GetDatarefByName(std::string name);
    void AddMessageToQueue(json j);
    json GetNextMessage();
    std::size_t GetMessageQueueLenght();
    std::queue<json> GetQueue();
    Logger GetLogger();
    bool isFF320Api();
    SharedValuesInterface* GetFF320Interface();

protected:
    Logger m_logger = Logger("XPLMServer.log", "DatarefManager", false);
    std::map<std::string, AbstractDataref*> _datarefMap;
    SharedValuesInterface* m_ff320;
    bool _isFF320Enable = false;
    std::queue<json> m_messageQueue;
    std::mutex gLock;
};
