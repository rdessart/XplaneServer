#include "DatarefManager.h"

void Callback(double step, void* tag)
{
	DatarefManager* cm = (DatarefManager*)tag;
    std::queue<json> messageQueue = cm->GetQueue();
    while(messageQueue.size() > 0)
    {
        json message = messageQueue.front();
        messageQueue.pop();
        cm->GetLogger().Log(message.dump());
    }
}

DatarefManager::DatarefManager(bool enableFlightFactorAPI) : 
    _isFF320Enable(false)
{
    if(enableFlightFactorAPI)
    {
        m_ff320 = new SharedValuesInterface();
        m_logger.Log("Initalising FF320 Data Interface...");
        int ffPluginID = XPLMFindPluginBySignature(XPLM_FF_SIGNATURE);
        if(ffPluginID == XPLM_NO_PLUGIN_ID)
        {
            m_logger.Log("Plugin not found !", Logger::Severity::CRITICAL);
            return;
        }
        m_logger.Log("FF320 plugin ID : " + std::to_string(ffPluginID));
        XPLMSendMessageToPlugin(ffPluginID, XPLM_FF_MSG_GET_SHARED_INTERFACE, m_ff320);
        if (m_ff320->DataVersion == NULL) {
            m_logger.Log("[FF320API] Unable to load version!");
            return;
        }
        unsigned int ffAPIdataversion = m_ff320->DataVersion();
        m_logger.Log("[FF320API] Version : " + std::to_string(ffAPIdataversion));
        _isFF320Enable = true;
        m_ff320->DataAddUpdate(Callback, this);
    }

}

int DatarefManager::AddDataref(std::string name, AbstractDataref *dataref)
{
    if(_datarefMap.contains(name))
    {
        m_logger.Log("Caution dataref storage already contain a dataref nammed : '" + name + "' overriding");
    }
    _datarefMap.emplace(name, dataref);
    m_logger.Log("Added dataref: '" + name + "' overriding");
    return _datarefMap.size();
}

int DatarefManager::AddDataref(std::string path, std::string name, std::string conversionFactor, Dataref::Type type)
{
    if(_datarefMap.contains(name))
    {
        m_logger.Log("Caution dataref storage already contain a dataref nammed : '" + name + "' overriding");
    }
    AbstractDataref* dataref;
    if(path.find(".") != std::string::npos) //it's an FFDataref
    {
        dataref = new Dataref();
        dataref->Load(path);
        if(type == Dataref::Type::Unknown){
            ((Dataref*)dataref)->LoadType();
        }
        else{
            ((Dataref*)dataref)->SetType(type);
        }
    }
    _datarefMap.emplace(name, dataref);
    return _datarefMap.size();
}

AbstractDataref *DatarefManager::GetDatarefByName(std::string name)
{
    if(!_datarefMap.contains(name))
    {
        m_logger.Log("dataref nammed: '" + name + "' NOT FOUND!");
        return nullptr;
    }
    return _datarefMap.at(name);
}

void DatarefManager::AddMessageToQueue(json j)
{
    m_messageQueue.emplace(j);
}

std::queue<json> DatarefManager::GetQueue()
{
    return std::queue<json>(m_messageQueue);
}

Logger DatarefManager::GetLogger()
{
    return m_logger;
}

bool DatarefManager::isFF320Api()
{
    return _isFF320Enable;
}
