#pragma once

#include <QLineEdit>
#include <QObject>
#include <QString>
#include <QWidget>
#include <QtNodes/Definitions>

using QtNodes::NodeId;
using QtNodes::PortIndex;
using QtNodes::PortType;

class RoiSphereWidget : public QWidget
{
    Q_OBJECT
public:
    RoiSphereWidget(float &x, float &y, float &z, float &r, QWidget *parent = nullptr);

    ~RoiSphereWidget() = default;

    void setX(float f) { _x->setText(QString::number(f)); }
    void setY(float f) { _y->setText(QString::number(f)); }
    void setZ(float f) { _z->setText(QString::number(f)); }
    void setR(float f) { _r->setText(QString::number(f)); }

private:
    QLineEdit *_x;
    QLineEdit *_y;
    QLineEdit *_z;
    QLineEdit *_r;
};
