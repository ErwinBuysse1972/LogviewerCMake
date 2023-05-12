#include "logentryview.h"
#include "ui_logentryview.h"

LogEntryView::LogEntryView(const CLogEntry& entry, std::shared_ptr<CTracer> tracer, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LogEntryView)
    , m_trace(tracer)

{
    CFuncTracer trace("LogEntryView::LogEntryView", m_trace);
    try
    {
        ui->setupUi(this);
        ui->txtFunction->setText(QString::fromStdString(entry.Class() + "::" + entry.FuncName()));
        ui->txtFunction->setReadOnly(true);
        ui->txtProcessID->setText(QString::fromStdString(entry.ProcID()));
        ui->txtProcessID->setReadOnly(true);
        ui->txtThreadID->setText(QString::fromStdString(entry.ThreadID()));
        ui->txtThreadID->setReadOnly(true);
        ui->txtTraceLevel->setText(QString::fromStdString(entry.Level()));
        ui->txtTraceLevel->setReadOnly(true);
        ui->txtTime->setText(QString::fromStdString(entry.Time()));
        ui->txtTime->setReadOnly(true);
        ui->textEdit->setText(QString::fromStdString(entry.Description()));
        ui->textEdit->setReadOnly(true);

        connect(ui->btnClose, &QPushButton::clicked, this, &LogEntryView::onBtnClose_clicked);
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

void LogEntryView::onBtnClose_clicked(void)
{
    CFuncTracer trace("LogEntryView::onBtnClose_clicked", m_trace);
    try
    {
        close();
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

LogEntryView::~LogEntryView()
{
    CFuncTracer trace("LogEntryView::~LogEntryView", m_trace);
    try
    {
        delete ui;
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
