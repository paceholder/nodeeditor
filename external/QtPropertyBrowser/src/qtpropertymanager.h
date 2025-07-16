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

#ifndef QTPROPERTYMANAGER_H
#define QTPROPERTYMANAGER_H

#include "qtpropertybrowser.h"

QT_BEGIN_NAMESPACE

class QDate;
class QTime;
class QDateTime;
class QLocale;
class QRegularExpression;

#pragma region QtGroupPropertyManager

class QT_QTPROPERTYBROWSER_EXPORT QtGroupPropertyManager : public QtAbstractPropertyManager
{
	Q_OBJECT
public:
	QtGroupPropertyManager(QObject* parent = 0);
	~QtGroupPropertyManager();

protected:
	bool hasValue(const QtProperty* property) const override;

	void initializeProperty(QtProperty* property) override;
	void uninitializeProperty(QtProperty* property) override;
};

#pragma endregion

#pragma region QtIntPropertyManager

class QtIntPropertyManagerPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtIntPropertyManager : public QtAbstractPropertyManager
{
	Q_OBJECT
public:
	QtIntPropertyManager(QObject* parent = 0);
	~QtIntPropertyManager();

	int value(const QtProperty* property) const;
	int initialValue(const QtProperty* property) const;
	int minimum(const QtProperty* property) const;
	int maximum(const QtProperty* property) const;
	int singleStep(const QtProperty* property) const;

public Q_SLOTS:
	void setValue(QtProperty* property, int val);
	void setValueOnly(QtProperty* property, int val);
	void setMinimum(QtProperty* property, int minVal);
	void setMaximum(QtProperty* property, int maxVal);
	void setRange(QtProperty* property, int minVal, int maxVal);
	void setSingleStep(QtProperty* property, int step);
Q_SIGNALS:
	void valueChanged(QtProperty* property, int val);
	void rangeChanged(QtProperty* property, int minVal, int maxVal);
	void singleStepChanged(QtProperty* property, int step);
protected:
	QString valueText(const QtProperty* property) const override;
	void initializeProperty(QtProperty* property) override;
	void uninitializeProperty(QtProperty* property) override;
private:
	QScopedPointer<QtIntPropertyManagerPrivate> d_ptr;
	Q_DECLARE_PRIVATE(QtIntPropertyManager)
		Q_DISABLE_COPY_MOVE(QtIntPropertyManager)
};
#pragma endregion

#pragma region QtBoolPropertyManager

class QtBoolPropertyManagerPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtBoolPropertyManager : public QtAbstractPropertyManager
{
	Q_OBJECT
public:
	QtBoolPropertyManager(QObject* parent = 0);
	~QtBoolPropertyManager();

	bool value(const QtProperty* property) const;
	bool initialValue(const QtProperty* property) const;

public Q_SLOTS:
	void setValue(QtProperty* property, bool val);
	void setValueOnly(QtProperty* property, bool val);
Q_SIGNALS:
	void valueChanged(QtProperty* property, bool val);
protected:
	QString valueText(const QtProperty* property) const override;
	QIcon valueIcon(const QtProperty* property) const override;
	void initializeProperty(QtProperty* property) override;
	void uninitializeProperty(QtProperty* property) override;
private:
	QScopedPointer<QtBoolPropertyManagerPrivate> d_ptr;
	Q_DECLARE_PRIVATE(QtBoolPropertyManager)
		Q_DISABLE_COPY_MOVE(QtBoolPropertyManager)
};

#pragma endregion

#pragma region QtDoublePropertyManager

class QtDoublePropertyManagerPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtDoublePropertyManager : public QtAbstractPropertyManager
{
	Q_OBJECT
public:
	QtDoublePropertyManager(QObject* parent = 0);
	~QtDoublePropertyManager();

