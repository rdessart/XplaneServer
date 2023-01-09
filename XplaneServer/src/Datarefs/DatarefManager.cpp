#include "DatarefManager.h"


enum class OperationsEnum
{
    Unknown,
    Speak,
    SetData,
    GetData,
    RegisterData,
    SetRegData,
    GetRegData
};

OperationsEnum GetOperation(std::string ops)
{
    static const std::map<std::string, OperationsEnum> operationsStrings{
        {"speak", OperationsEnum::Speak},
        {"setdata", OperationsEnum::SetData},
        {"getdata", OperationsEnum::GetData},
        {"registerdata", OperationsEnum::RegisterData},
        {"setregdata", OperationsEnum::SetRegData},
        {"getregdata", OperationsEnum::GetRegData},
    };
    std::transform(ops.begin(), ops.end(), ops.begin(),
        [](unsigned char c) { return std::tolower(c); });
    auto itr = operationsStrings.find(ops);
    if (itr != operationsStrings.end())
    {
        return itr->second;
    }
    return OperationsEnum::Unknown;
}

void Callback(double step, void* tag)
{
	DatarefManager* cm = (DatarefManager*)tag;
    while(cm->GetMessageQueueLenght() > 0)
    {
        json message = cm->GetNextMessage();
        cm->GetLogger().Log("Message : '" + message.dump() + "'");
        if (!message.contains("Operation")) continue;
        OperationsEnum ops = GetOperation(message.value("Operation", ""));
        switch (ops)
        {
        case OperationsEnum::Speak:
            XPLMSpeakString(message.value("Text", "").c_str());
            break;
        case OperationsEnum::SetData:
        {
            cm->GetLogger().Log("Setting dataref");
            if(!message.contains("Dataref")) continue;
            AbstractDataref* d;
            std::string link = message["Dataref"]["Link"].get<std::string>();
            if( cm->isFF320Api() &&
                link.find("Aircraft") != std::string::npos &&
                link.find(".") != std::string::npos)
            {
                d = new FFDataref(cm->GetFF320Interface());
            }
            else {
                d = new Dataref();
            }
            d->FromJson(message["Dataref"]);
            break;
        }
        case OperationsEnum::GetData:
            cm->GetLogger().Log("Getting dataref");
            break;
        case OperationsEnum::RegisterData:
            cm->GetLogger().Log("Registering dataref");
            break;
        case OperationsEnum::SetRegData:
            cm->GetLogger().Log("Setting registered dataref");
            break;
        case OperationsEnum::GetRegData:
            cm->GetLogger().Log("Getting registered dataref");
            break;
        default:
            break;
        }
    }
}

DatarefManager::DatarefManager(bool enableFlightFactorAPI) :
    _isFF320Enable(false), m_ff320(0)
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

std::size_t DatarefManager::AddDataref(std::string name, AbstractDataref *dataref)
{
    if(_datarefMap.contains(name))
    {
        m_logger.Log("Caution dataref storage already contain a dataref nammed : '" + name + "' overriding");
    }
    _datarefMap.emplace(name, dataref);
    m_logger.Log("Added dataref: '" + name + "' overriding");
    return _datarefMap.size();
}

std::size_t  DatarefManager::AddDataref(std::string path, std::string name, std::string conversionFactor, Dataref::Type type)
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
    gLock.lock();
        m_messageQueue.emplace(j);
    gLock.unlock();
}

json DatarefManager::GetNextMessage()
{
    gLock.lock();
        json message = m_messageQueue.back();
        m_messageQueue.pop();
    gLock.unlock();
    return message;
}

std::size_t DatarefManager::GetMessageQueueLenght()
{
    gLock.lock();
    std::size_t lenght = m_messageQueue.size();
    gLock.unlock();
    return lenght;
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

SharedValuesInterface* DatarefManager::GetFF320Interface()
{
    return m_ff320;
}
