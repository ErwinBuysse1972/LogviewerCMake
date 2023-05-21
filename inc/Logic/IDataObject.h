#pragma once
#include <QObject>
#include <QString>
#include <QColor>
#include <map>
#include "ctracer.h"

// Contains abstract classes that are used by the different model views.
//    for the moment we have implemented two model views:
//       - QFileModel : get the complete data of the file and give it to the viewer
//       - QCashFileModel : this model takes a cash of the data that is necessary to update the view.
//                          (the complete data is stored inside a IDataContainer where the base class
//                          is described here below.

const QColor white(255,255,255);
// This is the base class that contains the data of 1 specific entry (row) in the table.
class IData /*: public QObject*/
{
    /*Q_OBJECT*/
public:
    IData()/*:QObject()*/{}
    virtual ~IData(){}
    IData(const IData& itm){};
    IData(IData&& itm){};
    IData& operator=(const IData& itm){ return *this;}
    virtual QString GetDataElement(int column) const { return QString("");}         // Get the data of the specific column
    virtual QColor GetBackGroundColor(int column) const { return white; }           // Get the background color for specific row$
    virtual bool IsTextRequired(void) const{ return false; }

};


// This is the base class that is used by the cashcontainer/
//     this class contains all the data that need to be viewed by the viewer.
//     The data can be marked or required in the view with two functions you are able
//     to jump between corresponding selections (marks/required text)
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

    virtual int ColumnCount(void) = 0;                                              // Get the amount of columns in your view
    virtual std::map<int, std::string> GetColumns(void) = 0;                        // Get the names of the columns
    virtual int RowCount(void) = 0; //{ return 0; }                                 // Get the rows inside your data container
    virtual IData* GetData(int Row) = 0; //{ return nullptr;};                      // Get the data at specific position
    virtual void append(std::vector<IData*> dt) = 0; //{};                          // fill in the container with now data
    virtual void clear(void) = 0; //{};                                             // Clear the data of the container
    virtual long long ToggleMark(int Row) = 0; // {};                               // Toggle Mark at specific position
    virtual long long GetNextToggleMark(int currentRow) = 0;                        // Get next mark position
    virtual std::list<long long> IndicateSearchText(const std::string& text, std::list<int>& rowsChanged) = 0;   // Indicate the required text with specific text
    virtual long long GetNextRequiredText(int currentRow) = 0;                      // Get the next required text position
    virtual bool IsTextRequired(int currentRow) = 0;
protected:
    IData nullref;
private:
    std::shared_ptr<CTracer> m_trace;
};


// TODO:
//    add a filter feature to the model so that the logviewer is not responsible anymore
//      of filtering the data which is now the case.
