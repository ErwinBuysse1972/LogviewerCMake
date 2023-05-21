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
#include <QAbstractItemModel>

#include <functional>
#include <list>

#include "ctracer.h"
#include "richtextdelegate.h"
#include "qfilelogfilemodel.h"
#include "qcashfilemodel.h"
#include "qfiletableview.h"


class QLogFileWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QLogFileWidget(std::shared_ptr<CTracer> tracer, std::vector<IData *> entries, __attribute__((unused))QWidget *parent = nullptr);
    ~QLogFileWidget();
    QLogFileModel *GetModel(void);
    QCashFileModel *GetCashModel(void);
    void SetTabIndex(int index);
    int GetTabIndex(void);
    void SetFileName(std::string file);
    std::string GetFilename(void);
    QModelIndex GetCurrentIndex(void);
    long long ToggleMark(QModelIndex index, bool& bMark);
    void GotoToNextMark(void);
    void GotoNextRequiredText(void);
    void GotoTopOfTable(void);
    void RegisterDropDown_StartTime(std::function<void (const std::string&)> event){ m_notStartTime = event; }
    void RegisterDropDown_StopTime(std::function<void (const std::string&)> event){ m_notStopTime = event; }
    void RegisterDropDown_ProcID(std::function<void (const std::string&)> event){ m_notProcId = event; }
    void RegisterDropDown_ThreadID(std::function<void (const std::string&)> event){ m_notThreadId = event; }
    void RegisterDropDown_MarkToggle(std::function<void (QModelIndex)> event){ m_notMarkToggle = event; }
    void RegisterDropDown_DisableClass(std::function<void (const std::string&)> event){ m_notDisableClass = event; }
    void RegisterDropDown_DisableFunc(std::function<void (const std::string&)> event){ m_notDisableFunc = event; }
    void RegisterDropDown_DisableTraceLevel(std::function<void (const std::string&)> event){ m_notDisableTraceLevel = event; }

signals:
    void RowDoubleClicked(const CLogEntry* index);

private slots:
    void row_double_click(const QModelIndex &  index);
    void row_click(__attribute__((unused)) const QModelIndex &  index);
    void notifyStartDateTime(void);
    void notifyStopDateTime(void);
    void notifyProcessId(void);
    void notifyThreadID(void);
    void toggleMark(void);
    void disableClass(void);
    void disablefunction(void);
    void disableTraceLevel(void);

    private:
    int m_tabindex;
    std::string m_FileName;

    QModelIndex m_notIndex;
    QHBoxLayout *m_Layout;
    RichTextDelegate *m_delegate;
    QLogfileTableView *m_View;
    CLogEntryContainer *m_container;
    QCashFileModel  *m_cashModel;
    QLogFileModel *m_Model;
    std::shared_ptr<CTracer> m_trace;
    QAction *actNotifyStartDateTime;
    QAction *actNotifyStopDateTime;
    QAction *actNotifyProcessID;
    QAction *actNotifyThreadID;
    QAction *actNotifyMarkToggle;
    QAction *actNotifyDisableClass;
    QAction *actNotifyDisableFunction;
    QAction *actNotifyDisableTraceLevel;

    std::function<void (const std::string&)>    m_notStartTime;
    std::function<void (const std::string&)>    m_notStopTime;
    std::function<void (const std::string&)>    m_notProcId;
    std::function<void (const std::string&)>    m_notThreadId;
    std::function<void (QModelIndex)>           m_notMarkToggle;
    std::function<void (const std::string&)>    m_notDisableClass;
    std::function<void (const std::string&)>    m_notDisableFunc;
    std::function<void (const std::string&)>    m_notDisableTraceLevel;

    void init_createActions(void);
    void init_createForm(std::vector<IData*> entries);
};
