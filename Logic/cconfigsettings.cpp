#include "cconfigsettings.h"
#include "cfunctracer.h"

CConfigSettings::CConfigSettings(std::shared_ptr<CTracer> tracer, const std::string& filename)
    : m_trace(tracer)
    , m_sLogFilename(filename)
    , m_lines(0)
    , m_bConfigurationChanged(false)
{
    CFuncTracer trace("CConfigSettings::CConfigSettings", m_trace);
    try
    {
        m_sConfigFilename = filename.substr(0, filename.find_last_of('.'));
        m_sConfigFilename += ".xml";
        trace.Info("configfile : %s", m_sConfigFilename.c_str());
        loadConfigurations();
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
}
CConfigSettings::~CConfigSettings()
{
    CFuncTracer trace("CConfigSettings::~CConfigSettings", m_trace);
    try
    {
        if (m_bConfigurationChanged == true)
            storeConfigurations();

    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
}
void CConfigSettings::loadConfigurations(void)
{
    CFuncTracer trace("CConfigSettings::loadConfigurations", m_trace);
    try
    {

    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
}
void CConfigSettings::storeConfigurations(void)
{
    CFuncTracer trace("CConfigSettings::storeConfigurations", m_trace);
    try
    {
        m_bConfigurationChanged = false;
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
}