	double value(const QtProperty* property) const;
	double initialValue(const QtProperty* property) const;
	double minimum(const QtProperty* property) const;
	double maximum(const QtProperty* property) const;
	double singleStep(const QtProperty* property) const;
	int decimals(const QtProperty* property) const;

public Q_SLOTS:
	void setValue(QtProperty* property, double val);
	void setValueOnly(QtProperty* property, double val);
	void setMinimum(QtProperty* property, double minVal);
	void setMaximum(QtProperty* property, double maxVal);
	void setRange(QtProperty* property, double minVal, double maxVal);
	void setSingleStep(QtProperty* property, double step);
	void setDecimals(QtProperty* property, int prec);
Q_SIGNALS:
	void valueChanged(QtProperty* property, double val);
	void rangeChanged(QtProperty* property, double minVal, double maxVal);
	void singleStepChanged(QtProperty* property, double step);
	void decimalsChanged(QtProperty* property, int prec);
protected:
	QString valueText(const QtProperty* property) const override;
	void initializeProperty(QtProperty* property) override;
	void uninitializeProperty(QtProperty* property) override;
private:
	QScopedPointer<QtDoublePropertyManagerPrivate> d_ptr;
	Q_DECLARE_PRIVATE(QtDoublePropertyManager)
		Q_DISABLE_COPY_MOVE(QtDoublePropertyManager)
};

#pragma endregion

#pragma region QtStringPropertyManager

class QtStringPropertyManagerPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtStringPropertyManager : public QtAbstractPropertyManager
{
	Q_OBJECT
public:
	QtStringPropertyManager(QObject* parent = 0);
	~QtStringPropertyManager();

	QString value(const QtProperty* property) const;
	QString initialValue(const QtProperty* property) const;
	QRegularExpression regExp(const QtProperty* property) const;

public Q_SLOTS:
	void setValue(QtProperty* property, const QString& val);
	void setValueOnly(QtProperty* property, const QString& val);
	void setRegExp(QtProperty* property, const QRegularExpression& regExp);
Q_SIGNALS:
	void valueChanged(QtProperty* property, const QString& val);
	void regExpChanged(QtProperty* property, const QRegularExpression& regExp);
protected:
	QString valueText(const QtProperty* property) const override;
	void initializeProperty(QtProperty* property) override;
	void uninitializeProperty(QtProperty* property) override;
private:
	QScopedPointer<QtStringPropertyManagerPrivate> d_ptr;
	Q_DECLARE_PRIVATE(QtStringPropertyManager)
		Q_DISABLE_COPY_MOVE(QtStringPropertyManager)
};

#pragma endregion

#pragma region QtDatePropertyManager

class QtDatePropertyManagerPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtDatePropertyManager : public QtAbstractPropertyManager
{
	Q_OBJECT
public:
	QtDatePropertyManager(QObject* parent = 0);
	~QtDatePropertyManager();

	QDate value(const QtProperty* property) const;
	QDate initialValue(const QtProperty* property) const;
	QDate minimum(const QtProperty* property) const;
	QDate maximum(const QtProperty* property) const;

public Q_SLOTS:
	void setValue(QtProperty* property, QDate val);
	void setValueOnly(QtProperty* property, QDate val);
	void setMinimum(QtProperty* property, QDate minVal);
	void setMaximum(QtProperty* property, QDate maxVal);
	void setRange(QtProperty* property, QDate minVal, QDate maxVal);
Q_SIGNALS:
	void valueChanged(QtProperty* property, QDate val);
	void rangeChanged(QtProperty* property, QDate minVal, QDate maxVal);
protected:
	QString valueText(const QtProperty* property) const override;
	void initializeProperty(QtProperty* property) override;
	void uninitializeProperty(QtProperty* property) override;
private:
	QScopedPointer<QtDatePropertyManagerPrivate> d_ptr;
	Q_DECLARE_PRIVATE(QtDatePropertyManager)
		Q_DISABLE_COPY_MOVE(QtDatePropertyManager)
};

#pragma endregion

#pragma region QtTimePropertyManager

class QtTimePropertyManagerPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtTimePropertyManager : public QtAbstractPropertyManager
{
	Q_OBJECT
public:
	QtTimePropertyManager(QObject* parent = 0);
	~QtTimePropertyManager();

	QTime value(const QtProperty* property) const;
	QTime initialValue(const QtProperty* property) const;

public Q_SLOTS:
	void setValue(QtProperty* property, QTime val);
	void setValueOnly(QtProperty* property, QTime val);
Q_SIGNALS:
	void valueChanged(QtProperty* property, QTime val);
protected:
	QString valueText(const QtProperty* property) const override;
	void initializeProperty(QtProperty* property) override;
	void uninitializeProperty(QtProperty* property) override;
private:
	QScopedPointer<QtTimePropertyManagerPrivate> d_ptr;
	Q_DECLARE_PRIVATE(QtTimePropertyManager)
		Q_DISABLE_COPY_MOVE(QtTimePropertyManager)
};

