#pragma once
#include <QObject>
#include <QTableView>
#include <QWidget>
#include <QMouseEvent>
#include <QModelIndex>
#include <QPoint>

class QLogfileTableView : public QTableView
{
    Q_OBJECT
public:
    explicit QLogfileTableView(QWidget *parent = nullptr);
    virtual ~QLogfileTableView(){}
    void RegisterRightClick(std::function<void(QModelIndex, QPoint)> event){ m_onCellClickRight = event;}
    void RegisterLeftClick(std::function<void(QModelIndex, QPoint)> event){ m_onCellClickLeft = event;}

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    std::function<void(QModelIndex, QPoint)> m_onCellClickRight;
    std::function<void(QModelIndex, QPoint)> m_onCellClickLeft;
};
