#pragma once
#include <QObject>
#include <QStandardItemModel>

#include <vector>
#include <string>
#include "cfunctracer.h"
#include "ctracer.h"
#include "cscopedtimer.h"

class QCheckableModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit QCheckableModel(std::shared_ptr<CTracer> tracer, std::string sname, QObject *parent = nullptr)
        : QStandardItemModel(parent)
        , m_name(sname)
        , m_trace(tracer)
    {
        CFuncTracer trace("QCheckableModel::QCheckableModel", m_trace);
    }


    ~QCheckableModel()
    {
/*        if (m_stdItems.size() > 0)
        {
            std::for_each(m_stdItems.begin(),
                          m_stdItems.end(),
                          [=](std::pair<int, QStandardItem *> p) {
                              if (p.second != nullptr) {
                                  delete p.second;
                                  p.second = nullptr;
                              }
                          });
            m_stdItems.clear();
        }*/
    }
    int rowCount(const QModelIndex &) const override
    {
        CFuncTracer trace("QCheckableModel::rowCount", m_trace, false);
        trace.Trace("#rowcount : %ld", m_stdItems.size());
        return m_stdItems.size();
    }
    int columnCount(const QModelIndex &) const override
    {
        CFuncTracer trace("QCheckableModel::columnCount", m_trace, false);
        trace.Trace("count : %ld", 1);
        return 1;
    }
    int GetIndex(std::string sText)
    {
        CFuncTracer trace("QCheckableModel::GetIndex", m_trace);
        int index = -1;
        try
        {
            if (m_stdItems.size() > 0)
            {
                for_each(m_stdItems.begin(),
                         m_stdItems.end(),
                         [=, &index, &sText](std::pair<int, QStandardItem *> p) {
                             if (p.second->text().toStdString() == sText)
                                 index = p.first;
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
        return index;
    }
    bool IsItemChecked(std::string sText)
    {
        CFuncTracer trace("QCheckableModel::IsItemChecked", m_trace);
        try
        {
            trace.Trace("text : %s", sText.c_str());
            int index = GetIndex(sText);
            if (m_stdItems.find(index)!= m_stdItems.end())
            {
                if (m_stdItems[index]->data(Qt::CheckStateRole) == Qt::Checked)
                    return true;
            }
        }
        catch(std::exception& ex)
        {
            trace.Error("Exception occurred :%s", ex.what());
        }
        catch(...)
        {
            trace.Error("Exception occurred");
        }
        return false;
    }
    void append(std::map<std::string, bool> elements)
    {
        CFuncTracer trace("QCheckableModel::append", m_trace);
        CScopeTimer timer("QCheckableModel::append", 0, [=, &trace](const std::string& msg){
            trace.Trace("[%s] #m_stdItems : %ld", m_name.c_str(), m_stdItems.size());
            trace.Trace("[%s] #m_mapIndex : %ld", m_name.c_str(), m_mapindex.size());
            trace.Info("[%s]Timings:", m_name.c_str());
            trace.Info("[%s]    %s",m_name.c_str(), msg.c_str());
        });
        try
        {
            timer.SetTime("00");
            int index = 0;
            QStandardItemModel::beginInsertRows({}, m_stdItems.size(), m_stdItems.size() + elements.size());
            for (std::pair<std::string, bool> p : elements)
            {
                timer.SetTime("01");
                QStandardItem *item = new QStandardItem(QString::fromStdString(p.first));
                timer.SetTime("02");

                item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
                if (p.second == true)
                    item->setData(Qt::Checked, Qt::CheckStateRole);
                else
                    item->setData(Qt::Unchecked, Qt::CheckStateRole);
                timer.SetTime("03");
                m_mapindex.insert(std::make_pair(p.first, index));
                timer.SetTime("04");
                m_stdItems.insert(std::make_pair(index, item));
                timer.SetTime("05");

                trace.Trace("index:%ld, p.first:%s, checked state : %s"
                            , index, p.first.c_str(), (p.second == true)?"CHECKED" : "NOT CHECKED");

                timer.SetTime("06");
                insertRow(index, item);
                timer.SetTime("07");
                index++;
            }
            timer.SetTime("All elements created and append");
            this->endInsertRows();
            timer.SetTime("endInsertRows");

            trace.Info("Relative time 01-02 : %s" , timer.GetRelativeTimes("01","02").c_str());
            trace.Info("Relative time 02-03 : %s" , timer.GetRelativeTimes("02","03").c_str());
            trace.Info("Relative time 03-04 : %s" , timer.GetRelativeTimes("03","04").c_str());
            trace.Info("Relative time 04-05 : %s" , timer.GetRelativeTimes("04","05").c_str());
            trace.Info("Relative time 05-06 : %s" , timer.GetRelativeTimes("05","06").c_str());
            trace.Info("Relative time 06-07 : %s" , timer.GetRelativeTimes("06","07").c_str());
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
    void update(std::map<std::string, bool> elements)
    {
        CFuncTracer trace("QCheckableModel::update", m_trace);
        try
        {
            int index = m_stdItems.size();
            QStandardItemModel::beginInsertRows({}, m_stdItems.size(), m_stdItems.size() + elements.size());
            for (std::pair<std::string, bool> p : elements)
            {
                std::string key = p.first;
                if (m_stdItems.find(m_mapindex[key]) != m_stdItems.end())
                {
                    if (p.second == true)
                        m_stdItems[m_mapindex[key]]->setData(Qt::Checked, Qt::CheckStateRole);
                    else
                        m_stdItems[m_mapindex[key]]->setData(Qt::Unchecked, Qt::CheckStateRole);

                    trace.Trace("p.first : %s key(index) : %ld, checked state : %s"
                                , p.first.c_str()
                                , m_mapindex[key]
                                , (p.second == true)?"CHECKED" : "NOT CHECKED");
                }
                else
                {
                    QStandardItem *item = new QStandardItem(QString::fromStdString(p.first));
                    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
                    if (p.second == true)
                        item->setData(Qt::Checked, Qt::CheckStateRole);
                    else
                        item->setData(Qt::Unchecked, Qt::CheckStateRole);
                    insertRow(index, item);
                    m_stdItems.insert(std::make_pair(index, item));
                    m_mapindex.insert(std::make_pair(p.first, index));
                    trace.Trace("index:%ld, p.first:%s, checked state : %s"
                                , index, p.first.c_str(), (p.second == true)?"CHECKED" : "NOT CHECKED");
                    index++;
                }
            }
            this->endInsertRows();
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
private:
    // int is the index
    std::map<int, QStandardItem*> m_stdItems;
    std::map<std::string, int> m_mapindex;
    std::string m_name;
    std::shared_ptr<CTracer> m_trace;
};
