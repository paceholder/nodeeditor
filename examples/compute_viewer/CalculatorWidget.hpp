#pragma once

#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QString>
#include <QWidget>
#include <QtNodes/Definitions>

using QtNodes::NodeId;
using QtNodes::PortIndex;
using QtNodes::PortType;

class CalculatorWidget : public QWidget
{
    Q_OBJECT
public:
    CalculatorWidget(QString &_f, QWidget *parent = nullptr);

    ~CalculatorWidget() = default;

    void setF(const QString &f) { _exp_line->setText(f); }

private:
    QLabel *exp_label;
    QLineEdit *_exp_line;
};
