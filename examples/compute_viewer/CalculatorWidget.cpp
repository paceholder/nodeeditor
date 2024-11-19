#include "CalculatorWidget.hpp"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSizePolicy>

CalculatorWidget::CalculatorWidget(QString &_value, QWidget *parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);

    QGridLayout *layout = new QGridLayout(this);

    QLabel *exp_label = new QLabel("Formula", this);
    layout->addWidget(exp_label, 0, 0, 1, 1);
    _exp_line = new QLineEdit(this);
    _exp_line->setText(_value);
    layout->addWidget(_exp_line, 0, 1, 1, 1);
    connect(_exp_line, &QLineEdit::textChanged, [this, &_value](QString obj) { _value = obj; });
}
