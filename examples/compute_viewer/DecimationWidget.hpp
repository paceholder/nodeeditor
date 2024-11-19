#pragma once

#include <QLineEdit>
#include <QObject>
#include <QString>
#include <QWidget>
#include <QtNodes/Definitions>

using QtNodes::NodeId;
using QtNodes::PortIndex;
using QtNodes::PortType;

class DecimationWidget : public QWidget
{
    Q_OBJECT
public:
    DecimationWidget(float &_m, QWidget *parent = nullptr);

    ~DecimationWidget() = default;

    void setM(float m) { _m_line->setText(QString::number(m)); }

private:
    QLineEdit *_m_line;
};