#pragma endregion

#pragma region QtDateTimePropertyManager

class QtDateTimePropertyManagerPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtDateTimePropertyManager : public QtAbstractPropertyManager
{
	Q_OBJECT
public:
	QtDateTimePropertyManager(QObject* parent = 0);
	~QtDateTimePropertyManager();

	QDateTime value(const QtProperty* property) const;
	QDateTime initialValue(const QtProperty* property) const;

public Q_SLOTS:
	void setValue(QtProperty* property, const QDateTime& val);
	void setValueOnly(QtProperty* property, const QDateTime& val);
Q_SIGNALS:
	void valueChanged(QtProperty* property, const QDateTime& val);
protected:
	QString valueText(const QtProperty* property) const override;
	void initializeProperty(QtProperty* property) override;
	void uninitializeProperty(QtProperty* property) override;
private:
	QScopedPointer<QtDateTimePropertyManagerPrivate> d_ptr;
	Q_DECLARE_PRIVATE(QtDateTimePropertyManager)
		Q_DISABLE_COPY_MOVE(QtDateTimePropertyManager)
};

#pragma endregion

#pragma region QtKeySequencePropertyManager

class QtKeySequencePropertyManagerPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtKeySequencePropertyManager : public QtAbstractPropertyManager
{
	Q_OBJECT
public:
	QtKeySequencePropertyManager(QObject* parent = 0);
	~QtKeySequencePropertyManager();

	QKeySequence value(const QtProperty* property) const;

public Q_SLOTS:
	void setValue(QtProperty* property, const QKeySequence& val);
Q_SIGNALS:
	void valueChanged(QtProperty* property, const QKeySequence& val);
protected:
	QString valueText(const QtProperty* property) const override;
	void initializeProperty(QtProperty* property) override;
	void uninitializeProperty(QtProperty* property) override;
private:
	QScopedPointer<QtKeySequencePropertyManagerPrivate> d_ptr;
	Q_DECLARE_PRIVATE(QtKeySequencePropertyManager)
		Q_DISABLE_COPY_MOVE(QtKeySequencePropertyManager)
};

#pragma endregion

#pragma region QtCharPropertyManager

class QtCharPropertyManagerPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtCharPropertyManager : public QtAbstractPropertyManager
{
	Q_OBJECT
public:
	QtCharPropertyManager(QObject* parent = 0);
	~QtCharPropertyManager();

	QChar value(const QtProperty* property) const;

public Q_SLOTS:
	void setValue(QtProperty* property, const QChar& val);
Q_SIGNALS:
	void valueChanged(QtProperty* property, const QChar& val);
protected:
	QString valueText(const QtProperty* property) const override;
	void initializeProperty(QtProperty* property) override;
	void uninitializeProperty(QtProperty* property) override;
private:
	QScopedPointer<QtCharPropertyManagerPrivate> d_ptr;
	Q_DECLARE_PRIVATE(QtCharPropertyManager)
		Q_DISABLE_COPY_MOVE(QtCharPropertyManager)
};

#pragma endregion

#pragma region QtLocalePropertyManager

class QtEnumPropertyManager;
class QtLocalePropertyManagerPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtLocalePropertyManager : public QtAbstractPropertyManager
{
	Q_OBJECT
public:
	QtLocalePropertyManager(QObject* parent = 0);
	~QtLocalePropertyManager();

	QtEnumPropertyManager* subEnumPropertyManager() const;

	QLocale value(const QtProperty* property) const;

public Q_SLOTS:
	void setValue(QtProperty* property, const QLocale& val);
Q_SIGNALS:
	void valueChanged(QtProperty* property, const QLocale& val);
protected:
	QString valueText(const QtProperty* property) const override;
	void initializeProperty(QtProperty* property) override;
	void uninitializeProperty(QtProperty* property) override;
private:
	QScopedPointer<QtLocalePropertyManagerPrivate> d_ptr;
	Q_DECLARE_PRIVATE(QtLocalePropertyManager)
		Q_DISABLE_COPY_MOVE(QtLocalePropertyManager)
		Q_PRIVATE_SLOT(d_func(), void slotEnumChanged(QtProperty*, int))
		Q_PRIVATE_SLOT(d_func(), void slotPropertyDestroyed(QtProperty*))
};

