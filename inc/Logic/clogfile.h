#pragma once
#include <string>
#include <vector>
#include <map>
#include <QStringList>
#include <cconfigsettings.h>
#include <cfunctracer.h>
#include <ctracer.h>

class CLogEntry
{
public:
    CLogEntry() noexcept
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

    }
    CLogEntry(std::shared_ptr<CTracer> tracer, const char *time, const char* level, const char *procId, const char *threadId, const char *classname,
              const char *func, const char* desc) noexcept
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
    bool operator>(const CLogEntry& item)
    {
        return m_uiTime > item.m_uiTime;
    }
    bool operator<(const CLogEntry& item)
    {
        return m_uiTime < item.m_uiTime;
    }
    bool operator>=(const CLogEntry& item)
    {
        return m_uiTime >= item.m_uiTime;
    }
    bool operator <=(const CLogEntry& item)
    {
        return m_uiTime <= item.m_uiTime;
    }
    bool operator==(const CLogEntry& item)
    {
        if (m_uiTime != item.m_uiTime)
            return false;
        return !strcmp(m_description.c_str(), item.m_description.c_str());
    }
    virtual ~CLogEntry()
    {
        CFuncTracer trace("CLogEntry::~CLogEntry", m_trace, false);
    }

    std::string Time(void) const { return m_time; }
    std::string Level(void) const { return m_level; }
    std::string ProcID(void) const { return m_procId; }

    std::string ThreadID(void) const { return m_threadId;}
    std::string Class(void) const { return m_classname; }
    std::string FuncName(void) const { return m_funcName; }
    std::string Description(void) const { return m_description; }
    long long GetID(void) const { return m_ID; }
    void AddDescription(const char *s){ m_description += s;}
    void SetMark(bool bmark){ m_bMarked = bmark;}
    void SetSearchMark(bool required, const std::string& text){ m_bRequiredText = required; m_ReqText = text;}
    void FilterFunction(bool bFiltered){ m_bFunctionFiltered = bFiltered;}
    void FilterClass(bool bFiltered){ m_bClassFiltered = bFiltered;}
    void FilterLevel(bool bFiltered){ m_bLevelFiltered = bFiltered;}

    TracerLevel GetLevel(void){ return m_tracerLevel;}
    int GetProcId(void) const { return m_ProcId;}
    int GetThreadId(void) const { return m_ThdId;}
    unsigned long long GetTime(void){ return m_uiTime;}
    int GetTimeHours(void){ return m_hours;}
    int GetTimeMinutes(void){ return m_minutes;}
    int GetTimeSeconds(void){ return m_seconds;}
    int GetTimeMilliseconds(void) { return m_milliseconds;}
    static TracerLevel Convert(std::string level)    {
        if (level.find("TRACE") != std::string::npos) return TracerLevel::TRACER_DEBUG_LEVEL;
        else if (level.find("INFO") != std::string::npos) return TracerLevel::TRACER_INFO_LEVEL;
        else if (level.find("WARNING") != std::string::npos) return TracerLevel::TRACER_WARNING_LEVEL;
        else if (level.find("FATAL") != std::string::npos) return TracerLevel::TRACER_FATAL_ERROR_LEVEL;
        else if (level.find("ERROR") != std::string::npos) return TracerLevel::TRACER_ERROR_LEVEL;
        return TracerLevel::TRACER_OFF_LEVEL;
    }
    unsigned long GetClassLength(){ return m_classname.length();}
    unsigned long GetFuncNameLength(){ return m_funcName.length();}
    unsigned long GetDescriptionLength(){ return m_description.length();}
    bool IsMarked(void) const{ return m_bMarked;}
    bool IsEntryRequired(void) const { return m_bRequiredText; }
    bool IsClassFiltered(void) const { return m_bClassFiltered; }
    bool IsFunctionFiltered(void) const{ return m_bFunctionFiltered;}
    bool IsLevelFiltered(void) const{ return m_bLevelFiltered;}

    std::string GetRequiredText(void) const{ return m_ReqText; }

    static unsigned long long GetUiTime(std::string sTime)
    {
        int hours, minutes, milli, seconds;
        hours = std::atoi(sTime.substr(0, sTime.find_first_of(":")).c_str());
        minutes = std::atoi(sTime.substr(sTime.find_first_of(":") + 1, sTime.find_last_of(":")).c_str());
        seconds = std::atoi(sTime.substr(sTime.find_last_of(":") + 1, sTime.find_first_of(".")).c_str());
        milli = std::atoi(sTime.substr(sTime.find_last_of(".") + 1).c_str());
        return ((hours * 3600 + minutes * 60 + seconds) *1000 + milli);
    }
private:
    static long long m_genID;
    std::shared_ptr<CTracer> m_trace;
    std::string m_time;
    std::string m_level;
    std::string m_procId;
    std::string m_threadId;
    std::string m_classname;
    std::string m_funcName;
    std::string m_description;
    std::string m_ReqText;
    TracerLevel m_tracerLevel;
    bool m_bMarked;
    bool m_bRequiredText;
    bool m_bFunctionFiltered;
    bool m_bClassFiltered;
    bool m_bLevelFiltered;
    int m_ThdId;
    int m_ProcId;
    unsigned long long m_uiTime;
    int m_hours;
    int m_minutes;
    int m_seconds;
    int m_milliseconds;
    long long m_ID;
};

