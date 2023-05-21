#pragma once
#include <clogfile.h>
#include <QObject>
#include <QAction>
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


class RichTextDelegate : public QItemDelegate
{
        Q_OBJECT
public:
    RichTextDelegate(std::shared_ptr<CTracer> tracer, QObject *parent);
    void paint( QPainter *painter,
                const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint( const QStyleOptionViewItem &option,
                    const QModelIndex &index) const override;
    QWidget *createEditor(  QWidget *parent, const QStyleOptionViewItem &option,
                            const QModelIndex &index) const override ;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(  QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(  QWidget *editor,
                                const QStyleOptionViewItem &option,
                                const QModelIndex &index) const override;
protected:
    bool eventFilter(QObject *object, QEvent *event) override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model,
                     const QStyleOptionViewItem &option, const QModelIndex &index) override;
private:
    std::shared_ptr<CTracer> m_trace;
};
