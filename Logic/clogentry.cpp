
#include "clogentry.h"
#include <algorithm>
#include <QColor>

CLogEntry::CLogEntry() noexcept
    : m_time("")
    , m_level("")
    , m_procId("")
    , m_threadId("")
    , m_classname("")
    , m_funcName("")
    , m_description("")
    , m_bMarked(false)
    , m_ThdId(-1)
    , m_ProcId(-1)
    , m_uiTime(0)
    , m_ID(-1)
{
    CFuncTracer trace("CLogEntry::CLogEntry", m_trace);
}
CLogEntry::CLogEntry(std::shared_ptr<CTracer> tracer, const char *time, const char *level, const char *procId, const char *threadId, const char *classname, const char *func, const char *desc) noexcept
    : m_trace(tracer)
    , m_time(time)
    , m_level(level)
    , m_procId(procId)
    , m_threadId(threadId)
    , m_classname(classname)
    , m_funcName(func)
    , m_description(desc)
    , m_ReqText("")
    , m_bMarked(false)
    , m_bRequiredText(false)
    , m_bFunctionFiltered(false)
    , m_bClassFiltered(false)
    , m_bLevelFiltered(false)
    , m_bPIDFiltered(false)
    , m_bTIDFiltered(false)
{
    CFuncTracer trace("CLogEntry::CLogEntry", m_trace, false);
    m_ThdId = std::atoi(m_threadId.c_str());
    m_ProcId = std::atoi(m_procId.c_str());
    m_tracerLevel = Convert(m_level);
    m_uiTime = CLogEntry::GetUiTime(m_time);
    m_ID = m_genID++;

    m_hours = std::atoi(m_time.substr(0, m_time.find_first_of(":")).c_str());
    m_minutes = std::atoi(m_time.substr(m_time.find_first_of(":") + 1, m_time.find_last_of(":")).c_str());
    m_seconds = std::atoi(m_time.substr(m_time.find_last_of(":") + 1, m_time.find_first_of(".")).c_str());
    m_milliseconds = std::atoi(m_time.substr(m_time.find_last_of(".") + 1).c_str());
}
CLogEntry::CLogEntry(const CLogEntry &itm)
{
    CFuncTracer trace("CLogEntry::CLogEntry&", m_trace);
}
CLogEntry::CLogEntry(const CLogEntry &&itm)
{
    CFuncTracer trace("CLogEntry::CLogEntry&&", m_trace);
    m_trace = std::move(itm.m_trace);
    m_time = std::move(m_time);
    m_level = std::move(m_level);
    m_procId = std::move(m_procId);
    m_threadId = std::move(m_threadId);
    m_classname = std::move(m_classname);
    m_funcName = std::move(m_funcName);
    m_description = std::move(m_description);
    m_ReqText = std::move(m_ReqText);
    m_tracerLevel = std::move(m_tracerLevel);
    m_bMarked = std::move(m_bMarked);
    m_bRequiredText = std::move(m_bRequiredText);
    m_bFunctionFiltered = std::move(m_bFunctionFiltered);
    m_bClassFiltered = std::move(m_bClassFiltered);
    m_bLevelFiltered = std::move(m_bLevelFiltered);
    m_bPIDFiltered = std::move(m_bPIDFiltered);
    m_bTIDFiltered = std::move(m_bTIDFiltered);
    m_ThdId = std::move(m_ThdId);
    m_ProcId = std::move(m_ProcId);
    m_uiTime = std::move(m_uiTime);
    m_hours = std::move(m_hours);
    m_minutes = std::move(m_minutes);
    m_seconds = std::move(m_seconds);
    m_milliseconds = std::move(m_milliseconds);
    m_ID = std::move(m_ID);
}
CLogEntry& CLogEntry::operator=(const CLogEntry&& itm)
{
    CFuncTracer trace("CLogEntry::operator=&&", m_trace);
    if (this != &itm)
    {
        m_trace = std::move(itm.m_trace);
        m_time = std::move(m_time);
        m_level = std::move(m_level);
        m_procId = std::move(m_procId);
        m_threadId = std::move(m_threadId);
        m_classname = std::move(m_classname);
        m_funcName = std::move(m_funcName);
        m_description = std::move(m_description);
        m_ReqText = std::move(m_ReqText);
        m_tracerLevel = std::move(m_tracerLevel);
        m_bMarked = std::move(m_bMarked);
        m_bRequiredText = std::move(m_bRequiredText);
        m_bFunctionFiltered = std::move(m_bFunctionFiltered);
        m_bClassFiltered = std::move(m_bClassFiltered);
        m_bLevelFiltered = std::move(m_bLevelFiltered);
        m_bPIDFiltered = std::move(m_bPIDFiltered);
        m_bTIDFiltered = std::move(m_bTIDFiltered);
        m_ThdId = std::move(m_ThdId);
        m_ProcId = std::move(m_ProcId);
        m_uiTime = std::move(m_uiTime);
        m_hours = std::move(m_hours);
        m_minutes = std::move(m_minutes);
        m_seconds = std::move(m_seconds);
        m_milliseconds = std::move(m_milliseconds);
        m_ID = std::move(m_ID);
    }
    return *this;
}
CLogEntry& CLogEntry::operator=(const CLogEntry& itm)
{
    CFuncTracer trace("CLogEntry::operator=&", m_trace);
    if (this != &itm)
    {
        m_trace = itm.m_trace;
        m_time = itm.m_time;
        m_level = itm.m_level;
        m_procId = itm.m_procId;
        m_threadId = itm.m_threadId;
        m_classname = itm.m_classname;
        m_funcName = itm.m_funcName;
        m_description = itm.m_description;
        m_ReqText = itm.m_ReqText;
        m_tracerLevel = itm.m_tracerLevel;
        m_bMarked = itm.m_bMarked;
        m_bRequiredText = itm.m_bRequiredText;
        m_bFunctionFiltered = itm.m_bFunctionFiltered;
        m_bClassFiltered = itm.m_bClassFiltered;
        m_bLevelFiltered = itm.m_bLevelFiltered;
        m_bPIDFiltered = itm.m_bPIDFiltered;
        m_bTIDFiltered = itm.m_bTIDFiltered;
        m_ThdId = itm.m_ThdId;
        m_ProcId = itm.m_ProcId;
        m_uiTime = itm.m_uiTime;
        m_hours = itm.m_hours;
        m_minutes = itm.m_minutes;
        m_seconds = itm.m_seconds;
        m_milliseconds = itm.m_milliseconds;
        m_ID = itm.m_ID;
    }
    return *this;
}
CLogEntry::~CLogEntry()
{
    CFuncTracer trace("CLogEntry::~CLogEntry", m_trace, false);
}
QString CLogEntry::GetDataElement(int column) const
{
    CFuncTracer trace("CLogEntry::GetDataElement", m_trace);
    try
    {
        switch(column)
        {
            case eDateTime: return QString::fromStdString(Time());
            case eTraceLevel: return QString::fromStdString(Level());
            case eProc: return QString::fromStdString(ProcID());
            case eThread: return QString::fromStdString(ThreadID());
            case eClass: return QString::fromStdString(Class());
            case eFunction: return QString::fromStdString(FuncName());
            case eDescription: return QString::fromStdString(Description());

            case eNumOfColumns:
            default:
                break;
        }
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
    return QString("");
}
QColor CLogEntry::GetBackGroundColor(int column) const
{
    if (IsMarked())
        return QColor(212,235,242);
    return QColor(255,255,255);
}
TracerLevel CLogEntry::Convert(std::string level)    {
    if (level.find("TRACE") != std::string::npos) return TracerLevel::TRACER_DEBUG_LEVEL;
    else if (level.find("INFO") != std::string::npos) return TracerLevel::TRACER_INFO_LEVEL;
    else if (level.find("WARNING") != std::string::npos) return TracerLevel::TRACER_WARNING_LEVEL;
    else if (level.find("FATAL") != std::string::npos) return TracerLevel::TRACER_FATAL_ERROR_LEVEL;
    else if (level.find("ERROR") != std::string::npos) return TracerLevel::TRACER_ERROR_LEVEL;
    return TracerLevel::TRACER_OFF_LEVEL;
}
unsigned long long CLogEntry::GetUiTime(std::string sTime)
{
    int hours, minutes, milli, seconds;
    hours = std::atoi(sTime.substr(0, sTime.find_first_of(":")).c_str());
    minutes = std::atoi(sTime.substr(sTime.find_first_of(":") + 1, sTime.find_last_of(":")).c_str());
    seconds = std::atoi(sTime.substr(sTime.find_last_of(":") + 1, sTime.find_first_of(".")).c_str());
    milli = std::atoi(sTime.substr(sTime.find_last_of(".") + 1).c_str());
    return ((hours * 3600 + minutes * 60 + seconds) *1000 + milli);
}
CLogEntryContainer::CLogEntryContainer(std::shared_ptr<CTracer> tracer) noexcept
    : IDataContainer(tracer)
    , m_trace(tracer)
{
    CFuncTracer trace("CLogEntryContainer::CLogEntryContainer", m_trace);
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
CLogEntryContainer::~CLogEntryContainer()
{
    CFuncTracer trace("CLogEntryContainer::~CLogEntryContainer", m_trace);
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
void CLogEntryContainer::append(std::vector<CLogEntry> data)
{
    CFuncTracer trace("CLogEntryContainer::CLogEntryContainer", m_trace);
    try
    {
        m_entries.reserve((int)(data.max_size() + m_entries.size()));
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
void CLogEntryContainer::clear()
{
    CFuncTracer trace("CLogEntryContainer::CLogEntryContainer", m_trace);
    try
    {
        m_entries.clear();
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occured: %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occured");
    }
}
IData &CLogEntryContainer::GetData(int Row)
{
    CFuncTracer trace("CLogEntryContainer::GetData", m_trace);
    try
    {
        if (m_entries.size() > Row)
                return m_entries.at(Row);
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
    return IDataContainer::GetData(0);
}

void CLogEntryContainer::ToggleMark(int Row)
{
    CFuncTracer trace("CLogEntryContainer::ToggleMark", m_trace);
    try
    {
        if (m_entries[Row].IsMarked())
            m_entries[Row].SetMark( false );
        else
            m_entries[Row].SetMark( true );
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occured");
    }
}