class CLogFile
{
public:
    CLogFile(const char* file, std::shared_ptr<CTracer> tracer);
    CLogFile(const CLogFile& file);
    CLogFile(const CLogFile&& file);
    virtual ~CLogFile();

    CLogFile& operator=(CLogFile& file);
    CLogFile& operator=(CLogFile&& file);

    std::vector<CLogEntry> GetEntries(void);
    std::map<std::string, bool> GetFunctions(void);
    std::map<std::string, bool> GetClasses(void);
    std::map<std::string, bool> GetTracelevels(void);

    void SetTimeFilter(std::string startTime, std::string endTime);
    void SetDescriptionFilter(std::vector<std::string> TextFilters, bool caseSensitive = true, bool wordOnly = false);
    void SetInverseDescriptionFilter(std::vector<std::string> TextFilter, bool caseSensitive = true, bool WordOnly = false);
    void SetLevelFilter(void);
    void SetThreadIdFilter(std::vector<int> ThreadIdFilters);
    void SetProcIdFilter(std::vector<int> ProcIdFiters);
    void SetClassNameFilter(void);
    void SetFunctionFilter(void);
    void ClearFilter(void);
    void FunctionFilterClear(void);
    void LevelFilterClear(void);
    unsigned long GetMaxDescLength(){ return m_maxDescLength;}
    unsigned long GetMaxClassLength(){ return m_maxClassLength;}
    unsigned long GetMaxFuncLength(){ return m_maxFuncLength; }
    std::map<std::string, int> GetClassOccurences();
    std::map<std::string, int> GetFunctionOccurences();
    std::map<std::string, int> GetFunctionEntries();
    std::map<std::string, int> GetFunctionExits();

    void SetMark(long long id, bool bMark);
    void SetRequiredText(long long id, const std::string& text, bool bRequired );
    void UpdateClassFunctions(bool bFiltered, const std::string& classname);
    void UpdateFunctionName(bool bFiltered, const std::string& fullFunctionName);
    void UpdateLevel(bool bFiltered, const std::string& sLevel);
    bool Save(const char *filename);

    bool IsTimeAvailable(void){ return (m_TimeIdx >= 0);}
    bool IsLevelAvailable(void){ return (m_LevelIdx >= 0);}
    bool IsProcIdAvailable(void){ return (m_ProdIdIdx >= 0);}
    bool IsThreadIdAvailable(void){ return (m_ThreadIdIdx >= 0);}
    bool IsClassNameAvailable(void){ return (m_ClassNameIdx >= 0);}
    bool IsFuncNameAvailable(void){ return (m_FuncIdx >= 0);}
    bool IsDescriptionAvailable(void){ return (m_DescIdx >= 0);}
    int GetNumberLines(void){ return m_logEntries.size();}
    int GetNumberFilteredLines(void){ return m_filteredEntries.size();}
    std::string Name(void) const{ return m_name;}

private:
    std::shared_ptr<CTracer> m_trace;
    std::string m_name;
    std::string m_saveFileName;
    std::vector<CLogEntry> m_logEntries;
    std::vector<CLogEntry> m_filteredEntries;
    std::string m_sLine;
    std::string m_sTime = "";
    std::string m_sLevel = "";
    std::string m_sProcId = "";
    std::string m_sThreadId = "";
    std::string m_sClassname = "";
    std::string m_sFuncName = "";
    std::string m_sDescription = "";
    bool m_bDetectedFormat;
    int m_TimeIdx;
    int m_LevelIdx;
    int m_ProdIdIdx;
    int m_ThreadIdIdx;
    int m_ClassNameIdx;
    int m_FuncIdx;
    int m_DescIdx;
    unsigned long m_maxDescLength;
    unsigned long m_maxClassLength;
    unsigned long m_maxFuncLength;
    std::shared_ptr<CConfigSettings> m_SPConfigSettings;

    std::map<std::string, int> m_ClassOccurences;
    std::map<std::string, int> m_FunctionOccurences;
    std::map<std::string, int> m_FunctionEntries;
    std::map<std::string, int> m_FunctionExits;
    std::map<std::string, bool> m_Functions;
    std::map<std::string, bool> m_Classes;
    std::map<std::string, bool> m_TraceLevels;

    void parse(void);
    void parse_MP(void);
    void automaticDetectionFormat_MP(QStringList fields);
    void automaticDetectFormat(std::vector<std::string>& fields);
    bool isTraceLevelValid(const char *level);
    void saveXmlBegin(void);
    void saveCsvEntry(void);
    void saveJsonEntry(void);
    void saveXmlEntry(void);
    void saveTextEntry(void);
    void UpdateConfigSettings(void);
    int getNrOfLines(const std::string& file);
    int getFileSize(FILE* fp);
};
