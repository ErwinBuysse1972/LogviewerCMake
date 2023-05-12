#pragma once
#include <string>
#include <vector>
#include <memory>
#include "ctracer.h"

class CConfigSettings
{
public:
    CConfigSettings(std::shared_ptr<CTracer> tracer, const std::string& filename);
    virtual ~CConfigSettings();

    std::string configFilename(){ return m_sConfigFilename;}
    int lines(void){ return m_lines;}
    std::vector<int> markLines(void){ return m_MarkedLines;}
    std::vector<int> filteredProcIDs(void){ return m_filteredProcIDs;}
    std::vector<int> filteredThreadIDs(void){ return m_filteredThreadIDs;}
    std::vector<std::string> filteredFunctions(void){ return m_filteredFunctions;}
    std::vector<std::string> filteredClasses(void){ return m_filteredClasses;}
    std::vector<TracerLevel> filteredTracerlevels(void){ return m_filteredTracerlevels;}


    void lines(unsigned long long ullLines){ m_bConfigurationChanged = true; m_lines = ullLines; }
    void AddMarkLines(std::vector<int> marks){
        m_bConfigurationChanged = true;
        for (int mark : marks)
            m_MarkedLines.push_back(mark);
    }
    void ClearMarks(void){ m_bConfigurationChanged = true; m_MarkedLines.clear();}
    void AddProcIds(std::vector<int> procIds){
        m_bConfigurationChanged = true;
        for (int id : procIds)
            m_filteredProcIDs.push_back(id);
    }
    void ClearProcIDs(void){ m_bConfigurationChanged = true; m_filteredProcIDs.clear();}
    void AddThreadIds(std::vector<int> threadIds){
        m_bConfigurationChanged = true;
        for (int id : threadIds)
            m_filteredThreadIDs.push_back(id);
    }
    void ClearThreadIDs(void){ m_bConfigurationChanged = true; m_filteredThreadIDs.clear();}

    void AddFunctions(std::vector<std::string> functions){
        m_bConfigurationChanged = true;
        for (const std::string& func : functions)
            m_filteredFunctions.push_back(func);
    }
    void ClearFunctions(void){ m_bConfigurationChanged = true; m_filteredFunctions.clear();}

    void AddClasses(std::vector<std::string> classes){
        m_bConfigurationChanged = true;
        for (const std::string& cls : classes)
            m_filteredClasses.push_back(cls);
    }
    void ClearClasses(void){ m_bConfigurationChanged = true; m_filteredClasses.clear();}

    void AddTracerLevel(std::vector<TracerLevel> levels){
        m_bConfigurationChanged = true;
        for (TracerLevel lvl : levels)
            m_filteredTracerlevels.push_back(lvl);
    }
    void ClearTracerLevels(void){ m_bConfigurationChanged = true; m_filteredTracerlevels.clear();}


private:
    std::shared_ptr<CTracer>    m_trace;
    std::string                 m_sLogFilename;
    std::string                 m_sConfigFilename;
    unsigned long long          m_lines;
    std::vector<int>            m_MarkedLines;
    std::vector<int>            m_filteredProcIDs;
    std::vector<int>            m_filteredThreadIDs;
    std::vector<std::string>    m_filteredFunctions;
    std::vector<std::string>    m_filteredClasses;
    std::vector<TracerLevel>    m_filteredTracerlevels;
    bool                        m_bConfigurationChanged;

    void loadConfigurations(void);
    void storeConfigurations(void);

};