#pragma endregion

#pragma region QtPointPropertyManager

class QtPointPropertyManagerPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtPointPropertyManager : public QtAbstractPropertyManager
{
	Q_OBJECT
public:
	QtPointPropertyManager(QObject* parent = 0);
	~QtPointPropertyManager();

	QtIntPropertyManager* subIntPropertyManager() const;

	QPoint value(const QtProperty* property) const;

public Q_SLOTS:
	void setValue(QtProperty* property, const QPoint& val);
Q_SIGNALS:
	void valueChanged(QtProperty* property, const QPoint& val);
protected:
	QString valueText(const QtProperty* property) const override;
	void initializeProperty(QtProperty* property) override;
	void uninitializeProperty(QtProperty* property) override;
private:
	QScopedPointer<QtPointPropertyManagerPrivate> d_ptr;
	Q_DECLARE_PRIVATE(QtPointPropertyManager)
		Q_DISABLE_COPY_MOVE(QtPointPropertyManager)
		Q_PRIVATE_SLOT(d_func(), void slotIntChanged(QtProperty*, int))
		Q_PRIVATE_SLOT(d_func(), void slotPropertyDestroyed(QtProperty*))
};

#pragma endregion

#pragma region QtPointFPropertyManager

class QtPointFPropertyManagerPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtPointFPropertyManager : public QtAbstractPropertyManager
{
	Q_OBJECT
public:
	QtPointFPropertyManager(QObject* parent = 0);
	~QtPointFPropertyManager();

	QtDoublePropertyManager* subDoublePropertyManager() const;

	QPointF value(const QtProperty* property) const;
	int decimals(const QtProperty* property) const;

public Q_SLOTS:
	void setValue(QtProperty* property, const QPointF& val);
	void setDecimals(QtProperty* property, int prec);
Q_SIGNALS:
	void valueChanged(QtProperty* property, const QPointF& val);
	void decimalsChanged(QtProperty* property, int prec);
protected:
	QString valueText(const QtProperty* property) const override;
	void initializeProperty(QtProperty* property) override;
	void uninitializeProperty(QtProperty* property) override;
private:
	QScopedPointer<QtPointFPropertyManagerPrivate> d_ptr;
	Q_DECLARE_PRIVATE(QtPointFPropertyManager)
		Q_DISABLE_COPY_MOVE(QtPointFPropertyManager)
		Q_PRIVATE_SLOT(d_func(), void slotDoubleChanged(QtProperty*, double))
		Q_PRIVATE_SLOT(d_func(), void slotPropertyDestroyed(QtProperty*))
};

#pragma endregion

#pragma region QtSizePropertyManager

class QtSizePropertyManagerPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtSizePropertyManager : public QtAbstractPropertyManager
{
	Q_OBJECT
public:
	QtSizePropertyManager(QObject* parent = 0);
	~QtSizePropertyManager();

	QtIntPropertyManager* subIntPropertyManager() const;

	QSize value(const QtProperty* property) const;
	QSize minimum(const QtProperty* property) const;
	QSize maximum(const QtProperty* property) const;

public Q_SLOTS:
	void setValue(QtProperty* property, const QSize& val);
	void setMinimum(QtProperty* property, const QSize& minVal);
	void setMaximum(QtProperty* property, const QSize& maxVal);
	void setRange(QtProperty* property, const QSize& minVal, const QSize& maxVal);
Q_SIGNALS:
	void valueChanged(QtProperty* property, const QSize& val);
	void rangeChanged(QtProperty* property, const QSize& minVal, const QSize& maxVal);
protected:
	QString valueText(const QtProperty* property) const override;
	void initializeProperty(QtProperty* property) override;
	void uninitializeProperty(QtProperty* property) override;
private:
	QScopedPointer<QtSizePropertyManagerPrivate> d_ptr;
	Q_DECLARE_PRIVATE(QtSizePropertyManager)
		Q_DISABLE_COPY_MOVE(QtSizePropertyManager)
		Q_PRIVATE_SLOT(d_func(), void slotIntChanged(QtProperty*, int))
		Q_PRIVATE_SLOT(d_func(), void slotPropertyDestroyed(QtProperty*))
};

#pragma endregion

#pragma region QtSizeFPropertyManager

class QtSizeFPropertyManagerPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtSizeFPropertyManager : public QtAbstractPropertyManager
{
	Q_OBJECT
public:
	QtSizeFPropertyManager(QObject* parent = 0);
	~QtSizeFPropertyManager();

	QtDoublePropertyManager* subDoublePropertyManager() const;

	QSizeF value(const QtProperty* property) const;
	QSizeF minimum(const QtProperty* property) const;
	QSizeF maximum(const QtProperty* property) const;
	int decimals(const QtProperty* property) const;

public Q_SLOTS:
	void setValue(QtProperty* property, const QSizeF& val);
	void setMinimum(QtProperty* property, const QSizeF& minVal);
	void setMaximum(QtProperty* property, const QSizeF& maxVal);
	void setRange(QtProperty* property, const QSizeF& minVal, const QSizeF& maxVal);
	void setDecimals(QtProperty* property, int prec);
Q_SIGNALS:
	void valueChanged(QtProperty* property, const QSizeF& val);
	void rangeChanged(QtProperty* property, const QSizeF& minVal, const QSizeF& maxVal);
	void decimalsChanged(QtProperty* property, int prec);
protected:
	QString valueText(const QtProperty* property) const override;
	void initializeProperty(QtProperty* property) override;
	void uninitializeProperty(QtProperty* property) override;
private:
	QScopedPointer<QtSizeFPropertyManagerPrivate> d_ptr;
	Q_DECLARE_PRIVATE(QtSizeFPropertyManager)
		Q_DISABLE_COPY_MOVE(QtSizeFPropertyManager)
		Q_PRIVATE_SLOT(d_func(), void slotDoubleChanged(QtProperty*, double))
		Q_PRIVATE_SLOT(d_func(), void slotPropertyDestroyed(QtProperty*))
};

#pragma endregion

#pragma region QtRectPropertyManager

class QtRectPropertyManagerPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtRectPropertyManager : public QtAbstractPropertyManager
{
	Q_OBJECT
public:
	QtRectPropertyManager(QObject* parent = 0);
	~QtRectPropertyManager();

	QtIntPropertyManager* subIntPropertyManager() const;

	QRect value(const QtProperty* property) const;
	QRect constraint(const QtProperty* property) const;

public Q_SLOTS:
	void setValue(QtProperty* property, const QRect& val);
	void setConstraint(QtProperty* property, const QRect& constraint);
Q_SIGNALS:
	void valueChanged(QtProperty* property, const QRect& val);
	void constraintChanged(QtProperty* property, const QRect& constraint);
protected:
	QString valueText(const QtProperty* property) const override;
	void initializeProperty(QtProperty* property) override;
	void uninitializeProperty(QtProperty* property) override;
private:
	QScopedPointer<QtRectPropertyManagerPrivate> d_ptr;
	Q_DECLARE_PRIVATE(QtRectPropertyManager)
		Q_DISABLE_COPY_MOVE(QtRectPropertyManager)
		Q_PRIVATE_SLOT(d_func(), void slotIntChanged(QtProperty*, int))
		Q_PRIVATE_SLOT(d_func(), void slotPropertyDestroyed(QtProperty*))
};

#pragma endregion

#pragma region QtRectFPropertyManager

class QtRectFPropertyManagerPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtRectFPropertyManager : public QtAbstractPropertyManager
{
	Q_OBJECT
public:
	QtRectFPropertyManager(QObject* parent = 0);
	~QtRectFPropertyManager();

	QtDoublePropertyManager* subDoublePropertyManager() const;

	QRectF value(const QtProperty* property) const;
	QRectF constraint(const QtProperty* property) const;
	int decimals(const QtProperty* property) const;

public Q_SLOTS:
	void setValue(QtProperty* property, const QRectF& val);
	void setConstraint(QtProperty* property, const QRectF& constraint);
	void setDecimals(QtProperty* property, int prec);
Q_SIGNALS:
	void valueChanged(QtProperty* property, const QRectF& val);
	void constraintChanged(QtProperty* property, const QRectF& constraint);
	void decimalsChanged(QtProperty* property, int prec);
protected:
	QString valueText(const QtProperty* property) const override;
	void initializeProperty(QtProperty* property) override;
	void uninitializeProperty(QtProperty* property) override;
private:
	QScopedPointer<QtRectFPropertyManagerPrivate> d_ptr;
	Q_DECLARE_PRIVATE(QtRectFPropertyManager)
		Q_DISABLE_COPY_MOVE(QtRectFPropertyManager)
		Q_PRIVATE_SLOT(d_func(), void slotDoubleChanged(QtProperty*, double))
		Q_PRIVATE_SLOT(d_func(), void slotPropertyDestroyed(QtProperty*))
};

