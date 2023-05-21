#pragma once
#include <QObject>
#include "ctracer.h"

class IDataObject : public QObject
{
    Q_OBJECT
public:
    IDataObject(std::shared_ptr<CTracer> tracer, QObject* parent = nullptr)
        : QObject(parent)
        , m_trace(tracer)
    {

    }
    ~IDataObject(){}

    virtual QString GetData(int Row, int column) = 0;
private:
    std::shared_ptr<CTracer> m_trace;
};
