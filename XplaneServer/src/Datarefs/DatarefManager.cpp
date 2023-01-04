#include "DatarefManager.h"

void Callback(double step, void* tag)
{
	DatarefManager* cm = (DatarefManager*)tag;
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
    }
}

int DatarefManager::AddDataref(std::string name, AbstractDataref *dataref)
{
    return 0;
}

int DatarefManager::AddDataref(std::string path, std::string name, std::string conversionFactor, Dataref::Type type)
{
    return 0;
}

AbstractDataref *DatarefManager::GetDatarefByName(std::string name)
{
    return nullptr;
}
