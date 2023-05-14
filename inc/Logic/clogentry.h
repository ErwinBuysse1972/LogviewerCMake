#pragma once
#include "IDataObject.h"
#include <string>
#include <vector>
#include <map>
#include <QStringList>
#include <cconfigsettings.h>
#include <cfunctracer.h>
#include <ctracer.h>

enum eColumns : int
{
    eDateTime = 0,
    eTraceLevel = 1,
    eProc = 2,
    eThread = 3,
    eClass = 4,
    eFunction = 5,
    eDescription = 6,
    eNumOfColumns = 7
};

class CLogEntry : public IData
{
public:
    CLogEntry() noexcept;
    CLogEntry(std::shared_ptr<CTracer> tracer, const char *time, const char* level, const char *procId, const char *threadId, const char *classname,
              const char *func, const char* desc) noexcept;
    CLogEntry(const CLogEntry& itm);
    CLogEntry(const CLogEntry&& itm);
    virtual ~CLogEntry();
    CLogEntry& operator=(const CLogEntry& itm);
    CLogEntry& operator=(const CLogEntry&& itm);

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
    void FilterPID(bool bFiltered){ m_bPIDFiltered = bFiltered;}
    void FilterTID(bool bFiltered){ m_bTIDFiltered = bFiltered;}
    TracerLevel GetLevel(void){ return m_tracerLevel;}
    int GetProcId(void) const { return m_ProcId;}
    int GetThreadId(void) const { return m_ThdId;}
    unsigned long long GetTime(void){ return m_uiTime;}
    int GetTimeHours(void){ return m_hours;}
    int GetTimeMinutes(void){ return m_minutes;}
    int GetTimeSeconds(void){ return m_seconds;}
    int GetTimeMilliseconds(void) { return m_milliseconds;}
    unsigned long GetClassLength(){ return m_classname.length();}
    unsigned long GetFuncNameLength(){ return m_funcName.length();}
    unsigned long GetDescriptionLength(){ return m_description.length();}
    bool IsMarked(void) const{ return m_bMarked;}
    bool IsEntryRequired(void) const { return m_bRequiredText; }
    bool IsClassFiltered(void) const { return m_bClassFiltered; }
    bool IsFunctionFiltered(void) const{ return m_bFunctionFiltered;}
    bool IsLevelFiltered(void) const{ return m_bLevelFiltered;}
    bool IsPIDFiltered(void) const { return m_bPIDFiltered;}
    bool IsTIDFiltered(void) const { return m_bTIDFiltered;}

    std::string GetRequiredText(void) const{ return m_ReqText; }
    QString GetDataElement(int column) const override;
    QColor GetBackGroundColor(int column) const override;

    static TracerLevel Convert(std::string level);
    static unsigned long long GetUiTime(std::string sTime);
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
    bool m_bPIDFiltered;
    bool m_bTIDFiltered;
    int m_ThdId;
    int m_ProcId;
    unsigned long long m_uiTime;
    int m_hours;
    int m_minutes;
    int m_seconds;
    int m_milliseconds;
    long long m_ID;
};

class CLogEntryContainer : IDataContainer
{
public:
    CLogEntryContainer(std::shared_ptr<CTracer> tracer) noexcept;
    ~CLogEntryContainer();
    CLogEntryContainer(CLogEntryContainer& itm)= default;
    CLogEntryContainer(CLogEntryContainer&& itm) = default;
    CLogEntryContainer& operator=(CLogEntryContainer& itm) = default;
    CLogEntryContainer& operator=(CLogEntryContainer&& itm) = default;



    void append(std::vector<CLogEntry> data);
    void clear();
    IData& GetData(int Row) override;
    void ToggleMark(int Row) override;

private:
    std::vector<CLogEntry> m_entries;
    CLogEntry nullRef;
    std::shared_ptr<CTracer> m_trace;
};
