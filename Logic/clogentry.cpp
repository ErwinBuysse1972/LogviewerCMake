
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
CLogEntry::~CLogEntry()
{
    CFuncTracer trace("CLogEntry::~CLogEntry", m_trace, false);
}
QString CLogEntry::GetDataElement(int column) const
{
    CFuncTracer trace("CLogEntry::GetDataElement", m_trace, false);
    QString sValue = "";
    try
    {
        switch(column)
        {
            case eIndex: sValue = QString::fromStdString(Index()); break;
            case eDateTime: sValue = QString::fromStdString(Time()); break;
            case eTraceLevel: sValue =  QString::fromStdString(Level()); break;
            case eProc: sValue = QString::fromStdString(ProcID()); break;
            case eThread: sValue =  QString::fromStdString(ThreadID()); break;
            case eClass: sValue =  QString::fromStdString(Class()); break;
            case eFunction: sValue =  QString::fromStdString(FuncName()); break;
            case eDescription: sValue =  QString::fromStdString(Description()); break;

            case eNumOfColumns:
            default:
                break;
        }
        trace.Trace("%s",  sValue.toStdString().c_str());
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
    return sValue;
}
QColor CLogEntry::GetBackGroundColor(int column) const
{
    CFuncTracer trace("CLogEntry::GetBackGroundColor", m_trace, false);
    int red = 255;
    int green = 255;
    int blue = 255;
    if (IsMarked())
    {
        red = 212;
        green = 235;
        blue = 242;
    }
    trace.Trace("[%d - %p]Red : %ld, Green : %ld, Blue: %ld (Marked : %s)", GetID(), this, red, green, blue, IsMarked()?"MARKED" : "NOT_MARKED");
    return QColor(red,green,blue);
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
        m_mpColumns.clear();
        m_mpColumns[eColumns::eIndex] = "Index";
        m_mpColumns[eColumns::eDateTime] = "Date/Time";
        m_mpColumns[eColumns::eTraceLevel] = "Level";
        m_mpColumns[eColumns::eProc] = "PID";
        m_mpColumns[eColumns::eThread] = "TID";
        m_mpColumns[eColumns::eClass] = "Class";
        m_mpColumns[eColumns::eFunction] = "Function";
        m_mpColumns[eColumns::eDescription] = "Description";
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
void CLogEntryContainer::append(std::vector<IData *> data)
{
    CFuncTracer trace("CLogEntryContainer::CLogEntryContainer", m_trace);
    try
    {
        std::for_each(data.begin(), data.end(), [=](IData* item){
            CLogEntry* lpEntry = reinterpret_cast<CLogEntry*>(item);
            if (lpEntry != nullptr)
                m_entries.push_back(lpEntry);
        });
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
IData *CLogEntryContainer::GetData(int Row)
{
    CFuncTracer trace("CLogEntryContainer::GetData", m_trace, false);
    IData *ItemData = nullptr;
    try
    {
        trace.Trace("Row at %ld has index %s (Marked : %s)", Row, m_entries.at(Row)->Index().c_str(), m_entries.at(Row)->IsMarked() ? "MARKED" : "NOT MARKED");
        if (m_entries.size() > Row)
        {
            ItemData = m_entries.at(Row);
            trace.Trace("Address : %p", ItemData);
            return ItemData;
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
    return nullptr;
}
int CLogEntryContainer::RowCount(void)
{
    CFuncTracer trace("CLogEntryContainer::RowCount", m_trace, false);
    try
    {
        trace.Trace("Count : %ld", m_entries.size());
        return m_entries.size();
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred: %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
    return 0;
}
int CLogEntryContainer::ColumnCount(void)
{
    CFuncTracer trace("CLogEntryContainer::ColumnCount", m_trace, false);
    try
    {
        trace.Trace("Count : %ld", eColumns::eNumOfColumns);
        return eColumns::eNumOfColumns;
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
    return 0;
}
std::map<int, std::string> CLogEntryContainer::GetColumns(void)
{
    CFuncTracer trace("CLogEntryContainer::GetColumns", m_trace);
    return m_mpColumns;
}

long long CLogEntryContainer::GetNextToggleMark(int currentRow)
{
    CFuncTracer trace("CLogEntryContainer::GetNextToggleMark", m_trace, false);
    long long id = -1;
    try
    {
        if (currentRow < m_entries.size())
        {
            // From current position to the end of the file
            for (int idx = currentRow + 1; idx < m_entries.size(); idx++)
                if (m_entries[idx]->IsMarked() == true) return m_entries[idx]->GetID();

            // From the beginning of the file until the current position
            for (int idx = 0; idx < currentRow; idx++)
                if (m_entries[idx]->IsMarked() == true) return m_entries[idx]->GetID();

            if(id == -1)
                trace.Error("No next mark found!");
            else
                trace.Trace("Id : %ld", id);
        }
        else
            trace.Error("row is out of range: %d", currentRow);
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
    return id;
}

long long CLogEntryContainer::ToggleMark(int Row)
{
    CFuncTracer trace("CLogEntryContainer::ToggleMark", m_trace);
    long long id = -1;
    try
    {
        if (m_entries.size() > Row)
        {
            id = m_entries.at(Row)->GetID();
            if (m_entries.at(Row)->IsMarked())
            {
                trace.Trace("SetMark false at index %d", id);
                m_entries.at(Row)->SetMark( false );
            }
            else
            {
                trace.Trace("SetMark true at index %d", id);
                m_entries.at(Row)->SetMark( true );
            }

        }
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occured");
    }
    return id;
}
