#pragma once
#include <QObject>
#include <QContiguousCache>
#include <QAbstractListModel>
#include <IDataObject.h>
#include "ctracer.h"


QT_FORWARD_DECLARE_CLASS(QTimer)

class QCashFileModel: public QAbstractTableModel
{
    Q_OBJECT
public:
    QCashFileModel(std::shared_ptr<CTracer> tracer,
                   IDataContainer* container,
                   QObject* parent = nullptr);
    ~QCashFileModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent=QModelIndex()) const override;
    QVariant data(const QModelIndex&, int) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    long long rowToggleMark(const QModelIndex& index, bool& bMark);
    long long rowGetNextToggleMark(QModelIndex& currentIdx);
    std::list<long long> IndicateSearchText(const std::string& text);
    long long rowGetNextSearchFoundItem(const QModelIndex& currentIdx);
    long long CashCurrentPosition(int position);
    void append(std::vector<IData *> dt);
    void clear();
    IData* getLogEntry(int index) const;
    QModelIndex CreateIndex(int row, int column);
private:
    void cacheRows(int from, int to) const;
    IData* fetchRow(int position) const;

    mutable IDataContainer *m_container;
    mutable QContiguousCache<IData *> m_rows;
    std::shared_ptr<CTracer> m_trace;
};
