/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qtpropertybrowserutils_p.h"
#include <QtWidgets/QApplication>
#include <QtGui/QPainter>
#include <QtWidgets/QHBoxLayout>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMenu>
#include <QtCore/QLocale>

QT_BEGIN_NAMESPACE

QtCursorDatabase::QtCursorDatabase()
{
	appendCursor(Qt::ArrowCursor, QCoreApplication::translate("QtCursorDatabase", "Arrow"),
		QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-arrow.png")));
	appendCursor(Qt::UpArrowCursor, QCoreApplication::translate("QtCursorDatabase", "Up Arrow"),
		QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-uparrow.png")));
	appendCursor(Qt::CrossCursor, QCoreApplication::translate("QtCursorDatabase", "Cross"),
		QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-cross.png")));
	appendCursor(Qt::WaitCursor, QCoreApplication::translate("QtCursorDatabase", "Wait"),
		QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-wait.png")));
	appendCursor(Qt::IBeamCursor, QCoreApplication::translate("QtCursorDatabase", "IBeam"),
		QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-ibeam.png")));
	appendCursor(Qt::SizeVerCursor, QCoreApplication::translate("QtCursorDatabase", "Size Vertical"),
		QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-sizev.png")));
	appendCursor(Qt::SizeHorCursor, QCoreApplication::translate("QtCursorDatabase", "Size Horizontal"),
		QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-sizeh.png")));
	appendCursor(Qt::SizeFDiagCursor, QCoreApplication::translate("QtCursorDatabase", "Size Backslash"),
		QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-sizef.png")));
	appendCursor(Qt::SizeBDiagCursor, QCoreApplication::translate("QtCursorDatabase", "Size Slash"),
		QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-sizeb.png")));
	appendCursor(Qt::SizeAllCursor, QCoreApplication::translate("QtCursorDatabase", "Size All"),
		QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-sizeall.png")));
	appendCursor(Qt::BlankCursor, QCoreApplication::translate("QtCursorDatabase", "Blank"),
		QIcon());
	appendCursor(Qt::SplitVCursor, QCoreApplication::translate("QtCursorDatabase", "Split Vertical"),
		QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-vsplit.png")));
	appendCursor(Qt::SplitHCursor, QCoreApplication::translate("QtCursorDatabase", "Split Horizontal"),
		QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-hsplit.png")));
	appendCursor(Qt::PointingHandCursor, QCoreApplication::translate("QtCursorDatabase", "Pointing Hand"),
		QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-hand.png")));
	appendCursor(Qt::ForbiddenCursor, QCoreApplication::translate("QtCursorDatabase", "Forbidden"),
		QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-forbidden.png")));
	appendCursor(Qt::OpenHandCursor, QCoreApplication::translate("QtCursorDatabase", "Open Hand"),
		QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-openhand.png")));
	appendCursor(Qt::ClosedHandCursor, QCoreApplication::translate("QtCursorDatabase", "Closed Hand"),
		QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-closedhand.png")));
	appendCursor(Qt::WhatsThisCursor, QCoreApplication::translate("QtCursorDatabase", "What's This"),
		QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-whatsthis.png")));
	appendCursor(Qt::BusyCursor, QCoreApplication::translate("QtCursorDatabase", "Busy"),
		QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/cursor-busy.png")));
}

void QtCursorDatabase::clear()
{
	m_cursorNames.clear();
	m_cursorIcons.clear();
	m_valueToCursorShape.clear();
	m_cursorShapeToValue.clear();
}

void QtCursorDatabase::appendCursor(Qt::CursorShape shape, const QString& name, const QIcon& icon)
{
	if (m_cursorShapeToValue.contains(shape))
		return;
	const int value = m_cursorNames.count();
	m_cursorNames.append(name);
	m_cursorIcons.insert(value, icon);
	m_valueToCursorShape.insert(value, shape);
	m_cursorShapeToValue.insert(shape, value);
}

QStringList QtCursorDatabase::cursorShapeNames() const
{
	return m_cursorNames;
}

QMap<int, QIcon> QtCursorDatabase::cursorShapeIcons() const
{
	return m_cursorIcons;
}

QString QtCursorDatabase::cursorToShapeName(const QCursor& cursor) const
{
	int val = cursorToValue(cursor);
	if (val >= 0)
		return m_cursorNames.at(val);
	return QString();
}

QIcon QtCursorDatabase::cursorToShapeIcon(const QCursor& cursor) const
{
	int val = cursorToValue(cursor);
	return m_cursorIcons.value(val);
}

int QtCursorDatabase::cursorToValue(const QCursor& cursor) const
{
#ifndef QT_NO_CURSOR
	Qt::CursorShape shape = cursor.shape();
	if (m_cursorShapeToValue.contains(shape))
		return m_cursorShapeToValue[shape];
#endif
	return -1;
}

#ifndef QT_NO_CURSOR
QCursor QtCursorDatabase::valueToCursor(int value) const
{
	if (m_valueToCursorShape.contains(value))
		return QCursor(m_valueToCursorShape[value]);
	return QCursor();
}
#endif

QPixmap QtPropertyBrowserUtils::brushValuePixmap(const QBrush& b)
{
	QImage img(16, 16, QImage::Format_ARGB32_Premultiplied);
	img.fill(0);

	QPainter painter(&img);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.fillRect(0, 0, img.width(), img.height(), b);
	QColor color = b.color();
	if (color.alpha() != 255) { // indicate alpha by an inset
		QBrush  opaqueBrush = b;
		color.setAlpha(255);
		opaqueBrush.setColor(color);
		painter.fillRect(img.width() / 4, img.height() / 4,
			img.width() / 2, img.height() / 2, opaqueBrush);
	}
	painter.end();
	return QPixmap::fromImage(img);
}

QIcon QtPropertyBrowserUtils::brushValueIcon(const QBrush& b)
{
	return QIcon(brushValuePixmap(b));
}

QString QtPropertyBrowserUtils::colorValueText(const QColor& c)
{
	return QCoreApplication::translate("QtPropertyBrowserUtils", "[%1, %2, %3] (%4)")
		.arg(c.red()).arg(c.green()).arg(c.blue()).arg(c.alpha());
}

QPixmap QtPropertyBrowserUtils::fontValuePixmap(const QFont& font)
{
	QFont f = font;
	QImage img(16, 16, QImage::Format_ARGB32_Premultiplied);
	img.fill(0);
	QPainter p(&img);
	p.setRenderHint(QPainter::TextAntialiasing, true);
	p.setRenderHint(QPainter::Antialiasing, true);
	f.setPointSize(13);
	p.setFont(f);
	QTextOption t;
	t.setAlignment(Qt::AlignCenter);
	p.drawText(QRect(0, 0, 16, 16), QString(QLatin1Char('A')), t);
	return QPixmap::fromImage(img);
}

QIcon QtPropertyBrowserUtils::fontValueIcon(const QFont& f)
{
	return QIcon(fontValuePixmap(f));
}

QString QtPropertyBrowserUtils::fontValueText(const QFont& f)
{
	return QCoreApplication::translate("QtPropertyBrowserUtils", "[%1, %2]")
		.arg(f.family()).arg(f.pointSize());
}

QString QtPropertyBrowserUtils::dateFormat()
{
	QLocale loc;
	QString format = loc.dateFormat(QLocale::ShortFormat);
	// Change dd.MM.yy, MM/dd/yy to 4 digit years
	if (format.count(QLatin1Char('y')) == 2)
		format.insert(format.indexOf(QLatin1Char('y')), QLatin1String("yy"));
	return format;
}

QString QtPropertyBrowserUtils::timeFormat()
{
	QLocale loc;
	// ShortFormat is missing seconds on UNIX.
	return loc.timeFormat(QLocale::LongFormat);
}

QString QtPropertyBrowserUtils::dateTimeFormat()
{
	QString format = dateFormat();
	format += QLatin1Char(' ');
	format += timeFormat();
	return format;
}

#pragma region QtBoolEdit

QtBoolEdit::QtBoolEdit(QWidget* parent) :
	QWidget(parent),
	m_checkBox(new QCheckBox(this)),
	m_button(new QPushButton(this)),
	m_textVisible(true)
{
	m_button->setIcon(QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/button-reset.ico")));
	m_button->setMaximumWidth(15);

	QHBoxLayout* lt = new QHBoxLayout;
	if (QApplication::layoutDirection() == Qt::LeftToRight)
		lt->setContentsMargins(4, 0, 0, 0);
	else
		lt->setContentsMargins(0, 0, 4, 0);
	lt->setSpacing(1);
	lt->addWidget(m_checkBox);
	lt->addWidget(m_button);
	lt->setStretch(0, 1);
	lt->setStretch(1, 0);
	setLayout(lt);
	connect(m_checkBox, SIGNAL(toggled(bool)), this, SIGNAL(toggled(bool)));
	connect(m_checkBox, &QCheckBox::toggled, this, [&] { m_button->setEnabled(m_initialChecked != m_checkBox->isChecked()); });
	connect(m_button, &QPushButton::clicked, this, [&] { this->setChecked(m_initialChecked); });
	setFocusProxy(m_checkBox);
	m_checkBox->setText(tr("True"));
}

void QtBoolEdit::setTextVisible(bool textVisible)
{
	if (m_textVisible == textVisible)
		return;

	m_textVisible = textVisible;
	if (m_textVisible)
		m_checkBox->setText(isChecked() ? tr("True") : tr("False"));
	else
		m_checkBox->setText(QString());
}

Qt::CheckState QtBoolEdit::checkState() const
{
	return m_checkBox->checkState();
}

void QtBoolEdit::setCheckState(Qt::CheckState state)
{
	m_checkBox->setCheckState(state);
}

bool QtBoolEdit::isChecked() const
{
	return m_checkBox->isChecked();
}

void QtBoolEdit::setChecked(bool c)
{
	m_checkBox->setChecked(c);
	if (!m_textVisible)
		return;
	m_checkBox->setText(isChecked() ? tr("True") : tr("False"));
}

void QtBoolEdit::setInitialChecked(bool c)
{
	m_initialChecked = c;
	m_button->setEnabled(m_initialChecked != m_checkBox->isChecked());
}

bool QtBoolEdit::blockCheckBoxSignals(bool block)
{
	return m_checkBox->blockSignals(block);
}

void QtBoolEdit::mousePressEvent(QMouseEvent* event)
{
	if (event->buttons() == Qt::LeftButton) {
		m_checkBox->click();
		event->accept();
	}
	else {
		QWidget::mousePressEvent(event);
	}
}

#pragma endregion

#pragma region QtIntEdit

QtIntEdit::QtIntEdit(QWidget* parent) :
	QWidget(parent),
	m_spinbox(new QSpinBox(this)),
	m_button(new QPushButton(this))
{
	m_button->setIcon(QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/button-reset.ico")));
	m_button->setMaximumWidth(15);

	QHBoxLayout* lt = new QHBoxLayout;
	lt->setContentsMargins(0, 0, 0, 0);
	lt->setSpacing(1);
	lt->addWidget(m_spinbox);
	lt->addWidget(m_button);
	lt->setStretch(0, 1);
	lt->setStretch(1, 0);
	setLayout(lt);

	m_initialvalue = m_spinbox->value();
	connect(m_spinbox, &QSpinBox::valueChanged, this, &QtIntEdit::valueChanged);
	connect(m_spinbox, &QSpinBox::valueChanged, this, [&] { m_button->setEnabled(m_initialvalue != m_spinbox->value()); });
	connect(m_button, &QPushButton::clicked, this, [&] { m_spinbox->setValue(m_initialvalue); });
	setFocusProxy(m_spinbox);
}

int QtIntEdit::value()
{
	return m_spinbox->value();
}

void QtIntEdit::setValue(int val)
{
	m_spinbox->setValue(val);
}

void QtIntEdit::setInitialValue(int val)
{
	m_initialvalue = val;
	m_button->setEnabled(val != m_spinbox->value());
}

void QtIntEdit::setSingleStep(int step)
{
	m_spinbox->setSingleStep(step);
}

void QtIntEdit::setRange(int min, int max)
{
	m_spinbox->setRange(min, max);
}

void QtIntEdit::setKeyboardTracking(bool kt)
{
	m_spinbox->setKeyboardTracking(kt);
}

#pragma endregion

#pragma region QtDoubleEdit

QtDoubleEdit::QtDoubleEdit(QWidget* parent) :
	QWidget(parent),
	m_doublespinbox(new QDoubleSpinBox(this)),
	m_button(new QPushButton(this))
{
	m_button->setIcon(QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/button-reset.ico")));
	m_button->setMaximumWidth(15);

	QHBoxLayout* lt = new QHBoxLayout;
	lt->setContentsMargins(0, 0, 0, 0);
	lt->setSpacing(1);
	lt->addWidget(m_doublespinbox);
	lt->addWidget(m_button);
	lt->setStretch(0, 1);
	lt->setStretch(1, 0);
	setLayout(lt);

	m_initialvalue = m_doublespinbox->value();
	connect(m_doublespinbox, &QDoubleSpinBox::valueChanged, this, &QtDoubleEdit::valueChanged);
	connect(m_doublespinbox, &QDoubleSpinBox::valueChanged, this, [&] {
		int e10 = pow(10, m_doublespinbox->decimals());
		m_button->setEnabled(int(m_initialvalue + 0.5) * e10 != int(m_doublespinbox->value() + 0.5) * e10); });
	connect(m_button, &QPushButton::clicked, this, [&] { m_doublespinbox->setValue(m_initialvalue); });
	setFocusProxy(m_doublespinbox);
}

int QtDoubleEdit::value()
{
	return m_doublespinbox->value();
}

void QtDoubleEdit::setValue(double val)
{
	m_doublespinbox->setValue(val);
}

void QtDoubleEdit::setInitialValue(double val)
{
	int e10 = pow(10, m_doublespinbox->decimals());
	m_initialvalue = val;
	m_button->setEnabled(int(val + 0.5) * e10 != int(m_doublespinbox->value() + 0.5) * e10);
}

void QtDoubleEdit::setDecimals(double val)
{
	m_doublespinbox->setDecimals(val);
}

void QtDoubleEdit::setSingleStep(double step)
{
	m_doublespinbox->setSingleStep(step);
}

void QtDoubleEdit::setRange(double min, double max)
{
	m_doublespinbox->setRange(min, max);
}

void QtDoubleEdit::setKeyboardTracking(bool kt)
{
	m_doublespinbox->setKeyboardTracking(kt);
}

#pragma endregion

#pragma region QtStringEdit

QtStringEdit::QtStringEdit(QWidget* parent) :
	QWidget(parent),
	m_lineedit(new QLineEdit(this)),
	m_button(new QPushButton(this))
{
	m_button->setIcon(QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/button-reset.ico")));
	m_button->setMaximumWidth(15);

	QHBoxLayout* lt = new QHBoxLayout;
	lt->setContentsMargins(0, 0, 0, 0);
	lt->setSpacing(1);
	lt->addWidget(m_lineedit);
	lt->addWidget(m_button);
	lt->setStretch(0, 1);
	lt->setStretch(1, 0);
	setLayout(lt);

	m_initialvalue = m_lineedit->text();
	connect(m_lineedit, &QLineEdit::textChanged, this, &QtStringEdit::textChanged);
	connect(m_lineedit, &QLineEdit::textEdited, this, &QtStringEdit::textEdited);
	connect(m_lineedit, &QLineEdit::textChanged, this, [&] { m_button->setEnabled(m_initialvalue != m_lineedit->text()); });
	connect(m_lineedit, &QLineEdit::textEdited, this, [&] { m_button->setEnabled(m_initialvalue != m_lineedit->text()); });
	connect(m_button, &QPushButton::clicked, this, [&] { m_lineedit->setText(m_initialvalue); });
	setFocusProxy(m_lineedit);
}

QString QtStringEdit::text()
{
	return m_lineedit->text();
}

void QtStringEdit::setText(QString val)
{
	m_lineedit->setText(val);
}

const QValidator* QtStringEdit::validator()
{
	return m_lineedit->validator();
}

void QtStringEdit::setValidator(QValidator* val)
{
	m_lineedit->setValidator(val);
}

void QtStringEdit::setInitialText(QString val)
{
	m_initialvalue = val;
	m_button->setEnabled(val != m_lineedit->text());
}

#pragma endregion

#pragma region QtDateEdit

QtDateEdit::QtDateEdit(QWidget* parent) :
	QWidget(parent),
	m_dateedit(new QDateEdit(this)),
	m_button(new QPushButton(this))
{
	m_button->setIcon(QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/button-reset.ico")));
	m_button->setMaximumWidth(15);

	QHBoxLayout* lt = new QHBoxLayout;
	lt->setContentsMargins(0, 0, 0, 0);
	lt->setSpacing(1);
	lt->addWidget(m_dateedit);
	lt->addWidget(m_button);
	lt->setStretch(0, 1);
	lt->setStretch(1, 0);
	setLayout(lt);

	m_initialvalue = m_dateedit->date();
	connect(m_dateedit, &QDateEdit::dateChanged, this, &QtDateEdit::dateChanged);
	connect(m_dateedit, &QDateEdit::dateChanged, this, [&] { m_button->setEnabled(m_initialvalue != m_dateedit->date()); });
	connect(m_button, &QPushButton::clicked, this, [&] { m_dateedit->setDate(m_initialvalue); });
	setFocusProxy(m_dateedit);
}

QDate QtDateEdit::date()
{
	return m_dateedit->date();
}

void QtDateEdit::setDate(QDate val)
{
	m_dateedit->setDate(val);
}

void QtDateEdit::setInitialDate(QDate val)
{
	m_initialvalue = val;
	m_button->setEnabled(val != m_dateedit->date());
}

void QtDateEdit::setDateRange(QDate min, QDate max)
{
	m_dateedit->setDateRange(min, max);
}

void QtDateEdit::setDisplayFormat(QString format)
{
	m_dateedit->setDisplayFormat(format);
}

void QtDateEdit::setCalendarPopup(bool popup)
{
	m_dateedit->setCalendarPopup(popup);
}

#pragma endregion

#pragma region QtTimeEdit

QtTimeEdit::QtTimeEdit(QWidget* parent) :
	QWidget(parent),
	m_timeedit(new QTimeEdit(this)),
	m_button(new QPushButton(this))
{
	m_button->setIcon(QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/button-reset.ico")));
	m_button->setMaximumWidth(15);

	QHBoxLayout* lt = new QHBoxLayout;
	lt->setContentsMargins(0, 0, 0, 0);
	lt->setSpacing(1);
	lt->addWidget(m_timeedit);
	lt->addWidget(m_button);
	lt->setStretch(0, 1);
	lt->setStretch(1, 0);
	setLayout(lt);

	m_initialvalue = m_timeedit->time();
	connect(m_timeedit, &QTimeEdit::timeChanged, this, &QtTimeEdit::timeChanged);
	connect(m_timeedit, &QTimeEdit::timeChanged, this, [&] { m_button->setEnabled(m_initialvalue != m_timeedit->time()); });
	connect(m_button, &QPushButton::clicked, this, [&] { m_timeedit->setTime(m_initialvalue); });
	setFocusProxy(m_timeedit);
}

QTime QtTimeEdit::time()
{
	return m_timeedit->time();
}

void QtTimeEdit::setTime(QTime val)
{
	m_timeedit->setTime(val);
}

void QtTimeEdit::setInitialTime(QTime val)
{
	m_initialvalue = val;
	m_button->setEnabled(val != m_timeedit->time());
}

void QtTimeEdit::setTimeRange(QTime min, QTime max)
{
	m_timeedit->setTimeRange(min, max);
}

void QtTimeEdit::setDisplayFormat(QString format)
{
	m_timeedit->setDisplayFormat(format);
}

void QtTimeEdit::setCalendarPopup(bool popup)
{
	m_timeedit->setCalendarPopup(popup);
}

#pragma endregion

#pragma region QtDateTimeEdit

QtDateTimeEdit::QtDateTimeEdit(QWidget* parent) :
	QWidget(parent),
	m_timeedit(new QDateTimeEdit(this)),
	m_button(new QPushButton(this))
{
	m_button->setIcon(QIcon(QLatin1String(":/qt-project.org/qtpropertybrowser/images/button-reset.ico")));
	m_button->setMaximumWidth(15);

	QHBoxLayout* lt = new QHBoxLayout;
	lt->setContentsMargins(0, 0, 0, 0);
	lt->setSpacing(1);
	lt->addWidget(m_timeedit);
	lt->addWidget(m_button);
	lt->setStretch(0, 1);
	lt->setStretch(1, 0);
	setLayout(lt);

	m_initialvalue = m_timeedit->dateTime();
	connect(m_timeedit, &QTimeEdit::dateTimeChanged, this, &QtDateTimeEdit::dateTimeChanged);
	connect(m_timeedit, &QTimeEdit::dateTimeChanged, this, [&] { m_button->setEnabled(m_initialvalue != m_timeedit->dateTime()); });
	connect(m_button, &QPushButton::clicked, this, [&] { m_timeedit->setDateTime(m_initialvalue); });
	setFocusProxy(m_timeedit);
}

QDateTime QtDateTimeEdit::dateTime()
{
	return m_timeedit->dateTime();
}

void QtDateTimeEdit::setDateTime(const QDateTime& val)
{
	m_timeedit->setDateTime(val);
}

void QtDateTimeEdit::setInitialDateTime(const QDateTime& val)
{
	m_initialvalue = val;
	m_button->setEnabled(val != m_timeedit->dateTime());
}

void QtDateTimeEdit::setDateTimeRange(const QDateTime& min, const QDateTime& max)
{
	m_timeedit->setDateTimeRange(min, max);
}

void QtDateTimeEdit::setDisplayFormat(const QString& format)
{
	m_timeedit->setDisplayFormat(format);
}

void QtDateTimeEdit::setCalendarPopup(bool popup)
{
	m_timeedit->setCalendarPopup(popup);
}

#pragma endregion

QT_END_NAMESPACE
