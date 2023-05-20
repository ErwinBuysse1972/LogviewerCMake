#pragma once
#include <string>
#include <vector>
#include <map>
#include <QStringList>
#include <cconfigsettings.h>
#include <cfunctracer.h>
#include <ctracer.h>
#include <functional>
#include <QRunnable>
#include "clogentry.h"

class CLogFile
{
public:
    CLogFile(const char* file, std::shared_ptr<CTracer> tracer, bool bParse = true);
    CLogFile(const CLogFile& file);
    CLogFile(const CLogFile&& file);
    virtual ~CLogFile();

    CLogFile& operator=(CLogFile& file);
    CLogFile& operator=(CLogFile&& file);

    std::vector<IData *> GetEntries(void);
    std::map<std::string, bool> GetFunctions(void);
    std::map<std::string, bool> GetClasses(void);
    std::map<std::string, bool> GetTracelevels(void);
    std::map<std::string, bool> GetPIDs(void);
    std::map<std::string, bool> GetTIDs(void);

    void SetTimeFilter(std::string startTime, std::string endTime);
    void SetDescriptionFilter(std::vector<std::string> TextFilters, bool caseSensitive = true, bool wordOnly = false);
    void SetInverseDescriptionFilter(std::vector<std::string> TextFilter, bool caseSensitive = true, bool WordOnly = false);
    void SetLevelFilter(void);
    void SetThreadIdFilter(void);
    void SetProcIdFilter(void);
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
    void UpdatePID(bool bFiltered, const std::string& sPid);
    void UpdateTID(bool bFiltered, const std::string& sTid);
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
    void parse(void);
    void parse_MP(void);
    std::string Name(void) const{ return m_name;}
    void RegisterParsedData(std::function<void (std::vector<IData*> items)> event);

private:
    int m_parsedItemsBuffer;
    std::shared_ptr<CTracer> m_trace;
    std::string m_name;
    std::string m_saveFileName;
    std::recursive_mutex m_mutex;
    std::vector<IData *> m_logEntries;
    std::vector<IData *> m_filteredEntries;
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
    int m_CurrentParsedDataIndex;
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
    std::map<std::string, bool> m_PIDs;
    std::map<std::string, bool> m_TIDs;
    std::function<void (std::vector<IData*> items)> m_onParsedData;


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

class ParseFileTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    ParseFileTask(std::shared_ptr<CTracer> tracer, std::string filename, int trunkDataSize, QObject* parent=nullptr);
    ~ParseFileTask() = default;

    std::shared_ptr<CLogFile> GetFile(void) const { return m_file;}
    void RegisterOnParsedData(std::function<void(std::vector<IData*> items)> event){ m_onParsedData = event;}
    void RegisterOnParsedFinished(std::function<void(int items)> event){m_onParsedFinished = event;}
protected:
    void run();
private:
    int                         m_itemsParsed;
    std::shared_ptr<CTracer>    m_trace;
    std::shared_ptr<CLogFile>   m_file;
    std::function<void(std::vector<IData*> items)> m_onParsedData;
    std::function<void(int items)> m_onParsedFinished;
};
