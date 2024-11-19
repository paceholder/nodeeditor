#pragma once

#include <QLineEdit>
#include <QObject>
#include <QSlider>
#include <QString>
#include <QWidget>
#include <QtNodes/Definitions>

using QtNodes::NodeId;
using QtNodes::PortIndex;
using QtNodes::PortType;

class ClutterFilterWidget : public QWidget
{
    Q_OBJECT
public:
    ClutterFilterWidget(float &_f, QWidget *parent = nullptr);

    ~ClutterFilterWidget() = default;

    void setF(float f)
    {
        _value_scroll->setValue(static_cast<int>(f * 100.));
        _value_line->setText(QString::number(f));
    }

private:
    QLineEdit *_value_line;
    QSlider *_value_scroll;
};
