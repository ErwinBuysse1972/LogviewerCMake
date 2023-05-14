#pragma once
#include <QObject>
#include <QString>
#include <QColor>
#include "ctracer.h"

const QColor white(255,255,255);
class IData : public QObject
{
    Q_OBJECT
public:
    IData():QObject(){}
    virtual ~IData(){}
    IData(const IData& itm){};
    IData(IData&& itm){};
    IData& operator=(const IData& itm){ return *this;}
    virtual QString GetDataElement(int column) const { return QString("");}
    virtual QColor GetBackGroundColor(int column) const { return white; }

};

class IDataContainer : public QObject
{
    Q_OBJECT
public:
    IDataContainer(std::shared_ptr<CTracer> tracer, QObject* parent = nullptr) noexcept
        : QObject(parent)
        , m_trace(tracer)
    {}
    virtual ~IDataContainer(){}
    IDataContainer(const IDataContainer& itm)
    {
        m_trace = itm.m_trace;
        nullref = itm.nullref;
    }
    IDataContainer(const IDataContainer&& itm)
    {
        m_trace = std::move(itm.m_trace);
        nullref = std::move(itm.nullref);
    }
    IDataContainer& operator=(const IDataContainer&& itm)
    {
        if (this != &itm)
        {
            m_trace = std::move(itm.m_trace);
            nullref = std::move(itm.nullref);
        }
        return *this;
    }

    virtual IData& GetData(int Row) { return nullref;};
    virtual void ToggleMark(int Row) {};
protected:
    IData nullref;
private:
    std::shared_ptr<CTracer> m_trace;
};
