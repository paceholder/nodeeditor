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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of Qt Designer.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef QTPROPERTYBROWSERUTILS_H
#define QTPROPERTYBROWSERUTILS_H

#include <QtCore/QMap>
#include <QtGui/QIcon>
#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QSpinBox>
#include <QtCore/QDateTime>
#include <QtCore/QStringList>

QT_BEGIN_NAMESPACE

class QMouseEvent;
class QCheckBox;
class QLineEdit;

class QtCursorDatabase
{
public:
	QtCursorDatabase();
	void clear();

	QStringList cursorShapeNames() const;
	QMap<int, QIcon> cursorShapeIcons() const;
	QString cursorToShapeName(const QCursor& cursor) const;
	QIcon cursorToShapeIcon(const QCursor& cursor) const;
	int cursorToValue(const QCursor& cursor) const;
#ifndef QT_NO_CURSOR
	QCursor valueToCursor(int value) const;
#endif
private:
	void appendCursor(Qt::CursorShape shape, const QString& name, const QIcon& icon);
	QStringList m_cursorNames;
	QMap<int, QIcon> m_cursorIcons;
	QMap<int, Qt::CursorShape> m_valueToCursorShape;
	QMap<Qt::CursorShape, int> m_cursorShapeToValue;
};

class QtPropertyBrowserUtils
{
public:
	static QPixmap brushValuePixmap(const QBrush& b);
	static QIcon brushValueIcon(const QBrush& b);
	static QString colorValueText(const QColor& c);
	static QPixmap fontValuePixmap(const QFont& f);
	static QIcon fontValueIcon(const QFont& f);
	static QString fontValueText(const QFont& f);
	static QString dateFormat();
	static QString timeFormat();
	static QString dateTimeFormat();
};

#pragma region QtBoolEdit

class QtBoolEdit : public QWidget {
	Q_OBJECT
public:
	QtBoolEdit(QWidget* parent = 0);

	bool textVisible() const { return m_textVisible; }
	void setTextVisible(bool textVisible);

	Qt::CheckState checkState() const;
	void setCheckState(Qt::CheckState state);

	bool isChecked() const;
	void setChecked(bool c);
	void setInitialChecked(bool c);

	bool blockCheckBoxSignals(bool block);

Q_SIGNALS:
	void toggled(bool);

protected:
	void mousePressEvent(QMouseEvent* event) override;

private:
	QCheckBox* m_checkBox;
	QPushButton* m_button;
	bool m_textVisible;
	bool m_initialChecked;
};
#pragma endregion

#pragma region QtIntEdit

class QtIntEdit : public QWidget {
	Q_OBJECT
public:
	QtIntEdit(QWidget* parent = Q_NULLPTR);

	int value();
	void setValue(int);
	void setInitialValue(int);
	void setSingleStep(int);
	void setRange(int, int);
	void setKeyboardTracking(bool);

Q_SIGNALS:
	void valueChanged(int);

private:
	QSpinBox* m_spinbox;
	QPushButton* m_button;
	int m_initialvalue;
};

#pragma endregion

#pragma region QtDoubleEdit

class QtDoubleEdit : public QWidget {
	Q_OBJECT
public:
	QtDoubleEdit(QWidget* parent = Q_NULLPTR);

	int value();
	void setValue(double);
	void setInitialValue(double);
	void setDecimals(double);
	void setSingleStep(double);
	void setRange(double, double);
	void setKeyboardTracking(bool);

Q_SIGNALS:
	void valueChanged(double);

private:
	QDoubleSpinBox* m_doublespinbox;
	QPushButton* m_button;
	double m_initialvalue;
};

#pragma endregion

#pragma region QtStringEdit

class QtStringEdit : public QWidget {
	Q_OBJECT
public:
	QtStringEdit(QWidget* parent = Q_NULLPTR);

	QString text();
	void setText(QString);
	void setInitialText(QString);
	const QValidator* validator();
	void setValidator(QValidator*);

Q_SIGNALS:
	void textChanged(QString);
	void textEdited(QString);

private:
	QLineEdit* m_lineedit;
	QPushButton* m_button;
	QString m_initialvalue;
};

#pragma endregion

#pragma region QtDateEdit

class QtDateEdit : public QWidget {
	Q_OBJECT
public:
	QtDateEdit(QWidget* parent = Q_NULLPTR);

	QDate date();
	void setDate(QDate);
	void setInitialDate(QDate);
	void setDateRange(QDate, QDate);
	void setDisplayFormat(QString);
	void setCalendarPopup(bool);

Q_SIGNALS:
	void dateChanged(QDate);

private:
	QDateEdit* m_dateedit;
	QPushButton* m_button;
	QDate m_initialvalue;
};

#pragma endregion

#pragma region QtTimeEdit

class QtTimeEdit : public QWidget {
	Q_OBJECT
public:
	QtTimeEdit(QWidget* parent = Q_NULLPTR);

	QTime time();
	void setTime(QTime);
	void setInitialTime(QTime);
	void setTimeRange(QTime, QTime);
	void setDisplayFormat(QString);
	void setCalendarPopup(bool);

Q_SIGNALS:
	void timeChanged(QTime);

private:
	QTimeEdit* m_timeedit;
	QPushButton* m_button;
	QTime m_initialvalue;
};

#pragma endregion

#pragma region QDateTimeEdit

class QtDateTimeEdit : public QWidget {
	Q_OBJECT
public:
	QtDateTimeEdit(QWidget* parent = Q_NULLPTR);

	QDateTime dateTime();
	void setDateTime(const QDateTime&);
	void setInitialDateTime(const QDateTime&);
	void setDateTimeRange(const QDateTime&, const QDateTime&);
	void setDisplayFormat(const QString&);
	void setCalendarPopup(bool);

Q_SIGNALS:
	void dateTimeChanged(const QDateTime&);

private:
	QDateTimeEdit* m_timeedit;
	QPushButton* m_button;
	QDateTime m_initialvalue;
};

#pragma endregion


QT_END_NAMESPACE

#endif
