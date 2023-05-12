#ifndef LOGENTRYVIEW_H
#define LOGENTRYVIEW_H

#include <clogfile.h>
#include <QWidget>

namespace Ui {
class LogEntryView;
}

class LogEntryView : public QWidget
{
    Q_OBJECT

public:
    explicit LogEntryView(const CLogEntry& entry, std::shared_ptr<CTracer> tracer, QWidget *parent = nullptr);
    ~LogEntryView();

private slots:
    void onBtnClose_clicked(void);

private:
    Ui::LogEntryView *ui;
    std::shared_ptr<CTracer> m_trace;
};

#endif // LOGENTRYVIEW_H
