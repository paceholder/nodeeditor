#pragma once

#include <QLineEdit>
#include <QObject>
#include <QString>
#include <QWidget>
#include <QtNodes/Definitions>

using QtNodes::NodeId;
using QtNodes::PortIndex;
using QtNodes::PortType;

class RoiRectWidget : public QWidget
{
    Q_OBJECT
public:
    RoiRectWidget(
        float &x1, float &x2, float &y1, float &y2, float &z1, float &z2, QWidget *parent = nullptr);

    ~RoiRectWidget() = default;

    void setX1(float f) { _x1->setText(QString::number(f)); }
    void setX2(float f) { _x2->setText(QString::number(f)); }
    void setY1(float f) { _y1->setText(QString::number(f)); }
    void setY2(float f) { _y2->setText(QString::number(f)); }
    void setZ1(float f) { _z1->setText(QString::number(f)); }
    void setZ2(float f) { _z2->setText(QString::number(f)); }

private:
    QLineEdit *_x1;
    QLineEdit *_x2;
    QLineEdit *_y1;
    QLineEdit *_y2;
    QLineEdit *_z1;
    QLineEdit *_z2;
};
