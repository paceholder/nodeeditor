#pragma once

#include <QLineEdit>
#include <QObject>
#include <QString>
#include <QWidget>
#include <QtNodes/Definitions>

using QtNodes::NodeId;
using QtNodes::PortIndex;
using QtNodes::PortType;

class DasPwWidget : public QWidget
{
    Q_OBJECT
public:
    DasPwWidget(float &_f, float &_c0, QWidget *parent = nullptr);

    ~DasPwWidget() = default;

    void setF(float f) { _f_line->setText(QString::number(f)); }

    void setC0(float c0) { _c0_line->setText(QString::number(c0)); }

private:
    QLineEdit *_f_line;
    QLineEdit *_c0_line;
};
