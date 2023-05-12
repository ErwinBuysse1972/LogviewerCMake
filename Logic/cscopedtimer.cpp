#include <cscopedtimer.h>
#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <chrono>

using   Clocktype = std::chrono::high_resolution_clock;

std::map<std::string, Clocktype::time_point> CScopeTimer::m_recallTime;
std::map<std::string, float> CScopeTimer::m_totaltime;
std::map<std::string, long long> CScopeTimer::m_calls;
std::map<std::string, long long> CScopeTimer::m_funcAboveThreshold;

CScopeTimer::CScopeTimer(const std::string& funcName, long long thrsFuncTimens, std::function<void(const std::string& msg)> cb)
    : m_sFuncName(funcName)
    , m_start(Clocktype::now())
    , m_thrsFuncTime(thrsFuncTimens)
    , m_cbNotify(cb)
{
    if (m_calls.find(m_sFuncName) == m_calls.end())
    {
        m_calls.insert(std::make_pair(m_sFuncName, 1));
    }
    else
    {
       ++m_calls[m_sFuncName];
    }
    if (m_funcAboveThreshold.find(m_sFuncName) == m_funcAboveThreshold.end())
      m_funcAboveThreshold.insert(std::make_pair(m_sFuncName, 0));
}
CScopeTimer::~CScopeTimer()
{
    auto funcTime = Clocktype::now() - m_start;
    std::stringstream ss;

    if (std::chrono::duration<float, std::nano>(funcTime).count() > m_thrsFuncTime)
    {
        ++m_funcAboveThreshold[m_sFuncName];
    }

    ss << std::endl;
    ss << "#calls : " << m_calls[m_sFuncName] << std::endl;
    ss << "#calls above thrshd : " << m_funcAboveThreshold[m_sFuncName] << std::endl;
    ss << "func time : " << std::chrono::duration<float, std::milli>(funcTime).count() << " ms" << std::endl;
    ss << "time between two calls : ";
    if (m_recallTime.find(m_sFuncName) != m_recallTime.end())
    {
          ss << std::chrono::duration<float, std::nano>(m_start - m_recallTime[m_sFuncName]).count() << " ns" << std::endl;
    }
    else
    {
          ss << "0 ns" << std::endl;
    }
    if (m_totaltime.find(m_sFuncName) == m_totaltime.end())
    {
        m_totaltime.insert(std::make_pair(m_sFuncName, std::chrono::duration<float, std::nano>(funcTime).count()));
    }
    else
    {
        m_totaltime[m_sFuncName] += std::chrono::duration<float, std::nano>(funcTime).count();
    }

    ss << "total processing time : " << m_totaltime[m_sFuncName] << " ns." <<  std::endl;

    for (auto& ts : m_interestingTimes)
    {
        if (ts.second.size() > 0)
        {
            auto duration = ts.second[0] - m_start;
            float fdur = std::chrono::duration<float, std::micro>(duration).count();
            ss << "    " << ts.first << " : " << fdur << " us" << std::endl;
        }
    }


    if (m_cbNotify)
        m_cbNotify(ss.str());

    if (m_recallTime.find(m_sFuncName)!= m_recallTime.end())
        m_recallTime[m_sFuncName] = Clocktype::now();
    else
        m_recallTime.insert(std::make_pair(m_sFuncName, Clocktype::now()));
}
void CScopeTimer::SetTime(const std::string& tsName)
{
    if (m_interestingTimes.find(tsName)== m_interestingTimes.end())
    {
        m_interestingTimes.insert(std::make_pair(tsName, std::vector<Clocktype::time_point>()));
    }
    m_interestingTimes[tsName].push_back(Clocktype::now());
}

std::string CScopeTimer::GetRelativeTimes(const std::string& First, const std::string& Later, bool bAverageOnly)
{
    std::stringstream ss;

    if (m_interestingTimes.find(First) != m_interestingTimes.end())
    {
        if (m_interestingTimes[First].size() == m_interestingTimes[Later].size())
        {
            float fdur_total = 0.0;
            for(int idx = 0; idx < (int)m_interestingTimes[First].size(); idx++)
            {
                if (m_interestingTimes[Later][idx] > m_interestingTimes[First][idx])
                {
                    auto duration = m_interestingTimes[Later][idx] - m_interestingTimes[First][idx];
                    float fdur = std::chrono::duration<float, std::nano>(duration).count();
                    if (bAverageOnly == false)
                        ss << "    " << fdur << " ns." << std::endl;
                    fdur_total += fdur;
                }
            }
            ss << "average time : " << (fdur_total/m_interestingTimes[First].size()) << " ns, (#meas : " << m_interestingTimes[First].size() << "," << m_interestingTimes[Later].size() << ")" << std::endl;
        }
        else
            ss << "first (" << m_interestingTimes[First].size() << "), later (" << m_interestingTimes[Later].size() << ")" << std::endl;
    }
    return ss.str();
}