#pragma endregion

#pragma region QtEnumPropertyManager

class QtEnumPropertyManagerPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtEnumPropertyManager : public QtAbstractPropertyManager
{
	Q_OBJECT
public:
	QtEnumPropertyManager(QObject* parent = 0);
	~QtEnumPropertyManager();

	int value(const QtProperty* property) const;
	QStringList enumNames(const QtProperty* property) const;
	QMap<int, QIcon> enumIcons(const QtProperty* property) const;

public Q_SLOTS:
	void setValue(QtProperty* property, int val);
	void setEnumNames(QtProperty* property, const QStringList& names);
	void setEnumIcons(QtProperty* property, const QMap<int, QIcon>& icons);
Q_SIGNALS:
	void valueChanged(QtProperty* property, int val);
	void enumNamesChanged(QtProperty* property, const QStringList& names);
	void enumIconsChanged(QtProperty* property, const QMap<int, QIcon>& icons);
protected:
	QString valueText(const QtProperty* property) const override;
	QIcon valueIcon(const QtProperty* property) const override;
	void initializeProperty(QtProperty* property) override;
	void uninitializeProperty(QtProperty* property) override;
private:
	QScopedPointer<QtEnumPropertyManagerPrivate> d_ptr;
	Q_DECLARE_PRIVATE(QtEnumPropertyManager)
		Q_DISABLE_COPY_MOVE(QtEnumPropertyManager)
};

#pragma endregion

#pragma region QtFlagPropertyManager

class QtFlagPropertyManagerPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtFlagPropertyManager : public QtAbstractPropertyManager
{
	Q_OBJECT
public:
	QtFlagPropertyManager(QObject* parent = 0);
	~QtFlagPropertyManager();

	QtBoolPropertyManager* subBoolPropertyManager() const;

	int value(const QtProperty* property) const;
	QStringList flagNames(const QtProperty* property) const;

public Q_SLOTS:
	void setValue(QtProperty* property, int val);
	void setFlagNames(QtProperty* property, const QStringList& names);
Q_SIGNALS:
	void valueChanged(QtProperty* property, int val);
	void flagNamesChanged(QtProperty* property, const QStringList& names);
protected:
	QString valueText(const QtProperty* property) const override;
	void initializeProperty(QtProperty* property) override;
	void uninitializeProperty(QtProperty* property) override;
private:
	QScopedPointer<QtFlagPropertyManagerPrivate> d_ptr;
	Q_DECLARE_PRIVATE(QtFlagPropertyManager)
		Q_DISABLE_COPY_MOVE(QtFlagPropertyManager)
		Q_PRIVATE_SLOT(d_func(), void slotBoolChanged(QtProperty*, bool))
		Q_PRIVATE_SLOT(d_func(), void slotPropertyDestroyed(QtProperty*))
};

#pragma endregion

#pragma region QtSizePolicyPropertyManager

class QtSizePolicyPropertyManagerPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtSizePolicyPropertyManager : public QtAbstractPropertyManager
{
	Q_OBJECT
public:
	QtSizePolicyPropertyManager(QObject* parent = 0);
	~QtSizePolicyPropertyManager();

	QtIntPropertyManager* subIntPropertyManager() const;
	QtEnumPropertyManager* subEnumPropertyManager() const;

	QSizePolicy value(const QtProperty* property) const;

public Q_SLOTS:
	void setValue(QtProperty* property, const QSizePolicy& val);
Q_SIGNALS:
	void valueChanged(QtProperty* property, const QSizePolicy& val);
protected:
	QString valueText(const QtProperty* property) const override;
	void initializeProperty(QtProperty* property) override;
	void uninitializeProperty(QtProperty* property) override;
private:
	QScopedPointer<QtSizePolicyPropertyManagerPrivate> d_ptr;
	Q_DECLARE_PRIVATE(QtSizePolicyPropertyManager)
		Q_DISABLE_COPY_MOVE(QtSizePolicyPropertyManager)
		Q_PRIVATE_SLOT(d_func(), void slotIntChanged(QtProperty*, int))
		Q_PRIVATE_SLOT(d_func(), void slotEnumChanged(QtProperty*, int))
		Q_PRIVATE_SLOT(d_func(), void slotPropertyDestroyed(QtProperty*))
};

