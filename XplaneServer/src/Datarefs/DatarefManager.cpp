#include "DatarefManager.h"


enum class OperationsEnum
{
    Unknown,
    Speak,
    SetData,
    GetData,
    RegisterData,
    SetRegData,
    GetRegData,
    GetDatarefInfo,
    GetRegDatarefInfo,
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
        {"datarefinfo", OperationsEnum::GetRegData},
        {"regdatarefinfo", OperationsEnum::GetRegData},
        {"regdatarefinfo", OperationsEnum::GetDatarefInfo},
        {"regdatarefinfo", OperationsEnum::GetRegDatarefInfo},
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
        Message m = cm->GetNextMessage();
        cm->GetLogger().Log("Message : '" + m.message.dump() + "'");
        if (!m.message.contains("Operation")) continue;
        OperationsEnum ops = GetOperation(m.message.value("Operation", ""));
        switch (ops)
        {
        case OperationsEnum::Speak:
            XPLMSpeakString(m.message.value("Text", "").c_str());
            m.message["Result"] = "Ok";
            break;
        case OperationsEnum::SetData:
        {
            cm->GetLogger().Log("Setting dataref");
            if(!m.message.contains("Dataref")) continue;
            AbstractDataref* d;
            std::string link = m.message["Dataref"]["Link"].get<std::string>();
            if( cm->isFF320Api() &&
                link.find("Aircraft") != std::string::npos &&
                link.find(".") != std::string::npos)
            {
                d = new FFDataref(cm->GetFF320Interface());
            }
            else {
                d = new Dataref();
            }
            d->FromJson(m.message["Dataref"]);
            m.message["Result"] = "Ok";
            free(d);
            break;
        }
        case OperationsEnum::GetData:
        {
            cm->GetLogger().Log("Getting dataref");
            if (!m.message.contains("Dataref")) {
                m.message["Result"] = "Error:Missing Dataref entry in JSON";
                cm->AddMessageToOutQueue(m);
                continue;
            }
            std::string link = m.message["Dataref"]["Link"].get<std::string>();
            AbstractDataref* d;
            if (cm->isFF320Api() &&
                link.find("Aircraft") != std::string::npos &&
                link.find(".") != std::string::npos)
            {
                d = new FFDataref(cm->GetFF320Interface());
            }
            else {
                d = new Dataref();
            }
            d->FromJson(m.message["Dataref"]);
            std::string value = d->GetValue();
            m.message["Dataref"]["Value"] = value;
            m.message["Result"] = "Ok";
            free(d);
            break;
        }
        case OperationsEnum::RegisterData:
        {
            cm->GetLogger().Log("Registering dataref");
            if (!m.message.contains("Dataref")) {
                m.message["Result"] = "Error:Missing Dataref entry in JSON";
                cm->AddMessageToOutQueue(m);
                continue;
            }
            if (!m.message.contains("Name"))
            {
                m.message["Result"] = "Error:Missing 'Name' field";
                cm->AddMessageToOutQueue(m);
                continue;
            }
            std::string link = m.message["Dataref"]["Link"].get<std::string>();
            AbstractDataref* d;
            if (cm->isFF320Api() &&
                link.find("Aircraft") != std::string::npos &&
                link.find(".") != std::string::npos)
            {
                d = new FFDataref(cm->GetFF320Interface());
            }
            else {
                d = new Dataref();
            }
            d->FromJson(m.message["Dataref"]);
            cm->AddDatarefToMap(m.message["Name"], d);
            m.message["Result"] = "Ok";
            cm->AddMessageToOutQueue(m);
            break;
        }
        case OperationsEnum::SetRegData:
        {
            cm->GetLogger().Log("Setting registered dataref");
            if (!m.message.contains("Name"))
            {
                m.message["Result"] = "Error:Missing 'Name' field";
                cm->AddMessageToOutQueue(m);
                continue;
            }
            if (!m.message.contains("Value"))
            {
                m.message["Result"] = "Error:Missing 'Value' field";
                cm->AddMessageToOutQueue(m);
                continue;
            }

            AbstractDataref* d = cm->GetDatarefByName(m.message.value("Name", ""));
            if (d == nullptr) {
                m.message["Result"] = "Error:Dataref not in map !";
                cm->AddMessageToOutQueue(m);
                continue;
            }
            d->SetValue(m.message.value("Value", ""));

            m.message["Value"] = d->GetValue();
            m.message["Result"] = "Ok";
            cm->AddMessageToOutQueue(m);
            break;
        }
        case OperationsEnum::GetRegData:
        {
            cm->GetLogger().Log("Setting registered dataref");
            if (!m.message.contains("Name"))
            {
                m.message["Result"] = "Error:Missing 'Name' field";
                cm->AddMessageToOutQueue(m);
                continue;
            }

            AbstractDataref* d = cm->GetDatarefByName(m.message.value("Name", ""));
            if (d == nullptr) {
                m.message["Result"] = "Error:Dataref not in map !";
                cm->AddMessageToOutQueue(m);
                continue;
            }

            m.message["Value"] = d->GetValue();
            m.message["Result"] = "Ok";
            cm->AddMessageToOutQueue(m);
            break;
        }
        case OperationsEnum::GetDatarefInfo: 
        {
            cm->GetLogger().Log("Getting dataref");
            if (!m.message.contains("Dataref")) {
                m.message["Result"] = "Error:Missing Dataref entry in JSON";
                cm->AddMessageToOutQueue(m);
                continue;
            }
            std::string link = m.message["Dataref"]["Link"].get<std::string>();
            AbstractDataref* d;
            if (cm->isFF320Api() &&
                link.find("Aircraft") != std::string::npos &&
                link.find(".") != std::string::npos)
            {
                d = new FFDataref(cm->GetFF320Interface());
            }
            else {
                d = new Dataref();
            }
            m.message["Dataref"] = d->ToJson();
            cm->AddMessageToOutQueue(m);
            break;
        }
        case OperationsEnum::GetRegDatarefInfo:
        {
            cm->GetLogger().Log("Setting registered dataref");
            if (!m.message.contains("Name"))
            {
                m.message["Result"] = "Error:Missing 'Name' field";
                cm->AddMessageToOutQueue(m);
                continue;
            }

            AbstractDataref* d = cm->GetDatarefByName(m.message.value("Name", ""));
            if (d == nullptr) {
                m.message["Result"] = "Error:Dataref not in map !";
                cm->AddMessageToOutQueue(m);
                continue;
            }

            m.message["Dataref"] = d->ToJson();
            m.message["Result"] = "Ok";
            cm->AddMessageToOutQueue(m);
            break;
        }
        default:
            m.message["Result"] = "NoOp";
            break;
        }
        cm->AddMessageToOutQueue(m);
    }
}

