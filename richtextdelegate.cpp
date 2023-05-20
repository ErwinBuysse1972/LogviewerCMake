#include "richtextdelegate.h"
#include "qfilelogfilemodel.h"

RichTextDelegate::RichTextDelegate(std::shared_ptr<CTracer> tracer, QObject *parent)
    : QItemDelegate(parent)
    , m_trace(tracer)
{
    CFuncTracer trace("RichTextDelegate::RichTextDelegate", m_trace);
}

void RichTextDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    CFuncTracer trace("RichTextDelegate::paint", m_trace);
/*    CLogEntry entry = index.data(Qt::UserRole).value<CLogEntry>();
    trace.Trace("col :%ld, row:%ld, entry: %s",index.column(), index.row(), entry.Time().c_str());
    trace.Trace("Mark : %s, Required : %s, required text : %s"
                , (entry.IsMarked() == true)?"TRUE":"FALSE"
                , (entry.IsEntryRequired() == true)?"TRUE":"FALSE"
                , (entry.GetRequiredText().c_str()));
    if (   (index.column() == (int)eColumns::eDescription)
        &&(entry.IsEntryRequired()) )
    {
        trace.Trace("text should be placed in red: %s", entry.Description().c_str());
        painter->save();
        QStyleOptionViewItem opt = setOptions(index, option);
        drawBackground(painter, opt, index);
        painter->setFont(opt.font);
        painter->setPen(Qt::red);
        opt.backgroundBrush = QBrush(QColor(230,200,200));
        opt.font.setBold(true);
        opt.font.setUnderline(true);
        drawBackground(painter, opt, index);
        painter->setFont(opt.font);
        painter->drawText(opt.rect, opt.displayAlignment, QString::fromStdString(entry.Description().c_str()));
        drawFocus(painter, opt, option.rect);
        painter->restore();
    }
    else*/
        QItemDelegate::paint(painter, option, index);
}

QSize RichTextDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    CFuncTracer trace("RichTextDelegate::sizeHint", m_trace);
    return QItemDelegate::sizeHint(option, index);
}

QWidget *RichTextDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    CFuncTracer trace("RichTextDelegate::createEditor", m_trace);
    try
    {
        if (index.column() == (int)eColumns::eDescription)
            return new QTextEdit();
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
    return QItemDelegate::createEditor(parent, option, index);
}

void RichTextDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    CFuncTracer trace("RichTextDelegate::setEditorData", m_trace);
    QItemDelegate::setEditorData(editor, index);
}

void RichTextDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    CFuncTracer trace("RichTextDelegate::setModelData", m_trace);
    try
    {
        QLogFileModel *logModel = qobject_cast<QLogFileModel *>(model);
        if (logModel != nullptr)
        {
            auto entry = logModel->getLogEntry(index.row());
            if(entry)
            {
                CLogEntry* lpEntry = reinterpret_cast<CLogEntry*>(&entry);
                if (lpEntry != nullptr)
                {
                    if (lpEntry->IsEntryRequired() == true)
                    {
                        QTextEdit *editText = qobject_cast<QTextEdit *>(editor);
                        if (editText)
                        {
                            QList<QTextEdit::ExtraSelection> selections;
                            QTextCharFormat fmt;
                            fmt.setUnderlineStyle(QTextCharFormat::SingleUnderline);
                            fmt.setForeground(QBrush(QColor(200,0,0)));
                            fmt.setUnderlineColor(QColor(200,0,0));
                            fmt.setBackground(QBrush(QColor(230,200,200)));
                            QTextCursor cursor = editText->textCursor();
                            while (! (cursor = editText->document()->find(
                                         QString::fromStdString(lpEntry->GetRequiredText()), cursor)).isNull())
                            {
                                QTextEdit::ExtraSelection sel = {cursor, fmt };
                                selections.append(sel);
                            }
                            // set, show, go!
                            editText->setExtraSelections(selections);
                        }
                    }
                }
            }
            else
                trace.Error("entry is not available for index: %ld", index.row());
        }
        else
            trace.Error("model is not a QLogFileModel!");
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

void RichTextDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    CFuncTracer trace("RichTextDelegate::updateEditorGeometry", m_trace);
    QItemDelegate::updateEditorGeometry(editor, option, index);
}

bool RichTextDelegate::eventFilter(QObject *object, QEvent *event)
{
    CFuncTracer trace("RichTextDelegate::eventFilter", m_trace);
    return QItemDelegate::eventFilter(object, event);
}

bool RichTextDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    CFuncTracer trace("RichTextDelegate::editorEvent", m_trace);
    return  QItemDelegate::editorEvent(event, model, option, index);
}
