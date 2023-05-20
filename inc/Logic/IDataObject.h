#pragma once
#include <QObject>
#include <QString>
#include <QColor>
#include <map>
#include "ctracer.h"

const QColor white(255,255,255);
class IData /*: public QObject*/
{
    /*Q_OBJECT*/
public:
    IData()/*:QObject()*/{}
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

    virtual int ColumnCount(void) = 0;
    virtual std::map<int, std::string> GetColumns(void) = 0;
    virtual int RowCount(void) = 0; //{ return 0; }
    virtual IData* GetData(int Row) = 0; //{ return nullptr;};
    virtual void append(std::vector<IData*> dt) = 0; //{};
    virtual void clear(void) = 0; //{};
    virtual long long ToggleMark(int Row) = 0; // {};
    virtual long long GetNextToggleMark(int currentRow) = 0;
    virtual long long GetNextRequiredText(int currentRow) = 0;
protected:
    IData nullref;
private:
    std::shared_ptr<CTracer> m_trace;
};
