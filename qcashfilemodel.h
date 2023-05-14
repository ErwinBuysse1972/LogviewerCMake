#pragma once
#include <QObject>
#include <QContiguousCache>
#include <QAbstractListModel>
#include <IDataObject.h>
#include "ctracer.h"


QT_FORWARD_DECLARE_CLASS(QTimer)

class QCashFileModel: public QAbstractListModel
{
    Q_OBJECT
public:
    QCashFileModel(std::shared_ptr<CTracer> tracer,
                   IDataContainer& container,
                   QObject* parent = nullptr);
    ~QCashFileModel();

    int rowCount(const QModelIndex& = QModelIndex()) const override;
    QVariant data(const QModelIndex&, int) const override;
    void rowToggleMark(const QModelIndex& index, bool& bMark);
private:
    void cacheRows(int from, int to) const;
    IData fetchRow(int position) const;

    mutable IDataContainer m_container;
    mutable QContiguousCache<IData> m_rows;
    const int m_count;
    std::shared_ptr<CTracer> m_trace;
};
