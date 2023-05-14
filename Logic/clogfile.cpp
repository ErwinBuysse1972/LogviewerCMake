#include <clogfile.h>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QDataStream>
#include <QTextStream>
#include <vector>
#include <exception>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <stdio.h>
#include <memory>
#include <cfunctracer.h>
#include <cscopedtimer.h>
#include <cconversion.h>
#include <cconfigsettings.h>

long long CLogEntry::m_genID = 0;


CLogFile::CLogFile(const char* file, std::shared_ptr<CTracer> tracer)
    : m_trace(tracer)
    , m_name(file)
    , m_sLine("")
    , m_sTime("")
    , m_sLevel("")
    , m_sProcId("")
    , m_sThreadId("")
    , m_sClassname("")
    , m_sFuncName("")
    , m_sDescription("")
    , m_bDetectedFormat(false)
    , m_TimeIdx(-1)
    , m_LevelIdx(-1)
    , m_ProdIdIdx(-1)
    , m_ThreadIdIdx(-1)
    , m_ClassNameIdx(-1)
    , m_FuncIdx(-1)
    , m_DescIdx(-1)
    , m_maxDescLength(0)
    , m_maxClassLength(0)
    , m_maxFuncLength(0)
    , m_SPConfigSettings(std::make_shared<CConfigSettings>(m_trace, m_name))
{
    CFuncTracer trace("CLogFile::CLogFile", m_trace);
    CScopeTimer timer("CLogFile::CLogFile", 0, [=, &trace](const std::string& msg){
        trace.Info("Timings:");
        trace.Info("    %s", msg.c_str());
    });
    try
    {
        timer.SetTime("00");
        int iLines = m_SPConfigSettings->lines();

        if (iLines <= 0)
            getNrOfLines(file);
        else
            UpdateConfigSettings();

        timer.SetTime("01");
        trace.Info("File : %s (lines : %ld)", file, iLines);
        timer.SetTime("02");
        m_logEntries.reserve(iLines);
        timer.SetTime("03");
        m_filteredEntries.reserve(iLines);
        timer.SetTime("04");
        parse_MP();
        timer.SetTime("05");
        trace.Info("entries: %ld", m_logEntries.size());
        timer.SetTime("06");
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
CLogFile::CLogFile(const CLogFile& file)
    : m_trace(file.m_trace)
    , m_name(file.m_name)
    , m_sLine(file.m_sLine)
    , m_sTime(file.m_sTime)
    , m_sLevel(file.m_sLevel)
    , m_sProcId(file.m_sProcId)
    , m_sThreadId(file.m_sThreadId)
    , m_sClassname(file.m_sClassname)
    , m_sFuncName(file.m_sFuncName)
    , m_sDescription(file.m_sDescription)
    , m_bDetectedFormat(file.m_bDetectedFormat)
    , m_TimeIdx(file.m_TimeIdx)
    , m_LevelIdx(file.m_LevelIdx)
    , m_ProdIdIdx(file.m_ProdIdIdx)
    , m_ThreadIdIdx(file.m_ThreadIdIdx)
    , m_ClassNameIdx(file.m_ClassNameIdx)
    , m_FuncIdx(file.m_FuncIdx)
    , m_DescIdx(file.m_DescIdx)
    , m_maxDescLength(file.m_maxDescLength)
    , m_maxClassLength(file.m_maxClassLength)
    , m_maxFuncLength(file.m_maxFuncLength)
{
    CFuncTracer trace("CLogFile::CLogFile(COPY)", file.m_trace);
    try
    {
        std::copy(file.m_logEntries.begin(), file.m_logEntries.end(), std::back_inserter(m_logEntries));
        std::copy(file.m_filteredEntries.begin(), file.m_filteredEntries.end(), std::back_inserter(m_filteredEntries));
        m_ClassOccurences.insert(file.m_ClassOccurences.begin(), file.m_ClassOccurences.end());
        m_FunctionOccurences.insert(file.m_FunctionOccurences.begin(), file.m_FunctionOccurences.end());
        m_FunctionEntries.insert(file.m_FunctionEntries.begin(), file.m_FunctionEntries.end());
        m_FunctionExits.insert(file.m_FunctionExits.begin(), file.m_FunctionExits.end());
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

CLogFile::CLogFile(const CLogFile &&file)
{
    CFuncTracer trace("CLogFile::CLogFile(MOVE)", file.m_trace);
    try
    {
        m_name = std::move(file.m_name);
        m_sLine = std::move(file.m_sLine);
        m_sTime = std::move(file.m_sTime);
        m_sLevel = std::move(file.m_sLevel);
        m_sProcId = std::move(file.m_sProcId);
        m_sThreadId = std::move(file.m_sThreadId);
        m_sClassname = std::move(file.m_sClassname);
        m_sFuncName = std::move(file.m_sFuncName);
        m_sDescription = std::move(file.m_sDescription);
        m_TimeIdx = std::move(file.m_TimeIdx);
        m_LevelIdx = std::move(file.m_LevelIdx);
        m_ProdIdIdx = std::move(file.m_ProdIdIdx);
        m_ThreadIdIdx = std::move(file.m_ThreadIdIdx);
        m_ClassNameIdx = std::move(file.m_ClassNameIdx);
        m_FuncIdx = std::move(file.m_FuncIdx);
        m_DescIdx = std::move(file.m_DescIdx);
        m_maxDescLength = std::move(file.m_maxDescLength);
        m_maxClassLength = std::move(file.m_maxClassLength);
        m_maxFuncLength = std::move(file.m_maxFuncLength);
        m_logEntries = std::move(file.m_logEntries);
        m_filteredEntries = std::move(file.m_filteredEntries);
        m_ClassOccurences.insert(file.m_ClassOccurences.begin(), file.m_ClassOccurences.end());
        m_FunctionOccurences.insert(file.m_FunctionOccurences.begin(), file.m_FunctionOccurences.end());
        m_FunctionEntries.insert(file.m_FunctionEntries.begin(), file.m_FunctionEntries.end());
        m_FunctionExits.insert(file.m_FunctionExits.begin(), file.m_FunctionExits.end());
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
CLogFile::~CLogFile()
{
    CFuncTracer trace("CLogFile::~CLogFile", m_trace);
    try
    {
        m_logEntries.clear();
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
std::vector<CLogEntry>& CLogFile::GetEntries(void)
{
    CFuncTracer trace("CLogFile::GetEntries", m_trace);
    return m_filteredEntries;
}
std::map<std::string, bool> CLogFile::GetFunctions(void)
{
    CFuncTracer trace("CLogFile::GetFunctions", m_trace);
    try
    {
        if (m_Functions.size() == 0)
        {
            std::for_each(m_filteredEntries.begin(), m_filteredEntries.end(), [=](CLogEntry& entry){
                std::string fullFunctionName = entry.Class() + "::" + entry.FuncName();
                if (m_Functions.find(fullFunctionName) == m_Functions.end())
                    m_Functions.insert(std::make_pair(fullFunctionName, (entry.IsFunctionFiltered() == false)));
            });
            std::for_each(m_logEntries.begin(), m_logEntries.end(), [=](CLogEntry& entry){
                std::string fullFunctionName = entry.Class() + "::" + entry.FuncName();
                if (m_Functions.find(fullFunctionName) == m_Functions.end())
                    m_Functions.insert(std::make_pair(fullFunctionName, false));
            });
        }

        for (std::pair<std::string, bool> p : m_Functions)
            trace.Trace("%s : %s", p.first.c_str(), (p.second == true)?"CHECKED":"NOT_CHECKED");
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }

    return m_Functions;
}
std::map<std::string, bool> CLogFile::GetClasses(void)
{
    CFuncTracer trace("CLogFile::GetClasses", m_trace);
    CScopeTimer timer("CLogFile::GetClasses", 0, [=, &trace](const std::string& msg){
        trace.Info("Timings:");
        trace.Info("    %s", msg.c_str());
    });
    try
    {
        timer.SetTime("start");
        if (m_Classes.size() == 0)
        {
            timer.SetTime("add m_filteredEntries classes");
            std::for_each(m_filteredEntries.begin(),
                          m_filteredEntries.end(),
                          [=](CLogEntry& entry)
                          {
                            if (m_Classes.find(entry.Class()) == m_Classes.end())
                                m_Classes.insert(std::make_pair(entry.Class(), (entry.IsClassFiltered() == false)));
            });

            if (m_filteredEntries.size() < m_logEntries.size())
            {
                timer.SetTime("add m_logEntries classes");
                std::for_each(m_logEntries.begin(),
                              m_logEntries.end(),
                              [=](CLogEntry &entry)
                              {
                                if (m_Classes.find(entry.Class()) == m_Classes.end())
                                        m_Classes.insert(std::make_pair(entry.Class(), false));
                });
            }
            timer.SetTime("End");
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

    return m_Classes;
}
std::map<std::string, bool> CLogFile::GetTracelevels(void)
{
    CFuncTracer trace("CLogFile::GetTracelevels", m_trace);


    try
    {
        if (m_TraceLevels.size() == 0)
        {

            std::for_each(m_filteredEntries.begin(), m_filteredEntries.end(), [=](CLogEntry& entry){
                if (m_TraceLevels.find(entry.Level()) == m_TraceLevels.end())
                    m_TraceLevels.insert(std::make_pair(entry.Level(), true));
            });
            // Mark the tracelevels false if they do not occurs in the filter list
            std::for_each(m_logEntries.begin(), m_logEntries.end(), [=](CLogEntry& entry){
                if (m_TraceLevels.find(entry.Level()) == m_TraceLevels.end())
                    m_TraceLevels.insert(std::make_pair(entry.Level(), false));
            });
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

    return m_TraceLevels;
}
std::map<std::string, bool> CLogFile::GetPIDs(void)
{
    CFuncTracer trace("CLogFile::GetPIDs", m_trace);
    try
    {
        if (m_PIDs.size() == 0)
        {
            std::for_each(m_filteredEntries.begin(), m_filteredEntries.end(), [=](CLogEntry& entry){
                if (m_PIDs.find(entry.ProcID()) == m_PIDs.end())
                    m_PIDs.insert(std::make_pair(entry.ProcID(), true));
            });
            // Mark the tracelevels false if they do not occurs in the filter list
            std::for_each(m_logEntries.begin(), m_logEntries.end(), [=](CLogEntry& entry){
                if (m_PIDs.find(entry.ProcID()) == m_PIDs.end())
                    m_PIDs.insert(std::make_pair(entry.ProcID(), false));
            });
        }
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred: %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
    return m_PIDs;
}
std::map<std::string, bool> CLogFile::GetTIDs(void)
{
    CFuncTracer trace("CLogFile::GetTIDs", m_trace);
    try
    {
        if (m_TIDs.size() == 0)
        {
            std::for_each(m_filteredEntries.begin(), m_filteredEntries.end(), [=](CLogEntry& entry){
                if (m_TIDs.find(entry.ThreadID()) == m_TIDs.end())
                    m_TIDs.insert(std::make_pair(entry.ThreadID(), true));
            });
            // Mark the tracelevels false if they do not occurs in the filter list
            std::for_each(m_logEntries.begin(), m_logEntries.end(), [=](CLogEntry& entry){
                if (m_TIDs.find(entry.ThreadID()) == m_TIDs.end())
                    m_TIDs.insert(std::make_pair(entry.ThreadID(), false));
            });
        }

    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred: %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
    return m_TIDs;
}

void CLogFile::SetTimeFilter(std::string startTime, std::string endTime)
{
    CFuncTracer trace("CLogFile::SetTimeFilter", m_trace);
    try
    {
        unsigned long long uiStartTime = CLogEntry::GetUiTime(startTime);
        unsigned long long uiEndTime = CLogEntry::GetUiTime(endTime);
        if (uiStartTime >= uiEndTime)
        {
            trace.Error("StartTime is bigger then end time");
            return;
        }

        int prevSize = m_filteredEntries.size();

        m_filteredEntries.erase(std::remove_if(m_filteredEntries.begin(),
                                               m_filteredEntries.end(),
                                               [=, &uiStartTime, &uiEndTime](CLogEntry& entry){
                                    return (  (entry.GetTime() < uiStartTime)
                                            ||(entry.GetTime() > uiEndTime));
                               }), m_filteredEntries.end());

        trace.Trace("Number of entries removed: %ld, total entries: %ld"
                    , (prevSize - m_filteredEntries.size())
                    , m_filteredEntries.size());
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
void CLogFile::SetDescriptionFilter(std::vector<std::string> TextFilters, bool caseSensitive, bool wordOnly)
{
    CFuncTracer trace("CLogFile::SetDescriptionFilter", m_trace);
    try
    {
        std::vector<std::string> filter;
        for(std::string& s : TextFilters)
        {
            if (caseSensitive == false)
                transform(s.begin(), s.end(), s.begin(), ::tolower);

            if (wordOnly == true)
                s = " " + s + " ";
            filter.push_back(s);
        }
        m_filteredEntries.erase(std::remove_if(m_filteredEntries.begin(), m_filteredEntries.end(),[=, &caseSensitive](CLogEntry& entry){
                                    std::string sDesc = entry.Description();
                                    if (caseSensitive == false)
                                        transform(sDesc.begin(), sDesc.end(), sDesc.begin(), ::tolower);

                                    return !InverseContains<std::vector<std::string>>(filter, sDesc);
                                }), m_filteredEntries.end());
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
void CLogFile::SetInverseDescriptionFilter(std::vector<std::string> TextFilters, bool caseSensitive, bool wordOnly)
{
    CFuncTracer trace("CLogFile::SetInverseDescriptionFilter", m_trace);
    try
    {
        std::vector<std::string> filter;
        for(std::string& s : TextFilters)
        {
            if (caseSensitive == false)
                transform(s.begin(), s.end(), s.begin(), ::tolower);

            if (wordOnly == true)
                s = " " + s + " ";
            filter.push_back(s);
        }
        m_filteredEntries.erase(std::remove_if(m_filteredEntries.begin(), m_filteredEntries.end(),[=, &caseSensitive](CLogEntry& entry){
                                    std::string sDesc = entry.Description();
                                    if (caseSensitive == false)
                                        transform(sDesc.begin(), sDesc.end(), sDesc.begin(), ::tolower);

                                    return InverseContains<std::vector<std::string>>(filter, sDesc);
                                }), m_filteredEntries.end());

    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred: %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }

}
void CLogFile::SetLevelFilter()
{
    CFuncTracer trace("CLogFile::SetLevelFilter", m_trace);
    try
    {
        int prevSize = m_filteredEntries.size();
        m_filteredEntries.erase(std::remove_if(m_filteredEntries.begin(),
                                               m_filteredEntries.end(),
                                               [=](CLogEntry &entry) {
                                                   return entry.IsLevelFiltered();
                                               }),
                                m_filteredEntries.end());
        trace.Trace("Number of entries removed: %ld, total entries: %ld"
                    , (prevSize - m_filteredEntries.size())
                    , m_filteredEntries.size());

    }
    catch (std::exception &ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }

}
void CLogFile::SetThreadIdFilter(void)
{
    CFuncTracer trace("CLogFile::SetThreadIdFilter", m_trace);
    try
    {
        int prevSize = m_filteredEntries.size();
        m_filteredEntries.erase(std::remove_if(m_filteredEntries.begin(),
                                               m_filteredEntries.end(),
                                               [=](CLogEntry &entry) {
                                                   return entry.IsTIDFiltered();
                                               }),
                                m_filteredEntries.end());
        trace.Trace("Number of entries removed: %ld, total entries: %ld"
                    , (prevSize - m_filteredEntries.size())
                    , m_filteredEntries.size());
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
void CLogFile::SetProcIdFilter(void)
{
    CFuncTracer trace("CLogFile::SetProcIdFilter", m_trace);
    try
    {
        int prevSize = m_filteredEntries.size();
        m_filteredEntries.erase(std::remove_if(m_filteredEntries.begin(),
                                               m_filteredEntries.end(),
                                               [=](CLogEntry &entry) {
                                                   return entry.IsPIDFiltered();
                                               }),
                                m_filteredEntries.end());
        trace.Trace("Number of entries removed: %ld, total entries: %ld"
                    , (prevSize - m_filteredEntries.size())
                    , m_filteredEntries.size());
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
void CLogFile::SetClassNameFilter(void)
{
    CFuncTracer trace("CLogFile::SetClassNameFilter", m_trace);
    try
    {
        m_filteredEntries.erase(std::remove_if(m_filteredEntries.begin(), m_filteredEntries.end(), [=](CLogEntry& entry){
                                    return entry.IsClassFiltered();
                                }), m_filteredEntries.end());
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
void CLogFile::SetFunctionFilter(void)
{
    CFuncTracer trace("CLogFile::SetFunctionFilter", m_trace);
    try
    {
        m_filteredEntries.erase(std::remove_if(m_filteredEntries.begin(), m_filteredEntries.end(), [=](CLogEntry& entry){
                                    return entry.IsFunctionFiltered();
                                }), m_filteredEntries.end());
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
void CLogFile::ClearFilter(void)
{
    CFuncTracer trace("CLogFile::ClearFilter", m_trace);
    try
    {
        m_filteredEntries.clear();
        std::copy(m_logEntries.begin(), m_logEntries.end(),
                  std::back_inserter(m_filteredEntries));
    }
    catch (std::exception &ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }

}
void CLogFile::FunctionFilterClear(void)
{
    CFuncTracer trace("CLogFile::FunctionFilterClear", m_trace);
    try
    {
        if (m_Classes.size() == 0)
            GetClasses();
        if (m_Functions.size() == 0)
            GetFunctions();

        for(std::pair<std::string, bool> classes : m_Classes)
            UpdateClassFunctions(false, classes.first);

        for (std::pair<std::string, bool> functions : m_Functions)
            UpdateFunctionName(false, functions.first);
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
void CLogFile::LevelFilterClear(void)
{
    CFuncTracer trace("CLogFile::LevelFilterClear", m_trace);
    try
    {
        if (m_TraceLevels.size() == 0)
            GetTracelevels();
        for (std::pair<std::string, bool> tracelevel : m_TraceLevels)
            UpdateLevel(false, tracelevel.first);
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
void CLogFile::UpdateClassFunctions(bool bFiltered, const std::string& classname)
{
    CFuncTracer trace("CLogFile::UpdateClassFunctions", m_trace);
    int iCountFunctions = 0;
    int iCountClass = 0;
    try
    {
        trace.Trace("classname : %s, bFiltered : %s", classname.c_str(), (bFiltered == true)?"TRUE":"FALSE");
        if (m_Functions.size() == 0)
            GetFunctions();

        //Update the functions
        std::for_each(m_Functions.begin(),
                      m_Functions.end(),
                      [=, &classname, &bFiltered, &trace, &iCountFunctions](std::pair<std::string, bool> p) {
                          std::string sFind = classname + "::";
                          if (p.first.find(sFind) != std::string::npos) {
                              bool bvalue = (bFiltered == false);
                              trace.Trace("function(%s) found (%s) -> bFiltered: %s",
                                          p.first.c_str(),
                                          sFind.c_str(),
                                          (bvalue == true) ? "NOT FILTERED" : "FILTERED");
                              m_Functions[p.first] = bvalue;
                              iCountFunctions ++;
                          }
                      });
        // Update the class
        if (m_Classes.find(classname) != m_Classes.end())
        {
            m_Classes[classname] = (bFiltered == false);
        }

        // Update the filtered entries (IsFunctionFiltered, IsClassFiltered)
        std::for_each(m_logEntries.begin(),
                      m_logEntries.end(),
                      [=, &bFiltered, &classname, &iCountClass](CLogEntry &entry) {
                          if (entry.Class().find(classname) != std::string::npos) {
                              entry.FilterClass(bFiltered);
                              entry.FilterFunction(bFiltered);
                              iCountClass++;
                          }
                      });

        trace.Trace("iCountFunctions : %ld, iCountClass : %ld", iCountFunctions, iCountClass);
    }
    catch (std::exception &ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }

}
void CLogFile::UpdateFunctionName(bool bFiltered, const std::string& fullFunctionName)
{
    CFuncTracer trace("CLogFile::UpdateClassNames", m_trace);
    int iCount = 0;
    try
    {
        trace.Trace("FunctionName : %s, bFiltered : %s", fullFunctionName.c_str(), (bFiltered == true)?"TRUE" : "FALSE");
        if (m_Functions.size() == 0)
            GetFunctions();

        if (m_Functions.find(fullFunctionName) != m_Functions.end())
            m_Functions[fullFunctionName] = (bFiltered == false);

        // Update the filtered entries (IsFunctionFiltered)
        std::for_each(m_logEntries.begin(),
                      m_logEntries.end(),
                      [=, &bFiltered, &fullFunctionName, &iCount](CLogEntry &entry) {
                          if ((fullFunctionName.find(entry.Class()) != std::string::npos)
                              && (fullFunctionName.find(entry.FuncName()) != std::string::npos)) {
                              entry.FilterFunction(bFiltered);
                              iCount++;
                          }
                      });
        trace.Trace("iCount : %ld", iCount);
    }
    catch (std::exception &ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }

}
void CLogFile::UpdateLevel(bool bFiltered, const std::string& sLevel)
{
    CFuncTracer trace("CLogFile::UpdateLevel", m_trace);
    int iCount = 0;
    try
    {
        trace.Trace("Level : %s, bFiltered : %s", sLevel.c_str(), (bFiltered == true)?"TRUE" : "FALSE");
        if (m_TraceLevels.size() == 0)
            GetTracelevels();

        if (m_TraceLevels.find(sLevel) != m_TraceLevels.end())
            m_TraceLevels[sLevel] = (bFiltered == false);

        // Update the filtered entries (IsLevelFiltered)
        std::for_each(m_logEntries.begin(),
                      m_logEntries.end(),
                      [=, &bFiltered, &sLevel, &iCount](CLogEntry& entry) {
                          if (entry.Level().find(sLevel) != std::string::npos) {
                              entry.FilterLevel(bFiltered);
                              iCount++;
                          }
                      });
        trace.Trace("iCount : %ld", iCount);
    }
    catch (std::exception &ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
}
void CLogFile::UpdatePID(bool bFiltered, const std::string& sPid)
{
    CFuncTracer trace("CLogFile::UpdatePID", m_trace);
    int iCount = 0;
    try
    {
        trace.Trace("PID : %s, bFiltered : %s", sPid.c_str(), (bFiltered == true)?"TRUE" : "FALSE");
        if (m_PIDs.size() == 0)
            GetPIDs();

        if (m_PIDs.find(sPid) != m_PIDs.end())
            m_PIDs[sPid] = (bFiltered == false);

        // Update the filtered entries (IsLevelFiltered)
        std::for_each(m_logEntries.begin(),
                      m_logEntries.end(),
                      [=, &bFiltered, &sPid, &iCount](CLogEntry& entry) {
                          if (entry.ProcID().find(sPid) != std::string::npos) {
                              entry.FilterPID(bFiltered);
                              iCount++;
                          }
                      });
        trace.Trace("iCount : %ld", iCount);

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
void CLogFile::UpdateTID(bool bFiltered, const std::string& sTid)
{
    CFuncTracer trace("CLogFile::UpdateTID", m_trace);
    int iCount = 0;
    try
    {
        trace.Trace("TID : %s, bFiltered : %s", sTid.c_str(), (bFiltered == true)?"TRUE" : "FALSE");
        if (m_TIDs.size() == 0)
            GetTIDs();

        if (m_TIDs.find(sTid) != m_TIDs.end())
            m_TIDs[sTid] = (bFiltered == false);

        // Update the filtered entries (IsLevelFiltered)
        std::for_each(m_logEntries.begin(),
                      m_logEntries.end(),
                      [=, &bFiltered, &sTid, &iCount](CLogEntry& entry) {
                          if (entry.ThreadID().find(sTid) != std::string::npos) {
                              entry.FilterTID(bFiltered);
                              iCount++;
                          }
                      });
        trace.Trace("iCount : %ld", iCount);
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

bool CLogFile::Save(const char *filename)
{
    CFuncTracer trace("CLogFile::Save", m_trace);
    try
    {
        std::ofstream file(filename, std::fstream::out|std::fstream::trunc );
        std::for_each(m_filteredEntries.begin(), m_filteredEntries.end(), [=, &file](CLogEntry& entry){
            if (IsTimeAvailable())
                file << entry.Time() << " ";
            if (IsLevelAvailable())
                 file << entry.Level() << "\t";
            if (IsProcIdAvailable() || IsThreadIdAvailable())
                 file << "[" << std::to_string(entry.GetProcId()) << ":" << std::to_string(entry.GetThreadId()) << "]";
            file << "  -  ";
            if (IsFuncNameAvailable())
                 file << entry.Class() << "::" << entry.FuncName();
            file << " : ";
            if (IsDescriptionAvailable())
                file << entry.Description();
            file << std::endl;
        });
        file.flush();
        file.close();
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
    return false;
}
std::map<std::string, int> CLogFile::GetClassOccurences()
{
    CFuncTracer trace("CLogFile::GetClassOccurences", m_trace);
    try
    {
        if (m_ClassOccurences.size() > 0)
            return m_ClassOccurences;

        for_each(m_logEntries.begin(), m_logEntries.end(), [=](CLogEntry& entry){
            if (m_ClassOccurences.find(entry.Class()) == m_ClassOccurences.end())
                m_ClassOccurences.insert(std::make_pair(entry.Class(), 1));
            else
                m_ClassOccurences[entry.Class()] += 1;
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
    return m_ClassOccurences;
}
std::map<std::string, int> CLogFile::GetFunctionOccurences()
{
    CFuncTracer trace("CLogFile::GetFunctionOccurences", m_trace);
    try
    {
        if (m_FunctionOccurences.size() > 0)
            return m_FunctionOccurences;

        for_each(m_logEntries.begin(), m_logEntries.end(), [=](CLogEntry& entry){
            std::string fullFunction = entry.Class() + "::" + entry.FuncName();
            if (m_FunctionOccurences.find(fullFunction) == m_FunctionOccurences.end())
                m_FunctionOccurences.insert(std::make_pair(fullFunction, 1));
            else
                m_FunctionOccurences[fullFunction] += 1;
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
    return m_FunctionOccurences;
}
std::map<std::string, int> CLogFile::GetFunctionEntries()
{
    CFuncTracer trace("CLogFile::GetFunctionEntries", m_trace);
    try
    {
        if (m_FunctionEntries.size() > 0)
            return m_FunctionEntries;

        std::for_each(m_logEntries.begin(), m_logEntries.end(), [=](CLogEntry& entry){
            if (entry.Description().find("Entr") != std::string::npos)
            {
                std::string fullFunction = entry.Class() + "::" + entry.FuncName();
                if (m_FunctionEntries.find(fullFunction) == m_FunctionEntries.end())
                    m_FunctionEntries.insert(std::make_pair(fullFunction, 1));
                else
                    m_FunctionEntries[fullFunction] += 1;
            }
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
    return m_FunctionEntries;
}
std::map<std::string, int> CLogFile::GetFunctionExits()
{
    CFuncTracer trace("CLogFile::GetFunctionExits", m_trace);
    try
    {
        if (m_FunctionExits.size() > 0)
            return m_FunctionExits;

        std::for_each(m_logEntries.begin(), m_logEntries.end(), [=](CLogEntry& entry){
            if (entry.Description().find("Exit") != std::string::npos)
            {
                std::string fullFunction = entry.Class() + "::" + entry.FuncName();
                if (m_FunctionExits.find(fullFunction) == m_FunctionExits.end())
                    m_FunctionExits.insert(std::make_pair(fullFunction, 1));
                else
                    m_FunctionExits[fullFunction] += 1;
            }
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
    return m_FunctionExits;
}
void CLogFile::SetMark(long long id, bool bMark)
{
    CFuncTracer trace("CLogFile::SetMark", m_trace, false);
    try
    {
        trace.Info("id : %ld, bMark : %s", id, (bMark == true)?"TRUE" : "FALSE");
        int count = std::count_if(m_logEntries.begin(), m_logEntries.end(), [=, &id, &bMark](CLogEntry& entry){
            if (  (entry.GetID() == id)
                ||(id < 0))
            {
                entry.SetMark(bMark);
                return true;
            }
            return false;
        });
        trace.Trace("#%ld Marks changed in m_logEntries", count);
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
void CLogFile::SetRequiredText(long long id, const std::string& text, bool bRequired )
{
    CFuncTracer trace("CLogFile::setRequiredText", m_trace);
    try
    {
        int count = std::count_if(m_logEntries.begin(), m_logEntries.end(), [=, &id, &text, &bRequired](CLogEntry& entry){
            if (  (entry.GetID() == id)
                ||(id < 0))
            {
                entry.SetSearchMark(bRequired, text);
                return true;
            }
            return false;
        });
        trace.Trace("#%ld required text (%s)", count, text.c_str());
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

// Private functions
void CLogFile::automaticDetectFormat(std::vector<std::string>& fields)
{
    CFuncTracer trace("CLogFile::automaticDetectFormat", m_trace);
    try
    {
        int iFieldIdx = 0;
        fields = CConversion::split(m_sLine, ' ', fields, true);
        bool bFuncSeparatorFound = false;
        bool bDescriptionSeparatorFound = false;

        for (std::string sfield : fields)
        {
            // Remove spaces in the string
            sfield = CConversion::trim(sfield);

            if (  (sfield.length() == 1)
                &&(sfield ==  "-"))
            {
                bFuncSeparatorFound = true;
            }
            else if (  (sfield.length() == 1)
                     &&(sfield == ":"))
            {
                bDescriptionSeparatorFound = true;
            }
            else if (  (  (sfield.find("TRACE") != std::string::npos)
                        ||(sfield.find("INFO") != std::string::npos)
                        ||(sfield.find("WARNING") != std::string::npos)
                        ||(sfield.find("ERROR") != std::string::npos)
                        ||(sfield.find("FATAL") != std::string::npos))
                     &&(bDescriptionSeparatorFound == false) )
            {
                m_LevelIdx = iFieldIdx;
            }
            else if (  (sfield.length() == 12)
                     &&(sfield[2] == ':')
                     &&(sfield[5] == ':')
                     &&(sfield[8] == '.'))
            {
                m_TimeIdx = iFieldIdx;
            }
            else if (  (sfield.find(":") != std::string::npos)
                     &&(sfield[0] == '[')
                     &&(sfield[(sfield.length() - 1)] == ']'))
            {
                m_ProdIdIdx = iFieldIdx;
                m_ThreadIdIdx = iFieldIdx;
            }
            else if (bFuncSeparatorFound == true)
            {
                if (bDescriptionSeparatorFound == true)
                {
                    if (m_DescIdx == -1)
                    {
                        m_DescIdx = iFieldIdx;
                    }
                }
                else
                {
                    m_FuncIdx = iFieldIdx;
                    m_ClassNameIdx = iFieldIdx;
                }
            }

            iFieldIdx ++;
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
}
void CLogFile::UpdateConfigSettings(void)
{
    CFuncTracer trace("CLogFile::UpdateConfigSettings", m_trace);
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
bool CLogFile::isTraceLevelValid(const char *level)
{
    std::vector<std::string> validTraceLevels ={"TRACE", "INFO", "WARNING", "ERROR", "FATAL_ERROR"};
    return Contains(validTraceLevels, level);
}
void CLogFile::automaticDetectionFormat_MP(QStringList fields)
{
    CFuncTracer trace("CLogFile::automaticDetectionFormat_MP", m_trace);
    try
    {
        int iFieldIdx = 0;
        bool bFuncSeparatorFound = false;
        bool bDescriptionSeparatorFound = false;

        for (QString sfield : fields)
        {
            if (  (sfield.length() == 1)
                &&(sfield ==  "-"))
            {
                bFuncSeparatorFound = true;
            }
            else if (  (sfield.length() == 1)
                     &&(sfield == ":"))
            {
                bDescriptionSeparatorFound = true;
            }
            else if (  (  (sfield.indexOf("TRACE") >= 0)
                        ||(sfield.indexOf("INFO") >= 0)
                        ||(sfield.indexOf("WARNING") >= 0)
                        ||(sfield.indexOf("ERROR") >= 0)
                        ||(sfield.indexOf("FATAL") >= 0))
                     &&(bDescriptionSeparatorFound == false) )
            {
                m_LevelIdx = iFieldIdx;
            }
            else if (  (sfield.length() == 12)
                     &&(sfield[2] == ':')
                     &&(sfield[5] == ':')
                     &&(sfield[8] == '.'))
            {
                m_TimeIdx = iFieldIdx;
            }
            else if (  (sfield.indexOf(":") > 0)
                     &&(sfield[0] == '[')
                     &&(sfield[(sfield.length() - 1)] == ']'))
            {
                m_ProdIdIdx = iFieldIdx;
                m_ThreadIdIdx = iFieldIdx;
            }
            else if (bFuncSeparatorFound == true)
            {
                if (bDescriptionSeparatorFound == true)
                {
                    if (m_DescIdx == -1)
                    {
                        m_DescIdx = iFieldIdx;
                    }
                }
                else
                {
                    m_FuncIdx = iFieldIdx;
                    m_ClassNameIdx = iFieldIdx;
                }
            }

            iFieldIdx ++;
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
}
void CLogFile::parse_MP(void)
{
    CFuncTracer trace("CLogFile::parse_MP", m_trace);
    CScopeTimer timer("CLogFile::parse_MP", 0, [=, &trace](const std::string& msg)
                      {
                          trace.Info("Timings : ");
                          trace.Info("   %s", msg.c_str());
                      });
    std::ifstream file(m_name);
    try
    {
        timer.SetTime("0a");
        QFile file(QString::fromStdString(m_name));
        timer.SetTime("0b");
        file.open(QFile::ReadOnly);
        timer.SetTime("0c");

        unsigned char* memory = file.map(0, file.size());
        timer.SetTime("0d");

        if (memory == nullptr)
        {
            trace.Error("Memory allocation for the file failed");
            return;
        }
        timer.SetTime("0e");
/*
        QDataStream::readRawData(memory, file.size());*/
        timer.SetTime("0f");

        int size32Bit = static_cast<int>(file.size());
        timer.SetTime("0g");

        auto bytes = QByteArray::fromRawData(reinterpret_cast<const char*>(memory), size32Bit);
        timer.SetTime("0h");

        QTextStream instream(bytes);
        timer.SetTime("0i");

        timer.SetTime("00");
        m_logEntries.clear();
        timer.SetTime("01");

        while(!instream.atEnd())
        {

            timer.SetTime("01a");
            QString line = instream.readLine();
            timer.SetTime("01b");
            m_sLine = line.toStdString();
            timer.SetTime("01c");
            QStringList fields = line.split(' ', Qt::SplitBehaviorFlags::SkipEmptyParts);
            timer.SetTime("01d");

            if (fields.size() == 0)
                continue;

            timer.SetTime("02");
            if (!m_bDetectedFormat)
            {
                timer.SetTime("02b");
                automaticDetectionFormat_MP(fields);
                timer.SetTime("02c");
            }
            timer.SetTime("04");

            if (IsTimeAvailable())
            {
                if (static_cast<int>(fields.size()) > m_TimeIdx)
                {
                    m_sTime = fields[m_TimeIdx].toStdString();
                }
                else
                    m_sTime = "";
            }
            timer.SetTime("05");

            if (IsLevelAvailable())
            {
                if (static_cast<int>(fields.size()) > m_LevelIdx)
                {
                    m_sLevel = fields[m_LevelIdx].toStdString();
                }
                else
                    m_sLevel = "";
            }
            timer.SetTime("06");

            if (IsProcIdAvailable())
            {
                if (static_cast<int>(fields.size()) > m_ProdIdIdx)
                {
                    std::string sProc = fields[m_ProdIdIdx].toStdString();
                    m_sProcId = sProc.substr(1, sProc.find(":") - 1);
                }
                else
                    m_sProcId = "";
            }
            timer.SetTime("07");

            if (IsThreadIdAvailable())
            {
                if (static_cast<int>(fields.size()) > m_ThreadIdIdx)
                {
                    std::string sThread = fields[m_ThreadIdIdx].toStdString();
                    m_sThreadId = sThread.substr(sThread.find(":") + 1);
                    m_sThreadId = m_sThreadId.substr(0, m_sThreadId.find_last_of("]"));
                }
                else
                    m_sThreadId = "";
            }
            timer.SetTime("08");

            if (IsClassNameAvailable())
            {
                if (static_cast<int>(fields.size()) > m_ClassNameIdx)
                {
                    std::string sClass = fields[m_ClassNameIdx].toStdString();
                    m_sClassname = sClass.substr(0, sClass.find_first_of(":"));
                    if (m_sClassname.length() > m_maxClassLength)
                        m_maxClassLength = m_sClassname.length();
                }
                else
                    m_sClassname = "";
            }
            timer.SetTime("09");

            if (IsFuncNameAvailable())
            {
                if (static_cast<int>(fields.size()) > m_FuncIdx)
                {
                    std::string sFunc = fields[m_FuncIdx].toStdString();
                    m_sFuncName = sFunc.substr(sFunc.find_last_of(":") + 1);
                    if (m_sFuncName.length() > m_maxFuncLength)
                        m_maxFuncLength = m_sFuncName.length();
                }
                else
                    m_sFuncName = "";
            }
            timer.SetTime("10");

            if (IsDescriptionAvailable())
            {
                if (static_cast<int>(fields.size()) > m_DescIdx)
                {
                    std::string s = " : ";
                    int start_pos = 0;
                    if (fields[m_DescIdx].toStdString() == "-")
                    {
                        s = fields[m_DescIdx + 1].toStdString();
                        m_sDescription = " - ";
                        start_pos = m_sLine.find(s);
                    }
                    else
                    {
                        s += fields[m_DescIdx].toStdString();
                        m_sDescription = "";
                        start_pos = m_sLine.find(s) + 3;
                    }

                    m_sDescription += m_sLine.substr(start_pos);
                    if (m_sDescription.length() > m_maxDescLength)
                        m_maxDescLength = m_sDescription.length();
                }
                else
                    m_sDescription = "";
            }
            timer.SetTime("11");

            if (isTraceLevelValid(m_sLevel.c_str()))
            {
                timer.SetTime("12");
                m_bDetectedFormat = true;
                CLogEntry entry(m_trace, m_sTime.c_str(), m_sLevel.c_str(), m_sProcId.c_str(),
                                m_sThreadId.c_str(), m_sClassname.c_str(), m_sFuncName.c_str(), m_sDescription.c_str());
                timer.SetTime("12b");
                m_logEntries.emplace_back(entry);
                timer.SetTime("13");
            }
            else
            {
                if (! m_bDetectedFormat)
                    continue;

                if (m_logEntries.size() > 0)
                {
                    timer.SetTime("14");
                    CLogEntry entry(m_trace,
                              m_logEntries[m_logEntries.size() - 1].Time().c_str(),
                              m_logEntries[m_logEntries.size() - 1].Level().c_str(),
                              m_logEntries[m_logEntries.size() - 1].ProcID().c_str(),
                              m_logEntries[m_logEntries.size() - 1].ThreadID().c_str(),
                              m_logEntries[m_logEntries.size() - 1].Class().c_str(),
                              m_logEntries[m_logEntries.size() - 1].FuncName().c_str(),
                                    m_sLine.c_str());
                    timer.SetTime("14b");
                    m_logEntries.emplace_back(entry);
                    timer.SetTime("15");
                }
            }
            timer.SetTime("16");
        }
        file.unmap(memory);
        file.close();

        timer.SetTime("17");
        // Create the filtered entries and copy it from the m_logEntries
        m_filteredEntries.reserve(m_logEntries.size());
        timer.SetTime("18");
        std::copy(m_logEntries.begin(), m_logEntries.end(), std::back_inserter(m_filteredEntries));
        timer.SetTime("19");


        trace.Info("Relative time 0a-0b : %s" , timer.GetRelativeTimes("0a","0b").c_str());
        trace.Info("Relative time 0b-0c : %s" , timer.GetRelativeTimes("0b","0c").c_str());
        trace.Info("Relative time 0c-0d : %s" , timer.GetRelativeTimes("0c","0d").c_str());
        trace.Info("Relative time 0d-0e : %s" , timer.GetRelativeTimes("0d","0e").c_str());
        trace.Info("Relative time 0e-0f : %s" , timer.GetRelativeTimes("0e","0f").c_str());
        trace.Info("Relative time 0f-0g : %s" , timer.GetRelativeTimes("0f","0g").c_str());
        trace.Info("Relative time 0g-0h : %s" , timer.GetRelativeTimes("0g","0h").c_str());
        trace.Info("Relative time 0h-0i : %s" , timer.GetRelativeTimes("0h","0i").c_str());
        trace.Info("Relative time 00-01 : %s" , timer.GetRelativeTimes("00","01").c_str());

        trace.Info("Relative time 01a-01b : %s" , timer.GetRelativeTimes("01a","01b").c_str());
        trace.Info("Relative time 01b-01c : %s" , timer.GetRelativeTimes("01b","01c").c_str());
        trace.Info("Relative time 01c-01d : %s" , timer.GetRelativeTimes("01c","01d").c_str());

        trace.Info("Relative time 02b-02c : %s" , timer.GetRelativeTimes("02b","02c").c_str());
        trace.Info("Relative time 02-04 : %s" , timer.GetRelativeTimes("02","04").c_str());
        trace.Info("Relative time 04-05 : %s" , timer.GetRelativeTimes("04","05").c_str());
        trace.Info("Relative time 05-06 : %s" , timer.GetRelativeTimes("05","06").c_str());
        trace.Info("Relative time 06-07 : %s" , timer.GetRelativeTimes("06","07").c_str());
        trace.Info("Relative time 07-08 : %s" , timer.GetRelativeTimes("07","08").c_str());
        trace.Info("Relative time 08-09 : %s" , timer.GetRelativeTimes("08","09").c_str());
        trace.Info("Relative time 09-10 : %s" , timer.GetRelativeTimes("09","10").c_str());
        trace.Info("Relative time 10-11 : %s" , timer.GetRelativeTimes("10","11").c_str());
        trace.Info("Relative time 11-16 : %s" , timer.GetRelativeTimes("11","16").c_str());
        trace.Info("Relative time 16-02 : %s" , timer.GetRelativeTimes("16","02").c_str());
        trace.Info("Relative time 12-13 : %s" , timer.GetRelativeTimes("12","13").c_str());
        trace.Info("Relative time 14-15 : %s" , timer.GetRelativeTimes("14","15").c_str());
        trace.Info("Relative time 17-18 : %s" , timer.GetRelativeTimes("17","18").c_str());
        trace.Info("Relative time 18-19 : %s" , timer.GetRelativeTimes("18","19").c_str());
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

void CLogFile::parse(void)
{
    CFuncTracer trace("CLogFile::parse", m_trace);
    CScopeTimer timer("CLogFile::parse", 0, [=, &trace](const std::string& msg)
        {
            trace.Info("Timings : ");
            trace.Info("   %s", msg.c_str());
        });
    std::ifstream file(m_name);
    try
    {
        timer.SetTime("00");
        std::vector<std::string> fields;
        m_logEntries.clear();
        timer.SetTime("01");

        while(std::getline(file, m_sLine))
        {
            timer.SetTime("01b");
            if (m_sLine.length() == 0)
                continue;
            timer.SetTime("02");

            if (!m_bDetectedFormat)
            {
                timer.SetTime("02b");
                automaticDetectFormat(fields);
                timer.SetTime("02c");
            }
            else
            {
                timer.SetTime("02d");
                // Split string and add the correct strings
                fields.clear();
                fields = CConversion::split(m_sLine, ' ', fields, true);
                for (std::string& sfield : fields)
                    sfield = CConversion::trim(sfield);
                timer.SetTime("02e");
            }
            timer.SetTime("04");

            if (IsTimeAvailable())
            {
                if (static_cast<int>(fields.size()) > m_TimeIdx)
                {
                    m_sTime = fields[m_TimeIdx];
                }
                else
                    m_sTime = "";
            }
            timer.SetTime("05");

            if (IsLevelAvailable())
            {
                if (static_cast<int>(fields.size()) > m_LevelIdx)
                {
                    m_sLevel = fields[m_LevelIdx];
                }
                else
                    m_sLevel = "";
            }
            timer.SetTime("06");

            if (IsProcIdAvailable())
            {
                if (static_cast<int>(fields.size()) > m_ProdIdIdx)
                {
                    m_sProcId = fields[m_ProdIdIdx].substr(1, fields[m_ProdIdIdx].find(":") - 1);
                }
                else
                    m_sProcId = "";
            }
            timer.SetTime("07");

            if (IsThreadIdAvailable())
            {
                if (static_cast<int>(fields.size()) > m_ThreadIdIdx)
                {
                    m_sThreadId = fields[m_ThreadIdIdx].substr(fields[m_ProdIdIdx].find(":") + 1);
                    m_sThreadId = m_sThreadId.substr(0, m_sThreadId.find_last_of("]"));
                }
                else
                    m_sThreadId = "";
            }
            timer.SetTime("08");

            if (IsClassNameAvailable())
            {
                if (static_cast<int>(fields.size()) > m_ClassNameIdx)
                {
                    m_sClassname = fields[m_ClassNameIdx].substr(0, fields[m_ClassNameIdx].find_first_of(":"));
                    if (m_sClassname.length() > m_maxClassLength)
                        m_maxClassLength = m_sClassname.length();
                }
                else
                    m_sClassname = "";
            }
            timer.SetTime("09");

            if (IsFuncNameAvailable())
            {
                if (static_cast<int>(fields.size()) > m_FuncIdx)
                {
                    m_sFuncName = fields[m_FuncIdx].substr(fields[m_FuncIdx].find_last_of(":") + 1);
                    if (m_sFuncName.length() > m_maxFuncLength)
                        m_maxFuncLength = m_sFuncName.length();
                }
                else
                    m_sFuncName = "";
            }
            timer.SetTime("10");

            if (IsDescriptionAvailable())
            {
                if (static_cast<int>(fields.size()) > m_DescIdx)
                {
                    std::string s = " : ";
                    int start_pos = 0;
                    if (fields[m_DescIdx] == "-")
                    {
                        s = fields[m_DescIdx + 1];
                        m_sDescription = " - ";
                        start_pos = m_sLine.find(s);
                    }
                    else
                    {
                        s += fields[m_DescIdx];
                        m_sDescription = "";
                        start_pos = m_sLine.find(s) + 3;
                    }

                    m_sDescription += m_sLine.substr(start_pos);
                    if (m_sDescription.length() > m_maxDescLength)
                        m_maxDescLength = m_sDescription.length();
                }
                else
                    m_sDescription = "";
            }
            timer.SetTime("11");

            if (isTraceLevelValid(m_sLevel.c_str()))
            {
                timer.SetTime("12");
                m_bDetectedFormat = true;
                m_logEntries.push_back(CLogEntry(m_trace, m_sTime.c_str(), m_sLevel.c_str(), m_sProcId.c_str(),
                                                 m_sThreadId.c_str(), m_sClassname.c_str(), m_sFuncName.c_str(), m_sDescription.c_str()));
                timer.SetTime("13");
            }
            else
            {
                if (! m_bDetectedFormat)
                {
                    fields.clear();
                    continue;
                }

                if (m_logEntries.size() > 0)
                {
                    timer.SetTime("14");
                    m_logEntries.push_back(CLogEntry(m_trace,
                                                     m_logEntries[m_logEntries.size() - 1].Time().c_str(),
                                                     m_logEntries[m_logEntries.size() - 1].Level().c_str(),
                                                     m_logEntries[m_logEntries.size() - 1].ProcID().c_str(),
                                                     m_logEntries[m_logEntries.size() - 1].ThreadID().c_str(),
                                                     m_logEntries[m_logEntries.size() - 1].Class().c_str(),
                                                     m_logEntries[m_logEntries.size() - 1].FuncName().c_str(),
                                                     m_sLine.c_str()));
                    timer.SetTime("15");
                }
            }
            timer.SetTime("16");
        }
        timer.SetTime("17");
        // Create the filtered entries and copy it from the m_logEntries
        m_filteredEntries.reserve(m_logEntries.size());
        timer.SetTime("18");
        std::copy(m_logEntries.begin(), m_logEntries.end(), std::back_inserter(m_filteredEntries));
        timer.SetTime("19");


        trace.Info("Relative time 01-02 : %s" , timer.GetRelativeTimes("01","02").c_str());
        trace.Info("Relative time 02b-02c : %s" , timer.GetRelativeTimes("02b","02c").c_str());
        trace.Info("Relative time 02d-02e : %s" , timer.GetRelativeTimes("02d","02e").c_str());
        trace.Info("Relative time 02-04 : %s" , timer.GetRelativeTimes("02","04").c_str());
        trace.Info("Relative time 04-05 : %s" , timer.GetRelativeTimes("04","05").c_str());
        trace.Info("Relative time 05-06 : %s" , timer.GetRelativeTimes("05","06").c_str());
        trace.Info("Relative time 06-07 : %s" , timer.GetRelativeTimes("06","07").c_str());
        trace.Info("Relative time 07-08 : %s" , timer.GetRelativeTimes("07","08").c_str());
        trace.Info("Relative time 08-09 : %s" , timer.GetRelativeTimes("08","09").c_str());
        trace.Info("Relative time 09-10 : %s" , timer.GetRelativeTimes("09","10").c_str());
        trace.Info("Relative time 10-11 : %s" , timer.GetRelativeTimes("10","11").c_str());
        trace.Info("Relative time 11-16 : %s" , timer.GetRelativeTimes("11","16").c_str());
        trace.Info("Relative time 16-02 : %s" , timer.GetRelativeTimes("16","02").c_str());
        trace.Info("Relative time 12-13 : %s" , timer.GetRelativeTimes("12","13").c_str());
        trace.Info("Relative time 14-15 : %s" , timer.GetRelativeTimes("14","15").c_str());
        trace.Info("Relative time 17-18 : %s" , timer.GetRelativeTimes("17","18").c_str());
        trace.Info("Relative time 18-19 : %s" , timer.GetRelativeTimes("18","19").c_str());

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
int CLogFile::getFileSize(FILE* fp)
{
    CFuncTracer trace("CLogFile::getFileSize", m_trace);
    CScopeTimer timer("CLogFile::getFileSize", 0, [=, &trace](const std::string& msg){
        trace.Info("Timings:");
        trace.Info("    %s", msg.c_str());
    });
    int iSize = -1;
    try
    {
        int prev = ftell(fp);
        fseek(fp, 0L, SEEK_END);
        iSize = ftell(fp);
        fseek(fp, prev, SEEK_SET);
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
    return iSize;
}

int CLogFile::getNrOfLines(const std::string& file)
{
    CFuncTracer trace("CLogFile::getNrOfLines", m_trace);
    CScopeTimer timer("CLogFile::getNrOfLines", 0, [=, &trace](const std::string& msg){
        trace.Info("Timings:");
        trace.Info("    %s", msg.c_str());
    });

    int ilines = -1;
    try
    {
        FILE* fp = fopen(file.c_str(), "r");
        if (fp != NULL)
        {
            int iFileSize = getFileSize(fp);
            if (iFileSize > 0)
            {
                std::unique_ptr<char[]> buffer = std::make_unique<char[]>(iFileSize);
                int bytesRead = fread(buffer.get(), 1, iFileSize, fp);
                char *ptr = buffer.get();
                for (int i = 0; i < bytesRead; i++)
                {
                    if (*ptr++ == '\n')
                        ilines++;
                }
            }

            fclose(fp);
            fp = NULL;
            trace.Trace("Number of lines: %ld", ilines);
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
    return ilines;
}