DatarefManager::DatarefManager(bool enableFlightFactorAPI) :
    m_isFF320Enable(false), m_ff320(0)
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
        m_isFF320Enable = true;
        m_ff320->DataAddUpdate(Callback, this);
    }

}

//std::size_t DatarefManager::AddDataref(std::string name, AbstractDataref *dataref)
//{
//    if(m_datarefMap.contains(name))
//    {
//        m_logger.Log("Caution dataref storage already contain a dataref nammed : '" + name + "' overriding");
//    }
//    m_datarefMap.emplace(name, dataref);
//    m_logger.Log("Added dataref: '" + name + "' overriding");
//    return m_datarefMap.size();
//}

//std::size_t  DatarefManager::AddDataref(std::string path, std::string name, std::string conversionFactor, Dataref::Type type)
//{
//    if(m_datarefMap.contains(name))
//    {
//        m_logger.Log("Caution dataref storage already contain a dataref nammed : '" + name + "' overriding");
//    }
//    AbstractDataref* dataref;
//    if(path.find(".") != std::string::npos) //it's an FFDataref
//    {
//        dataref = new Dataref();
//        dataref->Load(path);
//        if(type == Dataref::Type::Unknown){
//            ((Dataref*)dataref)->LoadType();
//        }
//        else{
//            ((Dataref*)dataref)->SetType(type);
//        }
//    }
//    m_datarefMap.emplace(name, dataref);
//    return m_datarefMap.size();
//}

AbstractDataref *DatarefManager::GetDatarefByName(std::string name)
{
    AbstractDataref* dataref = nullptr;
    gLock.lock();
        if(!m_datarefMap.contains(name))
        {
            m_logger.Log("dataref nammed: '" + name + "' NOT FOUND!");
        }
        else 
        {
            dataref = m_datarefMap.at(name);
        }
    gLock.unlock();
    return dataref;
}

void DatarefManager::AddDatarefToMap(std::string name, AbstractDataref* dataref)
{
    gLock.lock();
        m_datarefMap.emplace(name, dataref);
    gLock.unlock();
}

void DatarefManager::AddMessageToQueue(Message m)
{
    gLock.lock();
        m_messageQueue.emplace(m);
    gLock.unlock();
}

Message DatarefManager::GetNextMessage()
{
    gLock.lock();
        Message message = m_messageQueue.back();
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

Message DatarefManager::GetNextMessageOut()
{
    gLock.lock();
        Message message = m_messageOutQueue.back();
        m_messageOutQueue.pop();
    gLock.unlock();
    return message;
}

void DatarefManager::AddMessageToOutQueue(Message m)
{
     gLock.lock();
        m_messageOutQueue.emplace(m);
    gLock.unlock();
}

std::size_t DatarefManager::GetMessageOutQueueLenght()
{
    gLock.lock();
        std::size_t lenght = m_messageOutQueue.size();
    gLock.unlock();
    return lenght;
}

std::queue<Message> DatarefManager::GetQueue()
{
    return std::queue<Message>(m_messageQueue);
}

Logger DatarefManager::GetLogger()
{
    return m_logger;
}

bool DatarefManager::isFF320Api()
{
    return m_isFF320Enable;
}

SharedValuesInterface* DatarefManager::GetFF320Interface()
{
    return m_ff320;
}
