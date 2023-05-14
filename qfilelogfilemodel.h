#pragma once
#include <clogfile.h>
#include <QObject>
#include <QAbstractTableModel>
#include <QGridLayout>
#include <QTableView>
#include <QPushButton>
#include <QStyledItemDelegate>
#include <QItemDelegate>
#include <QSortFilterProxyModel>
#include <QInputDialog>
#include <QHeaderView>
#include <QColor>
#include <QMessageBox>
#include <QTextEdit>
#include <QPainter>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>

#include <functional>
#include <list>
#include "ctracer.h"

class QLogFileModel : public QAbstractTableModel
{
    Q_OBJECT

private:
    QList<CLogEntry> m_entries;
    std::shared_ptr<CTracer> m_trace;
    int pageLength; // Page nr of items that will be viewed in the current window
    int currentPage;
public:
    QLogFileModel(std::shared_ptr<CTracer> tracer, QObject* parent = {});
    int rowCount(const QModelIndex &) const override;
    int columnCount(const QModelIndex &) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    void append(std::vector<CLogEntry> entries);
    void clear();
    long long rowToggleMark(const QModelIndex& index, bool& bMark);
    std::optional<CLogEntry> getLogEntry(int index) const;
    int rowGetNextToggleMark(QModelIndex& currentIdx);
    std::list<long long> IndicateSearchText(const std::string& text);
    int rowGetNextSearchFoundItem(const QModelIndex& currentIdx);
    QModelIndex CreateIndex(int row, int column);
};
