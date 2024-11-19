#include "DecimationWidget.hpp"
#include <QDoubleValidator>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSizePolicy>

DecimationWidget::DecimationWidget(float &_m, QWidget *parent)
    : QWidget(nullptr)
{
    setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);

    QGridLayout *layout = new QGridLayout(this);

    QLabel *m_label = new QLabel("M", this);
    layout->addWidget(m_label, 0, 0, 1, 1);
    _m_line = new QLineEdit(this);
    _m_line->setValidator(new QDoubleValidator());
    _m_line->setText(QString::number(_m));
    _m_line->setFixedWidth(40);
    layout->addWidget(_m_line, 0, 1, 1, 1);
    connect(_m_line, &QLineEdit::textChanged, [this, &_m](QString obj) { _m = obj.toFloat(); });
}