#pragma endregion

#pragma region QtFontPropertyManager

class QtFontPropertyManagerPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtFontPropertyManager : public QtAbstractPropertyManager
{
	Q_OBJECT
public:
	QtFontPropertyManager(QObject* parent = 0);
	~QtFontPropertyManager();

	QtIntPropertyManager* subIntPropertyManager() const;
	QtEnumPropertyManager* subEnumPropertyManager() const;
	QtBoolPropertyManager* subBoolPropertyManager() const;

	QFont value(const QtProperty* property) const;

public Q_SLOTS:
	void setValue(QtProperty* property, const QFont& val);
Q_SIGNALS:
	void valueChanged(QtProperty* property, const QFont& val);
protected:
	QString valueText(const QtProperty* property) const override;
	QIcon valueIcon(const QtProperty* property) const override;
	void initializeProperty(QtProperty* property) override;
	void uninitializeProperty(QtProperty* property) override;
private:
	QScopedPointer<QtFontPropertyManagerPrivate> d_ptr;
	Q_DECLARE_PRIVATE(QtFontPropertyManager)
		Q_DISABLE_COPY_MOVE(QtFontPropertyManager)
		Q_PRIVATE_SLOT(d_func(), void slotIntChanged(QtProperty*, int))
		Q_PRIVATE_SLOT(d_func(), void slotEnumChanged(QtProperty*, int))
		Q_PRIVATE_SLOT(d_func(), void slotBoolChanged(QtProperty*, bool))
		Q_PRIVATE_SLOT(d_func(), void slotPropertyDestroyed(QtProperty*))
		Q_PRIVATE_SLOT(d_func(), void slotFontDatabaseChanged())
		Q_PRIVATE_SLOT(d_func(), void slotFontDatabaseDelayedChange())
};

#pragma endregion

#pragma region QtColorPropertyManager

class QtColorPropertyManagerPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtColorPropertyManager : public QtAbstractPropertyManager
{
	Q_OBJECT
public:
	QtColorPropertyManager(QObject* parent = 0);
	~QtColorPropertyManager();

	QtIntPropertyManager* subIntPropertyManager() const;

	QColor value(const QtProperty* property) const;

public Q_SLOTS:
	void setValue(QtProperty* property, const QColor& val);
Q_SIGNALS:
	void valueChanged(QtProperty* property, const QColor& val);
protected:
	QString valueText(const QtProperty* property) const override;
	QIcon valueIcon(const QtProperty* property) const override;
	void initializeProperty(QtProperty* property) override;
	void uninitializeProperty(QtProperty* property) override;
private:
	QScopedPointer<QtColorPropertyManagerPrivate> d_ptr;
	Q_DECLARE_PRIVATE(QtColorPropertyManager)
		Q_DISABLE_COPY_MOVE(QtColorPropertyManager)
		Q_PRIVATE_SLOT(d_func(), void slotIntChanged(QtProperty*, int))
		Q_PRIVATE_SLOT(d_func(), void slotPropertyDestroyed(QtProperty*))
};

#pragma endregion

#pragma region QtCursorPropertyManager

class QtCursorPropertyManagerPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtCursorPropertyManager : public QtAbstractPropertyManager
{
	Q_OBJECT
public:
	QtCursorPropertyManager(QObject* parent = 0);
	~QtCursorPropertyManager();

#ifndef QT_NO_CURSOR
	QCursor value(const QtProperty* property) const;
#endif

public Q_SLOTS:
	void setValue(QtProperty* property, const QCursor& val);
Q_SIGNALS:
	void valueChanged(QtProperty* property, const QCursor& val);
protected:
	QString valueText(const QtProperty* property) const override;
	QIcon valueIcon(const QtProperty* property) const override;
	void initializeProperty(QtProperty* property) override;
	void uninitializeProperty(QtProperty* property) override;
private:
	QScopedPointer<QtCursorPropertyManagerPrivate> d_ptr;
	Q_DECLARE_PRIVATE(QtCursorPropertyManager)
		Q_DISABLE_COPY_MOVE(QtCursorPropertyManager)
};

#pragma endregion

QT_END_NAMESPACE

#endif
