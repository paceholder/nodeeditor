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

#include "qtvariantproperty.h"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtCore/QDate>
#include <QtCore/QLocale>
#include <QtCore/QRegularExpression>

#if defined(Q_CC_MSVC)
#    pragma warning(disable: 4786) /* MS VS 6: truncating debug info after 255 characters */
#endif

QT_BEGIN_NAMESPACE

class QtEnumPropertyType
{
};


class QtFlagPropertyType
{
};


class QtGroupPropertyType
{
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QtEnumPropertyType)
Q_DECLARE_METATYPE(QtFlagPropertyType)
Q_DECLARE_METATYPE(QtGroupPropertyType)

QT_BEGIN_NAMESPACE

/*!
    Returns the type id for an enum property.

    Note that the property's value type can be retrieved using the
    valueType() function (which is QMetaType::Int for the enum property
    type).

    \sa propertyType(), valueType()
*/
int QtVariantPropertyManager::enumTypeId()
{
    return qMetaTypeId<QtEnumPropertyType>();
}

/*!
    Returns the type id for a flag property.

    Note that the property's value type can be retrieved using the
    valueType() function (which is QMetaType::Int for the flag property
    type).

    \sa propertyType(), valueType()
*/
int QtVariantPropertyManager::flagTypeId()
{
    return qMetaTypeId<QtFlagPropertyType>();
}

/*!
    Returns the type id for a group property.

    Note that the property's value type can be retrieved using the
    valueType() function (which is QMetaType::UnknownType for the group
    property type, since it doesn't provide any value).

    \sa propertyType(), valueType()
*/
int QtVariantPropertyManager::groupTypeId()
{
    return qMetaTypeId<QtGroupPropertyType>();
}

/*!
    Returns the type id for a icon map attribute.

    Note that the property's attribute type can be retrieved using the
    attributeType() function.

    \sa attributeType(), QtEnumPropertyManager::enumIcons()
*/
int QtVariantPropertyManager::iconMapTypeId()
{
    return qMetaTypeId<QtIconMap>();
}

typedef QMap<const QtProperty *, QtProperty *> PropertyMap;
Q_GLOBAL_STATIC(PropertyMap, propertyToWrappedProperty)

static QtProperty *wrappedProperty(QtProperty *property)
{
    return propertyToWrappedProperty()->value(property, 0);
}

class QtVariantPropertyPrivate
{
public:
    QtVariantPropertyPrivate(QtVariantPropertyManager *m) : manager(m) {}

    QtVariantPropertyManager *manager;
};

/*!
    \class QtVariantProperty
    \internal
    \inmodule QtDesigner
    \since 4.4

    \brief The QtVariantProperty class is a convenience class handling
    QVariant based properties.

    QtVariantProperty provides additional API: A property's type,
    value type, attribute values and current value can easily be
    retrieved using the propertyType(), valueType(), attributeValue()
    and value() functions respectively. In addition, the attribute
    values and the current value can be set using the corresponding
    setValue() and setAttribute() functions.

    For example, instead of writing:

    \snippet doc/src/snippets/code/tools_shared_qtpropertybrowser_qtvariantproperty.cpp 0

    you can write:

    \snippet doc/src/snippets/code/tools_shared_qtpropertybrowser_qtvariantproperty.cpp 1

    QtVariantProperty instances can only be created by the
    QtVariantPropertyManager class.

    \sa QtProperty, QtVariantPropertyManager, QtVariantEditorFactory
*/

/*!
    Creates a variant property using the given \a manager.

    Do not use this constructor to create variant property instances;
    use the QtVariantPropertyManager::addProperty() function
    instead.  This constructor is used internally by the
    QtVariantPropertyManager::createProperty() function.

    \sa QtVariantPropertyManager
*/
QtVariantProperty::QtVariantProperty(QtVariantPropertyManager *manager)
    : QtProperty(manager), d_ptr(new QtVariantPropertyPrivate(manager))
{
}

/*!
    Destroys this property.

    \sa QtProperty::~QtProperty()
*/
QtVariantProperty::~QtVariantProperty()
{
}

/*!
    Returns the property's current value.

    \sa valueType(), setValue()
*/
QVariant QtVariantProperty::value() const
{
    return d_ptr->manager->value(this);
}

/*!
    Returns this property's value for the specified \a attribute.

    QtVariantPropertyManager provides a couple of related functions:
    \l{QtVariantPropertyManager::attributes()}{attributes()} and
    \l{QtVariantPropertyManager::attributeType()}{attributeType()}.

    \sa setAttribute()
*/
QVariant QtVariantProperty::attributeValue(const QString &attribute) const
{
    return d_ptr->manager->attributeValue(this, attribute);
}

/*!
    Returns the type of this property's value.

    \sa propertyType()
*/
int QtVariantProperty::valueType() const
{
    return d_ptr->manager->valueType(this);
}

/*!
    Returns this property's type.

    QtVariantPropertyManager provides several related functions:
    \l{QtVariantPropertyManager::enumTypeId()}{enumTypeId()},
    \l{QtVariantPropertyManager::flagTypeId()}{flagTypeId()} and
    \l{QtVariantPropertyManager::groupTypeId()}{groupTypeId()}.

    \sa valueType()
*/
int QtVariantProperty::propertyType() const
{
    return d_ptr->manager->propertyType(this);
}

/*!
    Sets the value of this property to \a value.

    The specified \a value must be of the type returned by
    valueType(), or of a type that can be converted to valueType()
    using the QVariant::canConvert() function; otherwise this function
    does nothing.

    \sa value()
*/
void QtVariantProperty::setValue(const QVariant &value)
{
    d_ptr->manager->setValue(this, value);
}

/*!
    Sets the \a attribute of property to \a value.

    QtVariantPropertyManager provides the related
    \l{QtVariantPropertyManager::setAttribute()}{setAttribute()}
    function.

    \sa attributeValue()
*/
void QtVariantProperty::setAttribute(const QString &attribute, const QVariant &value)
{
    d_ptr->manager->setAttribute(this, attribute, value);
}

class QtVariantPropertyManagerPrivate
{
    QtVariantPropertyManager *q_ptr;
    Q_DECLARE_PUBLIC(QtVariantPropertyManager)
public:
    QtVariantPropertyManagerPrivate();

    bool m_creatingProperty;
    bool m_creatingSubProperties;
    bool m_destroyingSubProperties;
    int m_propertyType;

    void slotValueChanged(QtProperty *property, int val);
    void slotRangeChanged(QtProperty *property, int min, int max);
    void slotSingleStepChanged(QtProperty *property, int step);
    void slotValueChanged(QtProperty *property, double val);
    void slotRangeChanged(QtProperty *property, double min, double max);
    void slotSingleStepChanged(QtProperty *property, double step);
    void slotDecimalsChanged(QtProperty *property, int prec);
    void slotValueChanged(QtProperty *property, bool val);
    void slotValueChanged(QtProperty *property, const QString &val);
    void slotRegExpChanged(QtProperty *property, const QRegularExpression &regExp);
    void slotValueChanged(QtProperty *property, QDate val);
    void slotRangeChanged(QtProperty *property, QDate min, QDate max);
    void slotValueChanged(QtProperty *property, QTime val);
    void slotValueChanged(QtProperty *property, const QDateTime &val);
    void slotValueChanged(QtProperty *property, const QKeySequence &val);
    void slotValueChanged(QtProperty *property, const QChar &val);
    void slotValueChanged(QtProperty *property, const QLocale &val);
    void slotValueChanged(QtProperty *property, const QPoint &val);
    void slotValueChanged(QtProperty *property, const QPointF &val);
    void slotValueChanged(QtProperty *property, const QSize &val);
    void slotRangeChanged(QtProperty *property, const QSize &min, const QSize &max);
    void slotValueChanged(QtProperty *property, const QSizeF &val);
    void slotRangeChanged(QtProperty *property, const QSizeF &min, const QSizeF &max);
    void slotValueChanged(QtProperty *property, const QRect &val);
    void slotConstraintChanged(QtProperty *property, const QRect &val);
    void slotValueChanged(QtProperty *property, const QRectF &val);
    void slotConstraintChanged(QtProperty *property, const QRectF &val);
    void slotValueChanged(QtProperty *property, const QColor &val);
    void slotEnumChanged(QtProperty *property, int val);
    void slotEnumNamesChanged(QtProperty *property, const QStringList &enumNames);
    void slotEnumIconsChanged(QtProperty *property, const QMap<int, QIcon> &enumIcons);
    void slotValueChanged(QtProperty *property, const QSizePolicy &val);
    void slotValueChanged(QtProperty *property, const QFont &val);
    void slotValueChanged(QtProperty *property, const QCursor &val);
    void slotFlagChanged(QtProperty *property, int val);
    void slotFlagNamesChanged(QtProperty *property, const QStringList &flagNames);
    void slotPropertyInserted(QtProperty *property, QtProperty *parent, QtProperty *after);
    void slotPropertyRemoved(QtProperty *property, QtProperty *parent);

    void valueChanged(QtProperty *property, const QVariant &val);

    int internalPropertyToType(QtProperty *property) const;
    QtVariantProperty *createSubProperty(QtVariantProperty *parent, QtVariantProperty *after,
            QtProperty *internal);
    void removeSubProperty(QtVariantProperty *property);

    QMap<int, QtAbstractPropertyManager *> m_typeToPropertyManager;
    QMap<int, QMap<QString, int> > m_typeToAttributeToAttributeType;

    QMap<const QtProperty *, QPair<QtVariantProperty *, int> > m_propertyToType;

    QMap<int, int> m_typeToValueType;


    QMap<QtProperty *, QtVariantProperty *> m_internalToProperty;

    const QString m_constraintAttribute;
    const QString m_singleStepAttribute;
    const QString m_decimalsAttribute;
    const QString m_enumIconsAttribute;
    const QString m_enumNamesAttribute;
    const QString m_flagNamesAttribute;
    const QString m_maximumAttribute;
    const QString m_minimumAttribute;
    const QString m_regExpAttribute;
};

QtVariantPropertyManagerPrivate::QtVariantPropertyManagerPrivate() :
    m_constraintAttribute(QLatin1String("constraint")),
    m_singleStepAttribute(QLatin1String("singleStep")),
    m_decimalsAttribute(QLatin1String("decimals")),
    m_enumIconsAttribute(QLatin1String("enumIcons")),
    m_enumNamesAttribute(QLatin1String("enumNames")),
    m_flagNamesAttribute(QLatin1String("flagNames")),
    m_maximumAttribute(QLatin1String("maximum")),
    m_minimumAttribute(QLatin1String("minimum")),
    m_regExpAttribute(QLatin1String("regExp"))
{
}

int QtVariantPropertyManagerPrivate::internalPropertyToType(QtProperty *property) const
{
    int type = 0;
    QtAbstractPropertyManager *internPropertyManager = property->propertyManager();
    if (qobject_cast<QtIntPropertyManager *>(internPropertyManager))
        type = QMetaType::Int;
    else if (qobject_cast<QtEnumPropertyManager *>(internPropertyManager))
        type = QtVariantPropertyManager::enumTypeId();
    else if (qobject_cast<QtBoolPropertyManager *>(internPropertyManager))
        type = QMetaType::Bool;
    else if (qobject_cast<QtDoublePropertyManager *>(internPropertyManager))
        type = QMetaType::Double;
    return type;
}

QtVariantProperty *QtVariantPropertyManagerPrivate::createSubProperty(QtVariantProperty *parent,
            QtVariantProperty *after, QtProperty *internal)
{
    int type = internalPropertyToType(internal);
    if (!type)
        return 0;

    bool wasCreatingSubProperties = m_creatingSubProperties;
    m_creatingSubProperties = true;

    QtVariantProperty *varChild = q_ptr->addProperty(type, internal->propertyName());

    m_creatingSubProperties = wasCreatingSubProperties;

    varChild->setPropertyName(internal->propertyName());
    varChild->setToolTip(internal->toolTip());
    varChild->setStatusTip(internal->statusTip());
    varChild->setWhatsThis(internal->whatsThis());

    parent->insertSubProperty(varChild, after);

    m_internalToProperty[internal] = varChild;
    propertyToWrappedProperty()->insert(varChild, internal);
    return varChild;
}

void QtVariantPropertyManagerPrivate::removeSubProperty(QtVariantProperty *property)
{
    QtProperty *internChild = wrappedProperty(property);
    bool wasDestroyingSubProperties = m_destroyingSubProperties;
    m_destroyingSubProperties = true;
    delete property;
    m_destroyingSubProperties = wasDestroyingSubProperties;
    m_internalToProperty.remove(internChild);
    propertyToWrappedProperty()->remove(property);
}

void QtVariantPropertyManagerPrivate::slotPropertyInserted(QtProperty *property,
            QtProperty *parent, QtProperty *after)
{
    if (m_creatingProperty)
        return;

    QtVariantProperty *varParent = m_internalToProperty.value(parent, 0);
    if (!varParent)
        return;

    QtVariantProperty *varAfter = 0;
    if (after) {
        varAfter = m_internalToProperty.value(after, 0);
        if (!varAfter)
            return;
    }

    createSubProperty(varParent, varAfter, property);
}

void QtVariantPropertyManagerPrivate::slotPropertyRemoved(QtProperty *property, QtProperty *parent)
{
    Q_UNUSED(parent);

    QtVariantProperty *varProperty = m_internalToProperty.value(property, 0);
    if (!varProperty)
        return;

    removeSubProperty(varProperty);
}

void QtVariantPropertyManagerPrivate::valueChanged(QtProperty *property, const QVariant &val)
{
    QtVariantProperty *varProp = m_internalToProperty.value(property, 0);
    if (!varProp)
        return;
    emit q_ptr->valueChanged(varProp, val);
    emit q_ptr->propertyChanged(varProp);
}

void QtVariantPropertyManagerPrivate::slotValueChanged(QtProperty *property, int val)
{
    valueChanged(property, QVariant(val));
}

void QtVariantPropertyManagerPrivate::slotRangeChanged(QtProperty *property, int min, int max)
{
    if (QtVariantProperty *varProp = m_internalToProperty.value(property, 0)) {
        emit q_ptr->attributeChanged(varProp, m_minimumAttribute, QVariant(min));
        emit q_ptr->attributeChanged(varProp, m_maximumAttribute, QVariant(max));
    }
}

void QtVariantPropertyManagerPrivate::slotSingleStepChanged(QtProperty *property, int step)
{
    if (QtVariantProperty *varProp = m_internalToProperty.value(property, 0))
        emit q_ptr->attributeChanged(varProp, m_singleStepAttribute, QVariant(step));
}

void QtVariantPropertyManagerPrivate::slotValueChanged(QtProperty *property, double val)
{
    valueChanged(property, QVariant(val));
}

void QtVariantPropertyManagerPrivate::slotRangeChanged(QtProperty *property, double min, double max)
{
    if (QtVariantProperty *varProp = m_internalToProperty.value(property, 0)) {
        emit q_ptr->attributeChanged(varProp, m_minimumAttribute, QVariant(min));
        emit q_ptr->attributeChanged(varProp, m_maximumAttribute, QVariant(max));
    }
}

void QtVariantPropertyManagerPrivate::slotSingleStepChanged(QtProperty *property, double step)
{
    if (QtVariantProperty *varProp = m_internalToProperty.value(property, 0))
        emit q_ptr->attributeChanged(varProp, m_singleStepAttribute, QVariant(step));
}

void QtVariantPropertyManagerPrivate::slotDecimalsChanged(QtProperty *property, int prec)
{
    if (QtVariantProperty *varProp = m_internalToProperty.value(property, 0))
        emit q_ptr->attributeChanged(varProp, m_decimalsAttribute, QVariant(prec));
}

void QtVariantPropertyManagerPrivate::slotValueChanged(QtProperty *property, bool val)
{
    valueChanged(property, QVariant(val));
}

void QtVariantPropertyManagerPrivate::slotValueChanged(QtProperty *property, const QString &val)
{
    valueChanged(property, QVariant(val));
}

void QtVariantPropertyManagerPrivate::slotRegExpChanged(QtProperty *property, const QRegularExpression &regExp)
{
    if (QtVariantProperty *varProp = m_internalToProperty.value(property, 0))
        emit q_ptr->attributeChanged(varProp, m_regExpAttribute, QVariant(regExp));
}

void QtVariantPropertyManagerPrivate::slotValueChanged(QtProperty *property, QDate val)
{
    valueChanged(property, QVariant(val));
}

void QtVariantPropertyManagerPrivate::slotRangeChanged(QtProperty *property, QDate min, QDate max)
{
    if (QtVariantProperty *varProp = m_internalToProperty.value(property, 0)) {
        emit q_ptr->attributeChanged(varProp, m_minimumAttribute, QVariant(min));
        emit q_ptr->attributeChanged(varProp, m_maximumAttribute, QVariant(max));
    }
}

void QtVariantPropertyManagerPrivate::slotValueChanged(QtProperty *property, QTime val)
{
    valueChanged(property, QVariant(val));
}

void QtVariantPropertyManagerPrivate::slotValueChanged(QtProperty *property, const QDateTime &val)
{
    valueChanged(property, QVariant(val));
}

void QtVariantPropertyManagerPrivate::slotValueChanged(QtProperty *property, const QKeySequence &val)
{
    QVariant v;
    v.setValue(val);
    valueChanged(property, v);
}

void QtVariantPropertyManagerPrivate::slotValueChanged(QtProperty *property, const QChar &val)
{
    valueChanged(property, QVariant(val));
}

void QtVariantPropertyManagerPrivate::slotValueChanged(QtProperty *property, const QLocale &val)
{
    valueChanged(property, QVariant(val));
}

void QtVariantPropertyManagerPrivate::slotValueChanged(QtProperty *property, const QPoint &val)
{
    valueChanged(property, QVariant(val));
}

void QtVariantPropertyManagerPrivate::slotValueChanged(QtProperty *property, const QPointF &val)
{
    valueChanged(property, QVariant(val));
}

void QtVariantPropertyManagerPrivate::slotValueChanged(QtProperty *property, const QSize &val)
{
    valueChanged(property, QVariant(val));
}

void QtVariantPropertyManagerPrivate::slotRangeChanged(QtProperty *property, const QSize &min, const QSize &max)
{
    if (QtVariantProperty *varProp = m_internalToProperty.value(property, 0)) {
        emit q_ptr->attributeChanged(varProp, m_minimumAttribute, QVariant(min));
        emit q_ptr->attributeChanged(varProp, m_maximumAttribute, QVariant(max));
    }
}

void QtVariantPropertyManagerPrivate::slotValueChanged(QtProperty *property, const QSizeF &val)
{
    valueChanged(property, QVariant(val));
}

void QtVariantPropertyManagerPrivate::slotRangeChanged(QtProperty *property, const QSizeF &min, const QSizeF &max)
{
    if (QtVariantProperty *varProp = m_internalToProperty.value(property, 0)) {
        emit q_ptr->attributeChanged(varProp, m_minimumAttribute, QVariant(min));
        emit q_ptr->attributeChanged(varProp, m_maximumAttribute, QVariant(max));
    }
}

void QtVariantPropertyManagerPrivate::slotValueChanged(QtProperty *property, const QRect &val)
{
    valueChanged(property, QVariant(val));
}

void QtVariantPropertyManagerPrivate::slotConstraintChanged(QtProperty *property, const QRect &constraint)
{
    if (QtVariantProperty *varProp = m_internalToProperty.value(property, 0))
        emit q_ptr->attributeChanged(varProp, m_constraintAttribute, QVariant(constraint));
}

void QtVariantPropertyManagerPrivate::slotValueChanged(QtProperty *property, const QRectF &val)
{
    valueChanged(property, QVariant(val));
}

void QtVariantPropertyManagerPrivate::slotConstraintChanged(QtProperty *property, const QRectF &constraint)
{
    if (QtVariantProperty *varProp = m_internalToProperty.value(property, 0))
        emit q_ptr->attributeChanged(varProp, m_constraintAttribute, QVariant(constraint));
}

void QtVariantPropertyManagerPrivate::slotValueChanged(QtProperty *property, const QColor &val)
{
    valueChanged(property, QVariant(val));
}

void QtVariantPropertyManagerPrivate::slotEnumNamesChanged(QtProperty *property, const QStringList &enumNames)
{
    if (QtVariantProperty *varProp = m_internalToProperty.value(property, 0))
        emit q_ptr->attributeChanged(varProp, m_enumNamesAttribute, QVariant(enumNames));
}

void QtVariantPropertyManagerPrivate::slotEnumIconsChanged(QtProperty *property, const QMap<int, QIcon> &enumIcons)
{
    if (QtVariantProperty *varProp = m_internalToProperty.value(property, 0)) {
        QVariant v;
        v.setValue(enumIcons);
        emit q_ptr->attributeChanged(varProp, m_enumIconsAttribute, v);
    }
}

void QtVariantPropertyManagerPrivate::slotValueChanged(QtProperty *property, const QSizePolicy &val)
{
    valueChanged(property, QVariant(val));
}

void QtVariantPropertyManagerPrivate::slotValueChanged(QtProperty *property, const QFont &val)
{
    valueChanged(property, QVariant(val));
}

void QtVariantPropertyManagerPrivate::slotValueChanged(QtProperty *property, const QCursor &val)
{
#ifndef QT_NO_CURSOR
    valueChanged(property, QVariant(val));
#endif
}

void QtVariantPropertyManagerPrivate::slotFlagNamesChanged(QtProperty *property, const QStringList &flagNames)
{
    if (QtVariantProperty *varProp = m_internalToProperty.value(property, 0))
        emit q_ptr->attributeChanged(varProp, m_flagNamesAttribute, QVariant(flagNames));
}

/*!
    \class QtVariantPropertyManager
    \internal
    \inmodule QtDesigner
    \since 4.4

    \brief The QtVariantPropertyManager class provides and manages QVariant based properties.

    QtVariantPropertyManager provides the addProperty() function which
    creates QtVariantProperty objects. The QtVariantProperty class is
    a convenience class handling QVariant based properties inheriting
    QtProperty. A QtProperty object created by a
    QtVariantPropertyManager instance can be converted into a
    QtVariantProperty object using the variantProperty() function.

    The property's value can be retrieved using the value(), and set
    using the setValue() slot. In addition the property's type, and
    the type of its value, can be retrieved using the propertyType()
    and valueType() functions respectively.

    A property's type is a QMetaType::QType enumerator value, and
    usually a property's type is the same as its value type. But for
    some properties the types differ, for example for enums, flags and
    group types in which case QtVariantPropertyManager provides the
    enumTypeId(), flagTypeId() and groupTypeId() functions,
    respectively, to identify their property type (the value types are
    QMetaType::Int for the enum and flag types, and QMetaType::UnknownType
    for the group type).

    Use the isPropertyTypeSupported() function to check if a particular
    property type is supported. The currently supported property types
    are:

    \table
    \header
        \li Property Type
        \li Property Type Id
    \row
        \li int
        \li QMetaType::Int
    \row
        \li double
        \li QMetaType::Double
    \row
        \li bool
        \li QMetaType::Bool
    \row
        \li QString
        \li QMetaType::QString
    \row
        \li QDate
        \li QMetaType::QDate
    \row
        \li QTime
        \li QMetaType::QTime
    \row
        \li QDateTime
        \li QMetaType::QDateTime
    \row
        \li QKeySequence
        \li QMetaType::QKeySequence
    \row
        \li QChar
        \li QMetaType::QChar
    \row
        \li QLocale
        \li QMetaType::QLocale
    \row
        \li QPoint
        \li QMetaType::QPoint
    \row
        \li QPointF
        \li QMetaType::QPointF
    \row
        \li QSize
        \li QMetaType::QSize
    \row
        \li QSizeF
        \li QMetaType::QSizeF
    \row
        \li QRect
        \li QMetaType::QRect
    \row
        \li QRectF
        \li QMetaType::QRectF
    \row
        \li QColor
        \li QMetaType::QColor
    \row
        \li QSizePolicy
        \li QMetaType::QSizePolicy
    \row
        \li QFont
        \li QMetaType::QFont
    \row
        \li QCursor
        \li QMetaType::QCursor
    \row
        \li enum
        \li enumTypeId()
    \row
        \li flag
        \li flagTypeId()
    \row
        \li group
        \li groupTypeId()
    \endtable

    Each property type can provide additional attributes,
    e.g. QMetaType::Int and QMetaType::Double provides minimum and
    maximum values. The currently supported attributes are:

    \table
    \header
        \li Property Type
        \li Attribute Name
        \li Attribute Type
    \row
        \li \c int
        \li minimum
        \li QMetaType::Int
    \row
        \li
        \li maximum
        \li QMetaType::Int
    \row
        \li
        \li singleStep
        \li QMetaType::Int
    \row
        \li \c double
        \li minimum
        \li QMetaType::Double
    \row
        \li
        \li maximum
        \li QMetaType::Double
    \row
        \li
        \li singleStep
        \li QMetaType::Double
    \row
        \li
        \li decimals
        \li QMetaType::Int
    \row
        \li QString
        \li regExp
        \li QMetaType::QRegExp
    \row
        \li QDate
        \li minimum
        \li QMetaType::QDate
    \row
        \li
        \li maximum
        \li QMetaType::QDate
    \row
        \li QPointF
        \li decimals
        \li QMetaType::Int
    \row
        \li QSize
        \li minimum
        \li QMetaType::QSize
    \row
        \li
        \li maximum
        \li QMetaType::QSize
    \row
        \li QSizeF
        \li minimum
        \li QMetaType::QSizeF
    \row
        \li
        \li maximum
        \li QMetaType::QSizeF
    \row
        \li
        \li decimals
        \li QMetaType::Int
    \row
        \li QRect
        \li constraint
        \li QMetaType::QRect
    \row
        \li QRectF
        \li constraint
        \li QMetaType::QRectF
    \row
        \li
        \li decimals
        \li QMetaType::Int
    \row
        \li \c enum
        \li enumNames
        \li QMetaType::QStringList
    \row
        \li
        \li enumIcons
        \li iconMapTypeId()
    \row
        \li \c flag
        \li flagNames
        \li QMetaType::QStringList
    \endtable

    The attributes for a given property type can be retrieved using
    the attributes() function. Each attribute has a value type which
    can be retrieved using the attributeType() function, and a value
    accessible through the attributeValue() function. In addition, the
    value can be set using the setAttribute() slot.

    QtVariantManager also provides the valueChanged() signal which is
    emitted whenever a property created by this manager change, and
    the attributeChanged() signal which is emitted whenever an
    attribute of such a property changes.

    \sa QtVariantProperty, QtVariantEditorFactory
*/

/*!
    \fn void QtVariantPropertyManager::valueChanged(QtProperty *property, const QVariant &value)

    This signal is emitted whenever a property created by this manager
    changes its value, passing a pointer to the \a property and the
    new \a value as parameters.

    \sa setValue()
*/

/*!
    \fn void QtVariantPropertyManager::attributeChanged(QtProperty *property,
                const QString &attribute, const QVariant &value)

    This signal is emitted whenever an attribute of a property created
    by this manager changes its value, passing a pointer to the \a
    property, the \a attribute and the new \a value as parameters.

    \sa setAttribute()
*/

/*!
    Creates a manager with the given \a parent.
*/
QtVariantPropertyManager::QtVariantPropertyManager(QObject *parent)
    : QtAbstractPropertyManager(parent), d_ptr(new QtVariantPropertyManagerPrivate)
{
    d_ptr->q_ptr = this;

    d_ptr->m_creatingProperty = false;
    d_ptr->m_creatingSubProperties = false;
    d_ptr->m_destroyingSubProperties = false;
    d_ptr->m_propertyType = 0;

    // IntPropertyManager
    QtIntPropertyManager *intPropertyManager = new QtIntPropertyManager(this);
    d_ptr->m_typeToPropertyManager[QMetaType::Int] = intPropertyManager;
    d_ptr->m_typeToAttributeToAttributeType[QMetaType::Int][d_ptr->m_minimumAttribute] = QMetaType::Int;
    d_ptr->m_typeToAttributeToAttributeType[QMetaType::Int][d_ptr->m_maximumAttribute] = QMetaType::Int;
    d_ptr->m_typeToAttributeToAttributeType[QMetaType::Int][d_ptr->m_singleStepAttribute] = QMetaType::Int;
    d_ptr->m_typeToValueType[QMetaType::Int] = QMetaType::Int;
    connect(intPropertyManager, SIGNAL(valueChanged(QtProperty*,int)),
                this, SLOT(slotValueChanged(QtProperty*,int)));
    connect(intPropertyManager, SIGNAL(rangeChanged(QtProperty*,int,int)),
                this, SLOT(slotRangeChanged(QtProperty*,int,int)));
    connect(intPropertyManager, SIGNAL(singleStepChanged(QtProperty*,int)),
                this, SLOT(slotSingleStepChanged(QtProperty*,int)));
    // DoublePropertyManager
    QtDoublePropertyManager *doublePropertyManager = new QtDoublePropertyManager(this);
    d_ptr->m_typeToPropertyManager[QMetaType::Double] = doublePropertyManager;
    d_ptr->m_typeToAttributeToAttributeType[QMetaType::Double][d_ptr->m_minimumAttribute] =
            QMetaType::Double;
    d_ptr->m_typeToAttributeToAttributeType[QMetaType::Double][d_ptr->m_maximumAttribute] =
            QMetaType::Double;
    d_ptr->m_typeToAttributeToAttributeType[QMetaType::Double][d_ptr->m_singleStepAttribute] =
            QMetaType::Double;
    d_ptr->m_typeToAttributeToAttributeType[QMetaType::Double][d_ptr->m_decimalsAttribute] =
            QMetaType::Int;
    d_ptr->m_typeToValueType[QMetaType::Double] = QMetaType::Double;
    connect(doublePropertyManager, SIGNAL(valueChanged(QtProperty*,double)),
                this, SLOT(slotValueChanged(QtProperty*,double)));
    connect(doublePropertyManager, SIGNAL(rangeChanged(QtProperty*,double,double)),
                this, SLOT(slotRangeChanged(QtProperty*,double,double)));
    connect(doublePropertyManager, SIGNAL(singleStepChanged(QtProperty*,double)),
                this, SLOT(slotSingleStepChanged(QtProperty*,double)));
    connect(doublePropertyManager, SIGNAL(decimalsChanged(QtProperty*,int)),
                this, SLOT(slotDecimalsChanged(QtProperty*,int)));
    // BoolPropertyManager
    QtBoolPropertyManager *boolPropertyManager = new QtBoolPropertyManager(this);
    d_ptr->m_typeToPropertyManager[QMetaType::Bool] = boolPropertyManager;
    d_ptr->m_typeToValueType[QMetaType::Bool] = QMetaType::Bool;
    connect(boolPropertyManager, SIGNAL(valueChanged(QtProperty*,bool)),
                this, SLOT(slotValueChanged(QtProperty*,bool)));
    // StringPropertyManager
    QtStringPropertyManager *stringPropertyManager = new QtStringPropertyManager(this);
    d_ptr->m_typeToPropertyManager[QMetaType::QString] = stringPropertyManager;
    d_ptr->m_typeToValueType[QMetaType::QString] = QMetaType::QString;
    d_ptr->m_typeToAttributeToAttributeType[QMetaType::QString][d_ptr->m_regExpAttribute] =
            QMetaType::QRegularExpression;
    connect(stringPropertyManager, SIGNAL(valueChanged(QtProperty*,QString)),
                this, SLOT(slotValueChanged(QtProperty*,QString)));
    connect(stringPropertyManager, SIGNAL(regExpChanged(QtProperty*,QRegularExpression)),
                this, SLOT(slotRegExpChanged(QtProperty*,QRegularExpression)));
    // DatePropertyManager
    QtDatePropertyManager *datePropertyManager = new QtDatePropertyManager(this);
    d_ptr->m_typeToPropertyManager[QMetaType::QDate] = datePropertyManager;
    d_ptr->m_typeToValueType[QMetaType::QDate] = QMetaType::QDate;
    d_ptr->m_typeToAttributeToAttributeType[QMetaType::QDate][d_ptr->m_minimumAttribute] =
            QMetaType::QDate;
    d_ptr->m_typeToAttributeToAttributeType[QMetaType::QDate][d_ptr->m_maximumAttribute] =
            QMetaType::QDate;
    connect(datePropertyManager, SIGNAL(valueChanged(QtProperty*,QDate)),
                this, SLOT(slotValueChanged(QtProperty*,QDate)));
    connect(datePropertyManager, SIGNAL(rangeChanged(QtProperty*,QDate,QDate)),
                this, SLOT(slotRangeChanged(QtProperty*,QDate,QDate)));
    // TimePropertyManager
    QtTimePropertyManager *timePropertyManager = new QtTimePropertyManager(this);
    d_ptr->m_typeToPropertyManager[QMetaType::QTime] = timePropertyManager;
    d_ptr->m_typeToValueType[QMetaType::QTime] = QMetaType::QTime;
    connect(timePropertyManager, SIGNAL(valueChanged(QtProperty*,QTime)),
                this, SLOT(slotValueChanged(QtProperty*,QTime)));
    // DateTimePropertyManager
    QtDateTimePropertyManager *dateTimePropertyManager = new QtDateTimePropertyManager(this);
    d_ptr->m_typeToPropertyManager[QMetaType::QDateTime] = dateTimePropertyManager;
    d_ptr->m_typeToValueType[QMetaType::QDateTime] = QMetaType::QDateTime;
    connect(dateTimePropertyManager, SIGNAL(valueChanged(QtProperty*,QDateTime)),
                this, SLOT(slotValueChanged(QtProperty*,QDateTime)));
    // KeySequencePropertyManager
    QtKeySequencePropertyManager *keySequencePropertyManager = new QtKeySequencePropertyManager(this);
    d_ptr->m_typeToPropertyManager[QMetaType::QKeySequence] = keySequencePropertyManager;
    d_ptr->m_typeToValueType[QMetaType::QKeySequence] = QMetaType::QKeySequence;
    connect(keySequencePropertyManager, SIGNAL(valueChanged(QtProperty*,QKeySequence)),
                this, SLOT(slotValueChanged(QtProperty*,QKeySequence)));
    // CharPropertyManager
    QtCharPropertyManager *charPropertyManager = new QtCharPropertyManager(this);
    d_ptr->m_typeToPropertyManager[QMetaType::QChar] = charPropertyManager;
    d_ptr->m_typeToValueType[QMetaType::QChar] = QMetaType::QChar;
    connect(charPropertyManager, SIGNAL(valueChanged(QtProperty*,QChar)),
                this, SLOT(slotValueChanged(QtProperty*,QChar)));
    // LocalePropertyManager
    QtLocalePropertyManager *localePropertyManager = new QtLocalePropertyManager(this);
    d_ptr->m_typeToPropertyManager[QMetaType::QLocale] = localePropertyManager;
    d_ptr->m_typeToValueType[QMetaType::QLocale] = QMetaType::QLocale;
    connect(localePropertyManager, SIGNAL(valueChanged(QtProperty*,QLocale)),
                this, SLOT(slotValueChanged(QtProperty*,QLocale)));
    connect(localePropertyManager->subEnumPropertyManager(), SIGNAL(valueChanged(QtProperty*,int)),
                this, SLOT(slotValueChanged(QtProperty*,int)));
    connect(localePropertyManager, SIGNAL(propertyInserted(QtProperty*,QtProperty*,QtProperty*)),
                this, SLOT(slotPropertyInserted(QtProperty*,QtProperty*,QtProperty*)));
    connect(localePropertyManager, SIGNAL(propertyRemoved(QtProperty*,QtProperty*)),
                this, SLOT(slotPropertyRemoved(QtProperty*,QtProperty*)));
    // PointPropertyManager
    QtPointPropertyManager *pointPropertyManager = new QtPointPropertyManager(this);
    d_ptr->m_typeToPropertyManager[QMetaType::QPoint] = pointPropertyManager;
    d_ptr->m_typeToValueType[QMetaType::QPoint] = QMetaType::QPoint;
    connect(pointPropertyManager, SIGNAL(valueChanged(QtProperty*,QPoint)),
                this, SLOT(slotValueChanged(QtProperty*,QPoint)));
    connect(pointPropertyManager->subIntPropertyManager(), SIGNAL(valueChanged(QtProperty*,int)),
                this, SLOT(slotValueChanged(QtProperty*,int)));
    connect(pointPropertyManager, SIGNAL(propertyInserted(QtProperty*,QtProperty*,QtProperty*)),
                this, SLOT(slotPropertyInserted(QtProperty*,QtProperty*,QtProperty*)));
    connect(pointPropertyManager, SIGNAL(propertyRemoved(QtProperty*,QtProperty*)),
                this, SLOT(slotPropertyRemoved(QtProperty*,QtProperty*)));
    // PointFPropertyManager
    QtPointFPropertyManager *pointFPropertyManager = new QtPointFPropertyManager(this);
    d_ptr->m_typeToPropertyManager[QMetaType::QPointF] = pointFPropertyManager;
    d_ptr->m_typeToValueType[QMetaType::QPointF] = QMetaType::QPointF;
    d_ptr->m_typeToAttributeToAttributeType[QMetaType::QPointF][d_ptr->m_decimalsAttribute] =
            QMetaType::Int;
    connect(pointFPropertyManager, SIGNAL(valueChanged(QtProperty*,QPointF)),
                this, SLOT(slotValueChanged(QtProperty*,QPointF)));
    connect(pointFPropertyManager, SIGNAL(decimalsChanged(QtProperty*,int)),
                this, SLOT(slotDecimalsChanged(QtProperty*,int)));
    connect(pointFPropertyManager->subDoublePropertyManager(), SIGNAL(valueChanged(QtProperty*,double)),
                this, SLOT(slotValueChanged(QtProperty*,double)));
    connect(pointFPropertyManager, SIGNAL(propertyInserted(QtProperty*,QtProperty*,QtProperty*)),
                this, SLOT(slotPropertyInserted(QtProperty*,QtProperty*,QtProperty*)));
    connect(pointFPropertyManager, SIGNAL(propertyRemoved(QtProperty*,QtProperty*)),
                this, SLOT(slotPropertyRemoved(QtProperty*,QtProperty*)));
    // SizePropertyManager
    QtSizePropertyManager *sizePropertyManager = new QtSizePropertyManager(this);
    d_ptr->m_typeToPropertyManager[QMetaType::QSize] = sizePropertyManager;
    d_ptr->m_typeToValueType[QMetaType::QSize] = QMetaType::QSize;
    d_ptr->m_typeToAttributeToAttributeType[QMetaType::QSize][d_ptr->m_minimumAttribute] =
            QMetaType::QSize;
    d_ptr->m_typeToAttributeToAttributeType[QMetaType::QSize][d_ptr->m_maximumAttribute] =
            QMetaType::QSize;
    connect(sizePropertyManager, SIGNAL(valueChanged(QtProperty*,QSize)),
                this, SLOT(slotValueChanged(QtProperty*,QSize)));
    connect(sizePropertyManager, SIGNAL(rangeChanged(QtProperty*,QSize,QSize)),
                this, SLOT(slotRangeChanged(QtProperty*,QSize,QSize)));
    connect(sizePropertyManager->subIntPropertyManager(), SIGNAL(valueChanged(QtProperty*,int)),
                this, SLOT(slotValueChanged(QtProperty*,int)));
    connect(sizePropertyManager->subIntPropertyManager(), SIGNAL(rangeChanged(QtProperty*,int,int)),
                this, SLOT(slotRangeChanged(QtProperty*,int,int)));
    connect(sizePropertyManager, SIGNAL(propertyInserted(QtProperty*,QtProperty*,QtProperty*)),
                this, SLOT(slotPropertyInserted(QtProperty*,QtProperty*,QtProperty*)));
    connect(sizePropertyManager, SIGNAL(propertyRemoved(QtProperty*,QtProperty*)),
                this, SLOT(slotPropertyRemoved(QtProperty*,QtProperty*)));
    // SizeFPropertyManager
    QtSizeFPropertyManager *sizeFPropertyManager = new QtSizeFPropertyManager(this);
    d_ptr->m_typeToPropertyManager[QMetaType::QSizeF] = sizeFPropertyManager;
    d_ptr->m_typeToValueType[QMetaType::QSizeF] = QMetaType::QSizeF;
    d_ptr->m_typeToAttributeToAttributeType[QMetaType::QSizeF][d_ptr->m_minimumAttribute] =
            QMetaType::QSizeF;
    d_ptr->m_typeToAttributeToAttributeType[QMetaType::QSizeF][d_ptr->m_maximumAttribute] =
            QMetaType::QSizeF;
    d_ptr->m_typeToAttributeToAttributeType[QMetaType::QSizeF][d_ptr->m_decimalsAttribute] =
            QMetaType::Int;
    connect(sizeFPropertyManager, SIGNAL(valueChanged(QtProperty*,QSizeF)),
                this, SLOT(slotValueChanged(QtProperty*,QSizeF)));
    connect(sizeFPropertyManager, SIGNAL(rangeChanged(QtProperty*,QSizeF,QSizeF)),
                this, SLOT(slotRangeChanged(QtProperty*,QSizeF,QSizeF)));
    connect(sizeFPropertyManager, SIGNAL(decimalsChanged(QtProperty*,int)),
                this, SLOT(slotDecimalsChanged(QtProperty*,int)));
    connect(sizeFPropertyManager->subDoublePropertyManager(), SIGNAL(valueChanged(QtProperty*,double)),
                this, SLOT(slotValueChanged(QtProperty*,double)));
    connect(sizeFPropertyManager->subDoublePropertyManager(), SIGNAL(rangeChanged(QtProperty*,double,double)),
                this, SLOT(slotRangeChanged(QtProperty*,double,double)));
    connect(sizeFPropertyManager, SIGNAL(propertyInserted(QtProperty*,QtProperty*,QtProperty*)),
                this, SLOT(slotPropertyInserted(QtProperty*,QtProperty*,QtProperty*)));
    connect(sizeFPropertyManager, SIGNAL(propertyRemoved(QtProperty*,QtProperty*)),
                this, SLOT(slotPropertyRemoved(QtProperty*,QtProperty*)));
    // RectPropertyManager
    QtRectPropertyManager *rectPropertyManager = new QtRectPropertyManager(this);
    d_ptr->m_typeToPropertyManager[QMetaType::QRect] = rectPropertyManager;
    d_ptr->m_typeToValueType[QMetaType::QRect] = QMetaType::QRect;
    d_ptr->m_typeToAttributeToAttributeType[QMetaType::QRect][d_ptr->m_constraintAttribute] =
            QMetaType::QRect;
    connect(rectPropertyManager, SIGNAL(valueChanged(QtProperty*,QRect)),
                this, SLOT(slotValueChanged(QtProperty*,QRect)));
    connect(rectPropertyManager, SIGNAL(constraintChanged(QtProperty*,QRect)),
                this, SLOT(slotConstraintChanged(QtProperty*,QRect)));
    connect(rectPropertyManager->subIntPropertyManager(), SIGNAL(valueChanged(QtProperty*,int)),
                this, SLOT(slotValueChanged(QtProperty*,int)));
    connect(rectPropertyManager->subIntPropertyManager(), SIGNAL(rangeChanged(QtProperty*,int,int)),
                this, SLOT(slotRangeChanged(QtProperty*,int,int)));
    connect(rectPropertyManager, SIGNAL(propertyInserted(QtProperty*,QtProperty*,QtProperty*)),
                this, SLOT(slotPropertyInserted(QtProperty*,QtProperty*,QtProperty*)));
    connect(rectPropertyManager, SIGNAL(propertyRemoved(QtProperty*,QtProperty*)),
                this, SLOT(slotPropertyRemoved(QtProperty*,QtProperty*)));
    // RectFPropertyManager
    QtRectFPropertyManager *rectFPropertyManager = new QtRectFPropertyManager(this);
    d_ptr->m_typeToPropertyManager[QMetaType::QRectF] = rectFPropertyManager;
    d_ptr->m_typeToValueType[QMetaType::QRectF] = QMetaType::QRectF;
    d_ptr->m_typeToAttributeToAttributeType[QMetaType::QRectF][d_ptr->m_constraintAttribute] =
            QMetaType::QRectF;
    d_ptr->m_typeToAttributeToAttributeType[QMetaType::QRectF][d_ptr->m_decimalsAttribute] =
            QMetaType::Int;
    connect(rectFPropertyManager, SIGNAL(valueChanged(QtProperty*,QRectF)),
                this, SLOT(slotValueChanged(QtProperty*,QRectF)));
    connect(rectFPropertyManager, SIGNAL(constraintChanged(QtProperty*,QRectF)),
                this, SLOT(slotConstraintChanged(QtProperty*,QRectF)));
    connect(rectFPropertyManager, SIGNAL(decimalsChanged(QtProperty*,int)),
                this, SLOT(slotDecimalsChanged(QtProperty*,int)));
    connect(rectFPropertyManager->subDoublePropertyManager(), SIGNAL(valueChanged(QtProperty*,double)),
                this, SLOT(slotValueChanged(QtProperty*,double)));
    connect(rectFPropertyManager->subDoublePropertyManager(), SIGNAL(rangeChanged(QtProperty*,double,double)),
                this, SLOT(slotRangeChanged(QtProperty*,double,double)));
    connect(rectFPropertyManager, SIGNAL(propertyInserted(QtProperty*,QtProperty*,QtProperty*)),
                this, SLOT(slotPropertyInserted(QtProperty*,QtProperty*,QtProperty*)));
    connect(rectFPropertyManager, SIGNAL(propertyRemoved(QtProperty*,QtProperty*)),
                this, SLOT(slotPropertyRemoved(QtProperty*,QtProperty*)));
    // ColorPropertyManager
    QtColorPropertyManager *colorPropertyManager = new QtColorPropertyManager(this);
    d_ptr->m_typeToPropertyManager[QMetaType::QColor] = colorPropertyManager;
    d_ptr->m_typeToValueType[QMetaType::QColor] = QMetaType::QColor;
    connect(colorPropertyManager, SIGNAL(valueChanged(QtProperty*,QColor)),
                this, SLOT(slotValueChanged(QtProperty*,QColor)));
    connect(colorPropertyManager->subIntPropertyManager(), SIGNAL(valueChanged(QtProperty*,int)),
                this, SLOT(slotValueChanged(QtProperty*,int)));
    connect(colorPropertyManager, SIGNAL(propertyInserted(QtProperty*,QtProperty*,QtProperty*)),
                this, SLOT(slotPropertyInserted(QtProperty*,QtProperty*,QtProperty*)));
    connect(colorPropertyManager, SIGNAL(propertyRemoved(QtProperty*,QtProperty*)),
                this, SLOT(slotPropertyRemoved(QtProperty*,QtProperty*)));
    // EnumPropertyManager
    int enumId = enumTypeId();
    QtEnumPropertyManager *enumPropertyManager = new QtEnumPropertyManager(this);
    d_ptr->m_typeToPropertyManager[enumId] = enumPropertyManager;
    d_ptr->m_typeToValueType[enumId] = QMetaType::Int;
    d_ptr->m_typeToAttributeToAttributeType[enumId][d_ptr->m_enumNamesAttribute] =
            QMetaType::QStringList;
    d_ptr->m_typeToAttributeToAttributeType[enumId][d_ptr->m_enumIconsAttribute] =
            iconMapTypeId();
    connect(enumPropertyManager, SIGNAL(valueChanged(QtProperty*,int)),
                this, SLOT(slotValueChanged(QtProperty*,int)));
    connect(enumPropertyManager, SIGNAL(enumNamesChanged(QtProperty*,QStringList)),
                this, SLOT(slotEnumNamesChanged(QtProperty*,QStringList)));
    connect(enumPropertyManager, SIGNAL(enumIconsChanged(QtProperty*,QMap<int,QIcon>)),
                this, SLOT(slotEnumIconsChanged(QtProperty*,QMap<int,QIcon>)));
    // SizePolicyPropertyManager
    QtSizePolicyPropertyManager *sizePolicyPropertyManager = new QtSizePolicyPropertyManager(this);
    d_ptr->m_typeToPropertyManager[QMetaType::QSizePolicy] = sizePolicyPropertyManager;
    d_ptr->m_typeToValueType[QMetaType::QSizePolicy] = QMetaType::QSizePolicy;
    connect(sizePolicyPropertyManager, SIGNAL(valueChanged(QtProperty*,QSizePolicy)),
                this, SLOT(slotValueChanged(QtProperty*,QSizePolicy)));
    connect(sizePolicyPropertyManager->subIntPropertyManager(), SIGNAL(valueChanged(QtProperty*,int)),
                this, SLOT(slotValueChanged(QtProperty*,int)));
    connect(sizePolicyPropertyManager->subIntPropertyManager(), SIGNAL(rangeChanged(QtProperty*,int,int)),
                this, SLOT(slotRangeChanged(QtProperty*,int,int)));
    connect(sizePolicyPropertyManager->subEnumPropertyManager(), SIGNAL(valueChanged(QtProperty*,int)),
                this, SLOT(slotValueChanged(QtProperty*,int)));
    connect(sizePolicyPropertyManager->subEnumPropertyManager(),
                SIGNAL(enumNamesChanged(QtProperty*,QStringList)),
                this, SLOT(slotEnumNamesChanged(QtProperty*,QStringList)));
    connect(sizePolicyPropertyManager, SIGNAL(propertyInserted(QtProperty*,QtProperty*,QtProperty*)),
                this, SLOT(slotPropertyInserted(QtProperty*,QtProperty*,QtProperty*)));
    connect(sizePolicyPropertyManager, SIGNAL(propertyRemoved(QtProperty*,QtProperty*)),
                this, SLOT(slotPropertyRemoved(QtProperty*,QtProperty*)));
    // FontPropertyManager
    QtFontPropertyManager *fontPropertyManager = new QtFontPropertyManager(this);
    d_ptr->m_typeToPropertyManager[QMetaType::QFont] = fontPropertyManager;
    d_ptr->m_typeToValueType[QMetaType::QFont] = QMetaType::QFont;
    connect(fontPropertyManager, SIGNAL(valueChanged(QtProperty*,QFont)),
                this, SLOT(slotValueChanged(QtProperty*,QFont)));
    connect(fontPropertyManager->subIntPropertyManager(), SIGNAL(valueChanged(QtProperty*,int)),
                this, SLOT(slotValueChanged(QtProperty*,int)));
    connect(fontPropertyManager->subIntPropertyManager(), SIGNAL(rangeChanged(QtProperty*,int,int)),
                this, SLOT(slotRangeChanged(QtProperty*,int,int)));
    connect(fontPropertyManager->subEnumPropertyManager(), SIGNAL(valueChanged(QtProperty*,int)),
                this, SLOT(slotValueChanged(QtProperty*,int)));
    connect(fontPropertyManager->subEnumPropertyManager(),
                SIGNAL(enumNamesChanged(QtProperty*,QStringList)),
                this, SLOT(slotEnumNamesChanged(QtProperty*,QStringList)));
    connect(fontPropertyManager->subBoolPropertyManager(), SIGNAL(valueChanged(QtProperty*,bool)),
                this, SLOT(slotValueChanged(QtProperty*,bool)));
    connect(fontPropertyManager, SIGNAL(propertyInserted(QtProperty*,QtProperty*,QtProperty*)),
                this, SLOT(slotPropertyInserted(QtProperty*,QtProperty*,QtProperty*)));
    connect(fontPropertyManager, SIGNAL(propertyRemoved(QtProperty*,QtProperty*)),
                this, SLOT(slotPropertyRemoved(QtProperty*,QtProperty*)));
    // CursorPropertyManager
    QtCursorPropertyManager *cursorPropertyManager = new QtCursorPropertyManager(this);
    d_ptr->m_typeToPropertyManager[QMetaType::QCursor] = cursorPropertyManager;
    d_ptr->m_typeToValueType[QMetaType::QCursor] = QMetaType::QCursor;
    connect(cursorPropertyManager, SIGNAL(valueChanged(QtProperty*,QCursor)),
                this, SLOT(slotValueChanged(QtProperty*,QCursor)));
    // FlagPropertyManager
    int flagId = flagTypeId();
    QtFlagPropertyManager *flagPropertyManager = new QtFlagPropertyManager(this);
    d_ptr->m_typeToPropertyManager[flagId] = flagPropertyManager;
    d_ptr->m_typeToValueType[flagId] = QMetaType::Int;
    d_ptr->m_typeToAttributeToAttributeType[flagId][d_ptr->m_flagNamesAttribute] =
            QMetaType::QStringList;
    connect(flagPropertyManager, SIGNAL(valueChanged(QtProperty*,int)),
                this, SLOT(slotValueChanged(QtProperty*,int)));
    connect(flagPropertyManager, SIGNAL(flagNamesChanged(QtProperty*,QStringList)),
                this, SLOT(slotFlagNamesChanged(QtProperty*,QStringList)));
    connect(flagPropertyManager->subBoolPropertyManager(), SIGNAL(valueChanged(QtProperty*,bool)),
                this, SLOT(slotValueChanged(QtProperty*,bool)));
    connect(flagPropertyManager, SIGNAL(propertyInserted(QtProperty*,QtProperty*,QtProperty*)),
                this, SLOT(slotPropertyInserted(QtProperty*,QtProperty*,QtProperty*)));
    connect(flagPropertyManager, SIGNAL(propertyRemoved(QtProperty*,QtProperty*)),
                this, SLOT(slotPropertyRemoved(QtProperty*,QtProperty*)));
    // FlagPropertyManager
    int groupId = groupTypeId();
    QtGroupPropertyManager *groupPropertyManager = new QtGroupPropertyManager(this);
    d_ptr->m_typeToPropertyManager[groupId] = groupPropertyManager;
    d_ptr->m_typeToValueType[groupId] = QMetaType::UnknownType;
}

/*!
    Destroys this manager, and all the properties it has created.
*/
QtVariantPropertyManager::~QtVariantPropertyManager()
{
    clear();
}

/*!
    Returns the given \a property converted into a QtVariantProperty.

    If the \a property was not created by this variant manager, the
    function returns 0.

    \sa createProperty()
*/
QtVariantProperty *QtVariantPropertyManager::variantProperty(const QtProperty *property) const
{
    const QMap<const QtProperty *, QPair<QtVariantProperty *, int> >::const_iterator it = d_ptr->m_propertyToType.constFind(property);
    if (it == d_ptr->m_propertyToType.constEnd())
        return 0;
    return it.value().first;
}

/*!
    Returns true if the given \a propertyType is supported by this
    variant manager; otherwise false.

    \sa propertyType()
*/
bool QtVariantPropertyManager::isPropertyTypeSupported(int propertyType) const
{
    if (d_ptr->m_typeToValueType.contains(propertyType))
        return true;
    return false;
}

/*!
   Creates and returns a variant property of the given \a propertyType
   with the given \a name.

   If the specified \a propertyType is not supported by this variant
   manager, this function returns 0.

   Do not use the inherited
   QtAbstractPropertyManager::addProperty() function to create a
   variant property (that function will always return 0 since it will
   not be clear what type the property should have).

    \sa isPropertyTypeSupported()
*/
QtVariantProperty *QtVariantPropertyManager::addProperty(int propertyType, const QString &name)
{
    if (!isPropertyTypeSupported(propertyType))
        return 0;

    bool wasCreating = d_ptr->m_creatingProperty;
    d_ptr->m_creatingProperty = true;
    d_ptr->m_propertyType = propertyType;
    QtProperty *property = QtAbstractPropertyManager::addProperty(name);
    d_ptr->m_creatingProperty = wasCreating;
    d_ptr->m_propertyType = 0;

    if (!property)
        return 0;

    return variantProperty(property);
}

/*!
    Returns the given \a property's value.

    If the given \a property is not managed by this manager, this
    function returns an invalid variant.

    \sa setValue()
*/
QVariant QtVariantPropertyManager::value(const QtProperty *property) const
{
    QtProperty *internProp = propertyToWrappedProperty()->value(property, 0);
    if (internProp == 0)
        return QVariant();

    QtAbstractPropertyManager *manager = internProp->propertyManager();
    if (QtIntPropertyManager *intManager = qobject_cast<QtIntPropertyManager *>(manager)) {
        return intManager->value(internProp);
    } else if (QtDoublePropertyManager *doubleManager = qobject_cast<QtDoublePropertyManager *>(manager)) {
        return doubleManager->value(internProp);
    } else if (QtBoolPropertyManager *boolManager = qobject_cast<QtBoolPropertyManager *>(manager)) {
        return boolManager->value(internProp);
    } else if (QtStringPropertyManager *stringManager = qobject_cast<QtStringPropertyManager *>(manager)) {
        return stringManager->value(internProp);
    } else if (QtDatePropertyManager *dateManager = qobject_cast<QtDatePropertyManager *>(manager)) {
        return dateManager->value(internProp);
    } else if (QtTimePropertyManager *timeManager = qobject_cast<QtTimePropertyManager *>(manager)) {
        return timeManager->value(internProp);
    } else if (QtDateTimePropertyManager *dateTimeManager = qobject_cast<QtDateTimePropertyManager *>(manager)) {
        return dateTimeManager->value(internProp);
    } else if (QtKeySequencePropertyManager *keySequenceManager = qobject_cast<QtKeySequencePropertyManager *>(manager)) {
        return QVariant::fromValue(keySequenceManager->value(internProp));
    } else if (QtCharPropertyManager *charManager = qobject_cast<QtCharPropertyManager *>(manager)) {
        return charManager->value(internProp);
    } else if (QtLocalePropertyManager *localeManager = qobject_cast<QtLocalePropertyManager *>(manager)) {
        return localeManager->value(internProp);
    } else if (QtPointPropertyManager *pointManager = qobject_cast<QtPointPropertyManager *>(manager)) {
        return pointManager->value(internProp);
    } else if (QtPointFPropertyManager *pointFManager = qobject_cast<QtPointFPropertyManager *>(manager)) {
        return pointFManager->value(internProp);
    } else if (QtSizePropertyManager *sizeManager = qobject_cast<QtSizePropertyManager *>(manager)) {
        return sizeManager->value(internProp);
    } else if (QtSizeFPropertyManager *sizeFManager = qobject_cast<QtSizeFPropertyManager *>(manager)) {
        return sizeFManager->value(internProp);
    } else if (QtRectPropertyManager *rectManager = qobject_cast<QtRectPropertyManager *>(manager)) {
        return rectManager->value(internProp);
    } else if (QtRectFPropertyManager *rectFManager = qobject_cast<QtRectFPropertyManager *>(manager)) {
        return rectFManager->value(internProp);
    } else if (QtColorPropertyManager *colorManager = qobject_cast<QtColorPropertyManager *>(manager)) {
        return colorManager->value(internProp);
    } else if (QtEnumPropertyManager *enumManager = qobject_cast<QtEnumPropertyManager *>(manager)) {
        return enumManager->value(internProp);
    } else if (QtSizePolicyPropertyManager *sizePolicyManager =
               qobject_cast<QtSizePolicyPropertyManager *>(manager)) {
        return sizePolicyManager->value(internProp);
    } else if (QtFontPropertyManager *fontManager = qobject_cast<QtFontPropertyManager *>(manager)) {
        return fontManager->value(internProp);
#ifndef QT_NO_CURSOR
    } else if (QtCursorPropertyManager *cursorManager = qobject_cast<QtCursorPropertyManager *>(manager)) {
        return cursorManager->value(internProp);
#endif
    } else if (QtFlagPropertyManager *flagManager = qobject_cast<QtFlagPropertyManager *>(manager)) {
        return flagManager->value(internProp);
    }
    return QVariant();
}

/*!
    Returns the given \a property's value type.

    \sa propertyType()
*/
int QtVariantPropertyManager::valueType(const QtProperty *property) const
{
    int propType = propertyType(property);
    return valueType(propType);
}

/*!
    \overload

    Returns the value type associated with the given \a propertyType.
*/
int QtVariantPropertyManager::valueType(int propertyType) const
{
    if (d_ptr->m_typeToValueType.contains(propertyType))
        return d_ptr->m_typeToValueType[propertyType];
    return 0;
}

/*!
    Returns the given \a property's type.

    \sa valueType()
*/
int QtVariantPropertyManager::propertyType(const QtProperty *property) const
{
    const QMap<const QtProperty *, QPair<QtVariantProperty *, int> >::const_iterator it = d_ptr->m_propertyToType.constFind(property);
    if (it == d_ptr->m_propertyToType.constEnd())
        return 0;
    return it.value().second;
}

/*!
    Returns the given \a property's value for the specified \a
    attribute

    If the given \a property was not created by \e this manager, or if
    the specified \a attribute does not exist, this function returns
    an invalid variant.

    \sa attributes(), attributeType(), setAttribute()
*/
QVariant QtVariantPropertyManager::attributeValue(const QtProperty *property, const QString &attribute) const
{
    int propType = propertyType(property);
    if (!propType)
        return QVariant();

    QMap<int, QMap<QString, int> >::ConstIterator it =
            d_ptr->m_typeToAttributeToAttributeType.find(propType);
    if (it == d_ptr->m_typeToAttributeToAttributeType.constEnd())
        return QVariant();

    QMap<QString, int> attributes = it.value();
    QMap<QString, int>::ConstIterator itAttr = attributes.find(attribute);
    if (itAttr == attributes.constEnd())
        return QVariant();

    QtProperty *internProp = propertyToWrappedProperty()->value(property, 0);
    if (internProp == 0)
        return QVariant();

    QtAbstractPropertyManager *manager = internProp->propertyManager();
    if (QtIntPropertyManager *intManager = qobject_cast<QtIntPropertyManager *>(manager)) {
        if (attribute == d_ptr->m_maximumAttribute)
            return intManager->maximum(internProp);
        if (attribute == d_ptr->m_minimumAttribute)
            return intManager->minimum(internProp);
        if (attribute == d_ptr->m_singleStepAttribute)
            return intManager->singleStep(internProp);
        return QVariant();
    } else if (QtDoublePropertyManager *doubleManager = qobject_cast<QtDoublePropertyManager *>(manager)) {
        if (attribute == d_ptr->m_maximumAttribute)
            return doubleManager->maximum(internProp);
        if (attribute == d_ptr->m_minimumAttribute)
            return doubleManager->minimum(internProp);
        if (attribute == d_ptr->m_singleStepAttribute)
            return doubleManager->singleStep(internProp);
        if (attribute == d_ptr->m_decimalsAttribute)
            return doubleManager->decimals(internProp);
        return QVariant();
    } else if (QtStringPropertyManager *stringManager = qobject_cast<QtStringPropertyManager *>(manager)) {
        if (attribute == d_ptr->m_regExpAttribute)
            return stringManager->regExp(internProp);
        return QVariant();
    } else if (QtDatePropertyManager *dateManager = qobject_cast<QtDatePropertyManager *>(manager)) {
        if (attribute == d_ptr->m_maximumAttribute)
            return dateManager->maximum(internProp);
        if (attribute == d_ptr->m_minimumAttribute)
            return dateManager->minimum(internProp);
        return QVariant();
    } else if (QtPointFPropertyManager *pointFManager = qobject_cast<QtPointFPropertyManager *>(manager)) {
        if (attribute == d_ptr->m_decimalsAttribute)
            return pointFManager->decimals(internProp);
        return QVariant();
    } else if (QtSizePropertyManager *sizeManager = qobject_cast<QtSizePropertyManager *>(manager)) {
        if (attribute == d_ptr->m_maximumAttribute)
            return sizeManager->maximum(internProp);
        if (attribute == d_ptr->m_minimumAttribute)
            return sizeManager->minimum(internProp);
        return QVariant();
    } else if (QtSizeFPropertyManager *sizeFManager = qobject_cast<QtSizeFPropertyManager *>(manager)) {
        if (attribute == d_ptr->m_maximumAttribute)
            return sizeFManager->maximum(internProp);
        if (attribute == d_ptr->m_minimumAttribute)
            return sizeFManager->minimum(internProp);
        if (attribute == d_ptr->m_decimalsAttribute)
            return sizeFManager->decimals(internProp);
        return QVariant();
    } else if (QtRectPropertyManager *rectManager = qobject_cast<QtRectPropertyManager *>(manager)) {
        if (attribute == d_ptr->m_constraintAttribute)
            return rectManager->constraint(internProp);
        return QVariant();
    } else if (QtRectFPropertyManager *rectFManager = qobject_cast<QtRectFPropertyManager *>(manager)) {
        if (attribute == d_ptr->m_constraintAttribute)
            return rectFManager->constraint(internProp);
        if (attribute == d_ptr->m_decimalsAttribute)
            return rectFManager->decimals(internProp);
        return QVariant();
    } else if (QtEnumPropertyManager *enumManager = qobject_cast<QtEnumPropertyManager *>(manager)) {
        if (attribute == d_ptr->m_enumNamesAttribute)
            return enumManager->enumNames(internProp);
        if (attribute == d_ptr->m_enumIconsAttribute) {
            QVariant v;
            v.setValue(enumManager->enumIcons(internProp));
            return v;
        }
        return QVariant();
    } else if (QtFlagPropertyManager *flagManager = qobject_cast<QtFlagPropertyManager *>(manager)) {
        if (attribute == d_ptr->m_flagNamesAttribute)
            return flagManager->flagNames(internProp);
        return QVariant();
    }
    return QVariant();
}

/*!
    Returns a list of the given \a propertyType 's attributes.

    \sa attributeValue(), attributeType()
*/
QStringList QtVariantPropertyManager::attributes(int propertyType) const
{
    QMap<int, QMap<QString, int> >::ConstIterator it =
            d_ptr->m_typeToAttributeToAttributeType.find(propertyType);
    if (it == d_ptr->m_typeToAttributeToAttributeType.constEnd())
        return QStringList();
    return it.value().keys();
}

/*!
    Returns the type of the specified \a attribute of the given \a
    propertyType.

    If the given \a propertyType is not supported by \e this manager,
    or if the given \a propertyType does not possess the specified \a
    attribute, this function returns QMetaType::UnknownType.

    \sa attributes(), valueType()
*/
int QtVariantPropertyManager::attributeType(int propertyType, const QString &attribute) const
{
    QMap<int, QMap<QString, int> >::ConstIterator it =
            d_ptr->m_typeToAttributeToAttributeType.find(propertyType);
    if (it == d_ptr->m_typeToAttributeToAttributeType.constEnd())
        return 0;

    QMap<QString, int> attributes = it.value();
    QMap<QString, int>::ConstIterator itAttr = attributes.find(attribute);
    if (itAttr == attributes.constEnd())
        return 0;
    return itAttr.value();
}

/*!
    \fn void QtVariantPropertyManager::setValue(QtProperty *property, const QVariant &value)

    Sets the value of the given \a property to \a value.

    The specified \a value must be of a type returned by valueType(),
    or of type that can be converted to valueType() using the
    QVariant::canConvert() function, otherwise this function does
    nothing.

    \sa value(), QtVariantProperty::setValue(), valueChanged()
*/
void QtVariantPropertyManager::setValue(QtProperty *property, const QVariant &val)
{
    int propType = val.userType();
    if (!propType)
        return;

    int valType = valueType(property);

    if (propType != valType && !val.canConvert(QMetaType(valType)))
        return;

    QtProperty *internProp = propertyToWrappedProperty()->value(property, 0);
    if (internProp == 0)
        return;


    QtAbstractPropertyManager *manager = internProp->propertyManager();
    if (QtIntPropertyManager *intManager = qobject_cast<QtIntPropertyManager *>(manager)) {
        intManager->setValue(internProp, qvariant_cast<int>(val));
        return;
    } else if (QtDoublePropertyManager *doubleManager = qobject_cast<QtDoublePropertyManager *>(manager)) {
        doubleManager->setValue(internProp, qvariant_cast<double>(val));
        return;
    } else if (QtBoolPropertyManager *boolManager = qobject_cast<QtBoolPropertyManager *>(manager)) {
        boolManager->setValue(internProp, qvariant_cast<bool>(val));
        return;
    } else if (QtStringPropertyManager *stringManager = qobject_cast<QtStringPropertyManager *>(manager)) {
        stringManager->setValue(internProp, qvariant_cast<QString>(val));
        return;
    } else if (QtDatePropertyManager *dateManager = qobject_cast<QtDatePropertyManager *>(manager)) {
        dateManager->setValue(internProp, qvariant_cast<QDate>(val));
        return;
    } else if (QtTimePropertyManager *timeManager = qobject_cast<QtTimePropertyManager *>(manager)) {
        timeManager->setValue(internProp, qvariant_cast<QTime>(val));
        return;
    } else if (QtDateTimePropertyManager *dateTimeManager = qobject_cast<QtDateTimePropertyManager *>(manager)) {
        dateTimeManager->setValue(internProp, qvariant_cast<QDateTime>(val));
        return;
    } else if (QtKeySequencePropertyManager *keySequenceManager = qobject_cast<QtKeySequencePropertyManager *>(manager)) {
        keySequenceManager->setValue(internProp, qvariant_cast<QKeySequence>(val));
        return;
    } else if (QtCharPropertyManager *charManager = qobject_cast<QtCharPropertyManager *>(manager)) {
        charManager->setValue(internProp, qvariant_cast<QChar>(val));
        return;
    } else if (QtLocalePropertyManager *localeManager = qobject_cast<QtLocalePropertyManager *>(manager)) {
        localeManager->setValue(internProp, qvariant_cast<QLocale>(val));
        return;
    } else if (QtPointPropertyManager *pointManager = qobject_cast<QtPointPropertyManager *>(manager)) {
        pointManager->setValue(internProp, qvariant_cast<QPoint>(val));
        return;
    } else if (QtPointFPropertyManager *pointFManager = qobject_cast<QtPointFPropertyManager *>(manager)) {
        pointFManager->setValue(internProp, qvariant_cast<QPointF>(val));
        return;
    } else if (QtSizePropertyManager *sizeManager = qobject_cast<QtSizePropertyManager *>(manager)) {
        sizeManager->setValue(internProp, qvariant_cast<QSize>(val));
        return;
    } else if (QtSizeFPropertyManager *sizeFManager = qobject_cast<QtSizeFPropertyManager *>(manager)) {
        sizeFManager->setValue(internProp, qvariant_cast<QSizeF>(val));
        return;
    } else if (QtRectPropertyManager *rectManager = qobject_cast<QtRectPropertyManager *>(manager)) {
        rectManager->setValue(internProp, qvariant_cast<QRect>(val));
        return;
    } else if (QtRectFPropertyManager *rectFManager = qobject_cast<QtRectFPropertyManager *>(manager)) {
        rectFManager->setValue(internProp, qvariant_cast<QRectF>(val));
        return;
    } else if (QtColorPropertyManager *colorManager = qobject_cast<QtColorPropertyManager *>(manager)) {
        colorManager->setValue(internProp, qvariant_cast<QColor>(val));
        return;
    } else if (QtEnumPropertyManager *enumManager = qobject_cast<QtEnumPropertyManager *>(manager)) {
        enumManager->setValue(internProp, qvariant_cast<int>(val));
        return;
    } else if (QtSizePolicyPropertyManager *sizePolicyManager =
               qobject_cast<QtSizePolicyPropertyManager *>(manager)) {
        sizePolicyManager->setValue(internProp, qvariant_cast<QSizePolicy>(val));
        return;
    } else if (QtFontPropertyManager *fontManager = qobject_cast<QtFontPropertyManager *>(manager)) {
        fontManager->setValue(internProp, qvariant_cast<QFont>(val));
        return;
#ifndef QT_NO_CURSOR
    } else if (QtCursorPropertyManager *cursorManager = qobject_cast<QtCursorPropertyManager *>(manager)) {
        cursorManager->setValue(internProp, qvariant_cast<QCursor>(val));
        return;
#endif
    } else if (QtFlagPropertyManager *flagManager = qobject_cast<QtFlagPropertyManager *>(manager)) {
        flagManager->setValue(internProp, qvariant_cast<int>(val));
        return;
    }
}

/*!
    Sets the value of the specified \a attribute of the given \a
    property, to \a value.

    The new \a value's type must be of the type returned by
    attributeType(), or of a type that can be converted to
    attributeType() using the QVariant::canConvert() function,
    otherwise this function does nothing.

    \sa attributeValue(), QtVariantProperty::setAttribute(), attributeChanged()
*/
void QtVariantPropertyManager::setAttribute(QtProperty *property,
        const QString &attribute, const QVariant &value)
{
    QVariant oldAttr = attributeValue(property, attribute);
    if (!oldAttr.isValid())
        return;

    int attrType = value.userType();
    if (!attrType)
        return;

    if (attrType != attributeType(propertyType(property), attribute) &&
                !value.canConvert(QMetaType(attrType)))
        return;

    QtProperty *internProp = propertyToWrappedProperty()->value(property, 0);
    if (internProp == 0)
        return;

    QtAbstractPropertyManager *manager = internProp->propertyManager();
    if (QtIntPropertyManager *intManager = qobject_cast<QtIntPropertyManager *>(manager)) {
        if (attribute == d_ptr->m_maximumAttribute)
            intManager->setMaximum(internProp, qvariant_cast<int>(value));
        else if (attribute == d_ptr->m_minimumAttribute)
            intManager->setMinimum(internProp, qvariant_cast<int>(value));
        else if (attribute == d_ptr->m_singleStepAttribute)
            intManager->setSingleStep(internProp, qvariant_cast<int>(value));
        return;
    } else if (QtDoublePropertyManager *doubleManager = qobject_cast<QtDoublePropertyManager *>(manager)) {
        if (attribute == d_ptr->m_maximumAttribute)
            doubleManager->setMaximum(internProp, qvariant_cast<double>(value));
        if (attribute == d_ptr->m_minimumAttribute)
            doubleManager->setMinimum(internProp, qvariant_cast<double>(value));
        if (attribute == d_ptr->m_singleStepAttribute)
            doubleManager->setSingleStep(internProp, qvariant_cast<double>(value));
        if (attribute == d_ptr->m_decimalsAttribute)
            doubleManager->setDecimals(internProp, qvariant_cast<int>(value));
        return;
    } else if (QtStringPropertyManager *stringManager = qobject_cast<QtStringPropertyManager *>(manager)) {
        if (attribute == d_ptr->m_regExpAttribute)
            stringManager->setRegExp(internProp, qvariant_cast<QRegularExpression>(value));
        return;
    } else if (QtDatePropertyManager *dateManager = qobject_cast<QtDatePropertyManager *>(manager)) {
        if (attribute == d_ptr->m_maximumAttribute)
            dateManager->setMaximum(internProp, qvariant_cast<QDate>(value));
        if (attribute == d_ptr->m_minimumAttribute)
            dateManager->setMinimum(internProp, qvariant_cast<QDate>(value));
        return;
    } else if (QtPointFPropertyManager *pointFManager = qobject_cast<QtPointFPropertyManager *>(manager)) {
        if (attribute == d_ptr->m_decimalsAttribute)
            pointFManager->setDecimals(internProp, qvariant_cast<int>(value));
        return;
    } else if (QtSizePropertyManager *sizeManager = qobject_cast<QtSizePropertyManager *>(manager)) {
        if (attribute == d_ptr->m_maximumAttribute)
            sizeManager->setMaximum(internProp, qvariant_cast<QSize>(value));
        if (attribute == d_ptr->m_minimumAttribute)
            sizeManager->setMinimum(internProp, qvariant_cast<QSize>(value));
        return;
    } else if (QtSizeFPropertyManager *sizeFManager = qobject_cast<QtSizeFPropertyManager *>(manager)) {
        if (attribute == d_ptr->m_maximumAttribute)
            sizeFManager->setMaximum(internProp, qvariant_cast<QSizeF>(value));
        if (attribute == d_ptr->m_minimumAttribute)
            sizeFManager->setMinimum(internProp, qvariant_cast<QSizeF>(value));
        if (attribute == d_ptr->m_decimalsAttribute)
            sizeFManager->setDecimals(internProp, qvariant_cast<int>(value));
        return;
    } else if (QtRectPropertyManager *rectManager = qobject_cast<QtRectPropertyManager *>(manager)) {
        if (attribute == d_ptr->m_constraintAttribute)
            rectManager->setConstraint(internProp, qvariant_cast<QRect>(value));
        return;
    } else if (QtRectFPropertyManager *rectFManager = qobject_cast<QtRectFPropertyManager *>(manager)) {
        if (attribute == d_ptr->m_constraintAttribute)
            rectFManager->setConstraint(internProp, qvariant_cast<QRectF>(value));
        if (attribute == d_ptr->m_decimalsAttribute)
            rectFManager->setDecimals(internProp, qvariant_cast<int>(value));
        return;
    } else if (QtEnumPropertyManager *enumManager = qobject_cast<QtEnumPropertyManager *>(manager)) {
        if (attribute == d_ptr->m_enumNamesAttribute)
            enumManager->setEnumNames(internProp, qvariant_cast<QStringList>(value));
        if (attribute == d_ptr->m_enumIconsAttribute)
            enumManager->setEnumIcons(internProp, qvariant_cast<QtIconMap>(value));
        return;
    } else if (QtFlagPropertyManager *flagManager = qobject_cast<QtFlagPropertyManager *>(manager)) {
        if (attribute == d_ptr->m_flagNamesAttribute)
            flagManager->setFlagNames(internProp, qvariant_cast<QStringList>(value));
        return;
    }
}

/*!
    \internal
*/
bool QtVariantPropertyManager::hasValue(const QtProperty *property) const
{
    if (propertyType(property) == groupTypeId())
        return false;
    return true;
}

/*!
    \internal
*/
QString QtVariantPropertyManager::valueText(const QtProperty *property) const
{
    const QtProperty *internProp = propertyToWrappedProperty()->value(property, 0);
    return internProp ? internProp->valueText() : QString();
}

/*!
    \internal
*/
QIcon QtVariantPropertyManager::valueIcon(const QtProperty *property) const
{
    const QtProperty *internProp = propertyToWrappedProperty()->value(property, 0);
    return internProp ? internProp->valueIcon() : QIcon();
}

/*!
    \internal
*/
void QtVariantPropertyManager::initializeProperty(QtProperty *property)
{
    QtVariantProperty *varProp = variantProperty(property);
    if (!varProp)
        return;

    QMap<int, QtAbstractPropertyManager *>::ConstIterator it =
            d_ptr->m_typeToPropertyManager.find(d_ptr->m_propertyType);
    if (it != d_ptr->m_typeToPropertyManager.constEnd()) {
        QtProperty *internProp = 0;
        if (!d_ptr->m_creatingSubProperties) {
            QtAbstractPropertyManager *manager = it.value();
            internProp = manager->addProperty();
            d_ptr->m_internalToProperty[internProp] = varProp;
        }
        propertyToWrappedProperty()->insert(varProp, internProp);
        if (internProp) {
            const auto children = internProp->subProperties();
            QtVariantProperty *lastProperty = 0;
            for (QtProperty *child : children) {
                QtVariantProperty *prop = d_ptr->createSubProperty(varProp, lastProperty, child);
                lastProperty = prop ? prop : lastProperty;
            }
        }
    }
}

/*!
    \internal
*/
void QtVariantPropertyManager::uninitializeProperty(QtProperty *property)
{
    const QMap<const QtProperty *, QPair<QtVariantProperty *, int> >::iterator type_it = d_ptr->m_propertyToType.find(property);
    if (type_it == d_ptr->m_propertyToType.end())
        return;

    PropertyMap::iterator it = propertyToWrappedProperty()->find(property);
    if (it != propertyToWrappedProperty()->end()) {
        QtProperty *internProp = it.value();
        if (internProp) {
            d_ptr->m_internalToProperty.remove(internProp);
            if (!d_ptr->m_destroyingSubProperties) {
                delete internProp;
            }
        }
        propertyToWrappedProperty()->erase(it);
    }
    d_ptr->m_propertyToType.erase(type_it);
}

/*!
    \internal
*/
QtProperty *QtVariantPropertyManager::createProperty()
{
    if (!d_ptr->m_creatingProperty)
        return 0;

    QtVariantProperty *property = new QtVariantProperty(this);
    d_ptr->m_propertyToType.insert(property, qMakePair(property, d_ptr->m_propertyType));

    return property;
}

/////////////////////////////

class QtVariantEditorFactoryPrivate
{
    QtVariantEditorFactory *q_ptr;
    Q_DECLARE_PUBLIC(QtVariantEditorFactory)
public:

    QtSpinBoxFactory           *m_spinBoxFactory;
    QtDoubleSpinBoxFactory     *m_doubleSpinBoxFactory;
    QtCheckBoxFactory          *m_checkBoxFactory;
    QtLineEditFactory          *m_lineEditFactory;
    QtDateEditFactory          *m_dateEditFactory;
    QtTimeEditFactory          *m_timeEditFactory;
    QtDateTimeEditFactory      *m_dateTimeEditFactory;
    QtKeySequenceEditorFactory *m_keySequenceEditorFactory;
    QtCharEditorFactory        *m_charEditorFactory;
    QtEnumEditorFactory        *m_comboBoxFactory;
    QtCursorEditorFactory      *m_cursorEditorFactory;
    QtColorEditorFactory       *m_colorEditorFactory;
    QtFontEditorFactory        *m_fontEditorFactory;

    QMap<QtAbstractEditorFactoryBase *, int> m_factoryToType;
    QMap<int, QtAbstractEditorFactoryBase *> m_typeToFactory;
};

/*!
    \class QtVariantEditorFactory
    \internal
    \inmodule QtDesigner
    \since 4.4

    \brief The QtVariantEditorFactory class provides widgets for properties
    created by QtVariantPropertyManager objects.

    The variant factory provides the following widgets for the
    specified property types:

    \table
    \header
        \li Property Type
        \li Widget
    \row
        \li \c int
        \li QSpinBox
    \row
        \li \c double
        \li QDoubleSpinBox
    \row
        \li \c bool
        \li QCheckBox
    \row
        \li QString
        \li QLineEdit
    \row
        \li QDate
        \li QDateEdit
    \row
        \li QTime
        \li QTimeEdit
    \row
        \li QDateTime
        \li QDateTimeEdit
    \row
        \li QKeySequence
        \li customized editor
    \row
        \li QChar
        \li customized editor
    \row
        \li \c enum
        \li QComboBox
    \row
        \li QCursor
        \li QComboBox
    \endtable

    Note that QtVariantPropertyManager supports several additional property
    types for which the QtVariantEditorFactory class does not provide
    editing widgets, e.g. QPoint and QSize. To provide widgets for other
    types using the variant approach, derive from the QtVariantEditorFactory
    class.

    \sa QtAbstractEditorFactory, QtVariantPropertyManager
*/

/*!
    Creates a factory with the given \a parent.
*/
QtVariantEditorFactory::QtVariantEditorFactory(QObject *parent)
    : QtAbstractEditorFactory<QtVariantPropertyManager>(parent), d_ptr(new QtVariantEditorFactoryPrivate())
{
    d_ptr->q_ptr = this;

    d_ptr->m_spinBoxFactory = new QtSpinBoxFactory(this);
    d_ptr->m_factoryToType[d_ptr->m_spinBoxFactory] = QMetaType::Int;
    d_ptr->m_typeToFactory[QMetaType::Int] = d_ptr->m_spinBoxFactory;

    d_ptr->m_doubleSpinBoxFactory = new QtDoubleSpinBoxFactory(this);
    d_ptr->m_factoryToType[d_ptr->m_doubleSpinBoxFactory] = QMetaType::Double;
    d_ptr->m_typeToFactory[QMetaType::Double] = d_ptr->m_doubleSpinBoxFactory;

    d_ptr->m_checkBoxFactory = new QtCheckBoxFactory(this);
    d_ptr->m_factoryToType[d_ptr->m_checkBoxFactory] = QMetaType::Bool;
    d_ptr->m_typeToFactory[QMetaType::Bool] = d_ptr->m_checkBoxFactory;

    d_ptr->m_lineEditFactory = new QtLineEditFactory(this);
    d_ptr->m_factoryToType[d_ptr->m_lineEditFactory] = QMetaType::QString;
    d_ptr->m_typeToFactory[QMetaType::QString] = d_ptr->m_lineEditFactory;

    d_ptr->m_dateEditFactory = new QtDateEditFactory(this);
    d_ptr->m_factoryToType[d_ptr->m_dateEditFactory] = QMetaType::QDate;
    d_ptr->m_typeToFactory[QMetaType::QDate] = d_ptr->m_dateEditFactory;

    d_ptr->m_timeEditFactory = new QtTimeEditFactory(this);
    d_ptr->m_factoryToType[d_ptr->m_timeEditFactory] = QMetaType::QTime;
    d_ptr->m_typeToFactory[QMetaType::QTime] = d_ptr->m_timeEditFactory;

    d_ptr->m_dateTimeEditFactory = new QtDateTimeEditFactory(this);
    d_ptr->m_factoryToType[d_ptr->m_dateTimeEditFactory] = QMetaType::QDateTime;
    d_ptr->m_typeToFactory[QMetaType::QDateTime] = d_ptr->m_dateTimeEditFactory;

    d_ptr->m_keySequenceEditorFactory = new QtKeySequenceEditorFactory(this);
    d_ptr->m_factoryToType[d_ptr->m_keySequenceEditorFactory] = QMetaType::QKeySequence;
    d_ptr->m_typeToFactory[QMetaType::QKeySequence] = d_ptr->m_keySequenceEditorFactory;

    d_ptr->m_charEditorFactory = new QtCharEditorFactory(this);
    d_ptr->m_factoryToType[d_ptr->m_charEditorFactory] = QMetaType::QChar;
    d_ptr->m_typeToFactory[QMetaType::QChar] = d_ptr->m_charEditorFactory;

    d_ptr->m_cursorEditorFactory = new QtCursorEditorFactory(this);
    d_ptr->m_factoryToType[d_ptr->m_cursorEditorFactory] = QMetaType::QCursor;
    d_ptr->m_typeToFactory[QMetaType::QCursor] = d_ptr->m_cursorEditorFactory;

    d_ptr->m_colorEditorFactory = new QtColorEditorFactory(this);
    d_ptr->m_factoryToType[d_ptr->m_colorEditorFactory] = QMetaType::QColor;
    d_ptr->m_typeToFactory[QMetaType::QColor] = d_ptr->m_colorEditorFactory;

    d_ptr->m_fontEditorFactory = new QtFontEditorFactory(this);
    d_ptr->m_factoryToType[d_ptr->m_fontEditorFactory] = QMetaType::QFont;
    d_ptr->m_typeToFactory[QMetaType::QFont] = d_ptr->m_fontEditorFactory;

    d_ptr->m_comboBoxFactory = new QtEnumEditorFactory(this);
    const int enumId = QtVariantPropertyManager::enumTypeId();
    d_ptr->m_factoryToType[d_ptr->m_comboBoxFactory] = enumId;
    d_ptr->m_typeToFactory[enumId] = d_ptr->m_comboBoxFactory;
}

/*!
    Destroys this factory, and all the widgets it has created.
*/
QtVariantEditorFactory::~QtVariantEditorFactory()
{
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtVariantEditorFactory::connectPropertyManager(QtVariantPropertyManager *manager)
{
    const auto intPropertyManagers = manager->findChildren<QtIntPropertyManager *>();
    for (QtIntPropertyManager *manager : intPropertyManagers)
        d_ptr->m_spinBoxFactory->addPropertyManager(manager);

    const auto doublePropertyManagers = manager->findChildren<QtDoublePropertyManager *>();
    for (QtDoublePropertyManager *manager : doublePropertyManagers)
        d_ptr->m_doubleSpinBoxFactory->addPropertyManager(manager);

    const auto boolPropertyManagers = manager->findChildren<QtBoolPropertyManager *>();
    for (QtBoolPropertyManager *manager : boolPropertyManagers)
        d_ptr->m_checkBoxFactory->addPropertyManager(manager);

    const auto stringPropertyManagers = manager->findChildren<QtStringPropertyManager *>();
    for (QtStringPropertyManager *manager : stringPropertyManagers)
        d_ptr->m_lineEditFactory->addPropertyManager(manager);

    const auto datePropertyManagers = manager->findChildren<QtDatePropertyManager *>();
    for (QtDatePropertyManager *manager : datePropertyManagers)
        d_ptr->m_dateEditFactory->addPropertyManager(manager);

    const auto timePropertyManagers = manager->findChildren<QtTimePropertyManager *>();
    for (QtTimePropertyManager *manager : timePropertyManagers)
        d_ptr->m_timeEditFactory->addPropertyManager(manager);

    const auto dateTimePropertyManagers = manager->findChildren<QtDateTimePropertyManager *>();
    for (QtDateTimePropertyManager *manager : dateTimePropertyManagers)
        d_ptr->m_dateTimeEditFactory->addPropertyManager(manager);

    const auto keySequencePropertyManagers = manager->findChildren<QtKeySequencePropertyManager *>();
    for (QtKeySequencePropertyManager *manager : keySequencePropertyManagers)
        d_ptr->m_keySequenceEditorFactory->addPropertyManager(manager);

    const auto charPropertyManagers = manager->findChildren<QtCharPropertyManager *>();
    for (QtCharPropertyManager *manager : charPropertyManagers)
        d_ptr->m_charEditorFactory->addPropertyManager(manager);

    const auto localePropertyManagers = manager->findChildren<QtLocalePropertyManager *>();
    for (QtLocalePropertyManager *manager : localePropertyManagers)
        d_ptr->m_comboBoxFactory->addPropertyManager(manager->subEnumPropertyManager());

    const auto pointPropertyManagers = manager->findChildren<QtPointPropertyManager *>();
    for (QtPointPropertyManager *manager : pointPropertyManagers)
        d_ptr->m_spinBoxFactory->addPropertyManager(manager->subIntPropertyManager());

    const auto pointFPropertyManagers = manager->findChildren<QtPointFPropertyManager *>();
    for (QtPointFPropertyManager *manager : pointFPropertyManagers)
        d_ptr->m_doubleSpinBoxFactory->addPropertyManager(manager->subDoublePropertyManager());

    const auto sizePropertyManagers = manager->findChildren<QtSizePropertyManager *>();
    for (QtSizePropertyManager *manager : sizePropertyManagers)
        d_ptr->m_spinBoxFactory->addPropertyManager(manager->subIntPropertyManager());

    const auto sizeFPropertyManagers = manager->findChildren<QtSizeFPropertyManager *>();
    for (QtSizeFPropertyManager *manager : sizeFPropertyManagers)
        d_ptr->m_doubleSpinBoxFactory->addPropertyManager(manager->subDoublePropertyManager());

    const auto rectPropertyManagers = manager->findChildren<QtRectPropertyManager *>();
    for (QtRectPropertyManager *manager : rectPropertyManagers)
        d_ptr->m_spinBoxFactory->addPropertyManager(manager->subIntPropertyManager());

    const auto rectFPropertyManagers = manager->findChildren<QtRectFPropertyManager *>();
    for (QtRectFPropertyManager *manager : rectFPropertyManagers)
        d_ptr->m_doubleSpinBoxFactory->addPropertyManager(manager->subDoublePropertyManager());

    const auto colorPropertyManagers = manager->findChildren<QtColorPropertyManager *>();
    for (QtColorPropertyManager *manager : colorPropertyManagers) {
        d_ptr->m_colorEditorFactory->addPropertyManager(manager);
        d_ptr->m_spinBoxFactory->addPropertyManager(manager->subIntPropertyManager());
    }

    const auto enumPropertyManagers = manager->findChildren<QtEnumPropertyManager *>();
    for (QtEnumPropertyManager *manager : enumPropertyManagers)
        d_ptr->m_comboBoxFactory->addPropertyManager(manager);

    const auto sizePolicyPropertyManagers = manager->findChildren<QtSizePolicyPropertyManager *>();
    for (QtSizePolicyPropertyManager *manager : sizePolicyPropertyManagers) {
        d_ptr->m_spinBoxFactory->addPropertyManager(manager->subIntPropertyManager());
        d_ptr->m_comboBoxFactory->addPropertyManager(manager->subEnumPropertyManager());
    }

    const auto fontPropertyManagers = manager->findChildren<QtFontPropertyManager *>();
    for (QtFontPropertyManager *manager : fontPropertyManagers) {
        d_ptr->m_fontEditorFactory->addPropertyManager(manager);
        d_ptr->m_spinBoxFactory->addPropertyManager(manager->subIntPropertyManager());
        d_ptr->m_comboBoxFactory->addPropertyManager(manager->subEnumPropertyManager());
        d_ptr->m_checkBoxFactory->addPropertyManager(manager->subBoolPropertyManager());
    }

    const auto cursorPropertyManagers = manager->findChildren<QtCursorPropertyManager *>();
    for (QtCursorPropertyManager *manager : cursorPropertyManagers)
        d_ptr->m_cursorEditorFactory->addPropertyManager(manager);

    const auto flagPropertyManagers = manager->findChildren<QtFlagPropertyManager *>();
    for (QtFlagPropertyManager *manager : flagPropertyManagers)
        d_ptr->m_checkBoxFactory->addPropertyManager(manager->subBoolPropertyManager());
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
QWidget *QtVariantEditorFactory::createEditor(QtVariantPropertyManager *manager, QtProperty *property,
        QWidget *parent)
{
    const int propType = manager->propertyType(property);
    QtAbstractEditorFactoryBase *factory = d_ptr->m_typeToFactory.value(propType, 0);
    if (!factory)
        return 0;
    return factory->createEditor(wrappedProperty(property), parent);
}

/*!
    \internal

    Reimplemented from the QtAbstractEditorFactory class.
*/
void QtVariantEditorFactory::disconnectPropertyManager(QtVariantPropertyManager *manager)
{
    const auto intPropertyManagers = manager->findChildren<QtIntPropertyManager *>();
    for (QtIntPropertyManager *manager : intPropertyManagers)
        d_ptr->m_spinBoxFactory->removePropertyManager(manager);

    const auto doublePropertyManagers = manager->findChildren<QtDoublePropertyManager *>();
    for (QtDoublePropertyManager *manager : doublePropertyManagers)
        d_ptr->m_doubleSpinBoxFactory->removePropertyManager(manager);

    const auto boolPropertyManagers = manager->findChildren<QtBoolPropertyManager *>();
    for (QtBoolPropertyManager *manager : boolPropertyManagers)
        d_ptr->m_checkBoxFactory->removePropertyManager(manager);

    const auto stringPropertyManagers = manager->findChildren<QtStringPropertyManager *>();
    for (QtStringPropertyManager *manager : stringPropertyManagers)
        d_ptr->m_lineEditFactory->removePropertyManager(manager);

    const auto datePropertyManagers = manager->findChildren<QtDatePropertyManager *>();
    for (QtDatePropertyManager *manager : datePropertyManagers)
        d_ptr->m_dateEditFactory->removePropertyManager(manager);

    const auto timePropertyManagers = manager->findChildren<QtTimePropertyManager *>();
    for (QtTimePropertyManager *manager : timePropertyManagers)
        d_ptr->m_timeEditFactory->removePropertyManager(manager);

    const auto dateTimePropertyManagers = manager->findChildren<QtDateTimePropertyManager *>();
    for (QtDateTimePropertyManager *manager : dateTimePropertyManagers)
        d_ptr->m_dateTimeEditFactory->removePropertyManager(manager);

    const auto keySequencePropertyManagers = manager->findChildren<QtKeySequencePropertyManager *>();
    for (QtKeySequencePropertyManager *manager : keySequencePropertyManagers)
        d_ptr->m_keySequenceEditorFactory->removePropertyManager(manager);

    const auto charPropertyManagers = manager->findChildren<QtCharPropertyManager *>();
    for (QtCharPropertyManager *manager : charPropertyManagers)
        d_ptr->m_charEditorFactory->removePropertyManager(manager);

    const auto localePropertyManagers = manager->findChildren<QtLocalePropertyManager *>();
    for (QtLocalePropertyManager *manager : localePropertyManagers)
        d_ptr->m_comboBoxFactory->removePropertyManager(manager->subEnumPropertyManager());

    const auto pointPropertyManagers = manager->findChildren<QtPointPropertyManager *>();
    for (QtPointPropertyManager *manager : pointPropertyManagers)
        d_ptr->m_spinBoxFactory->removePropertyManager(manager->subIntPropertyManager());

    const auto pointFPropertyManagers = manager->findChildren<QtPointFPropertyManager *>();
    for (QtPointFPropertyManager *manager : pointFPropertyManagers)
        d_ptr->m_doubleSpinBoxFactory->removePropertyManager(manager->subDoublePropertyManager());

    const auto sizePropertyManagers = manager->findChildren<QtSizePropertyManager *>();
    for (QtSizePropertyManager *manager : sizePropertyManagers)
        d_ptr->m_spinBoxFactory->removePropertyManager(manager->subIntPropertyManager());

    const auto sizeFPropertyManagers = manager->findChildren<QtSizeFPropertyManager *>();
    for (QtSizeFPropertyManager *manager : sizeFPropertyManagers)
        d_ptr->m_doubleSpinBoxFactory->removePropertyManager(manager->subDoublePropertyManager());

    const auto rectPropertyManagers = manager->findChildren<QtRectPropertyManager *>();
    for (QtRectPropertyManager *manager : rectPropertyManagers)
        d_ptr->m_spinBoxFactory->removePropertyManager(manager->subIntPropertyManager());

    const auto rectFPropertyManagers = manager->findChildren<QtRectFPropertyManager *>();
    for (QtRectFPropertyManager *manager : rectFPropertyManagers)
        d_ptr->m_doubleSpinBoxFactory->removePropertyManager(manager->subDoublePropertyManager());

    const auto colorPropertyManagers = manager->findChildren<QtColorPropertyManager *>();
    for (QtColorPropertyManager *manager : colorPropertyManagers) {
        d_ptr->m_colorEditorFactory->removePropertyManager(manager);
        d_ptr->m_spinBoxFactory->removePropertyManager(manager->subIntPropertyManager());
    }

    const auto enumPropertyManagers = manager->findChildren<QtEnumPropertyManager *>();
    for (QtEnumPropertyManager *manager : enumPropertyManagers)
        d_ptr->m_comboBoxFactory->removePropertyManager(manager);

    const auto sizePolicyPropertyManagers = manager->findChildren<QtSizePolicyPropertyManager *>();
    for (QtSizePolicyPropertyManager *manager : sizePolicyPropertyManagers) {
        d_ptr->m_spinBoxFactory->removePropertyManager(manager->subIntPropertyManager());
        d_ptr->m_comboBoxFactory->removePropertyManager(manager->subEnumPropertyManager());
    }

    const auto fontPropertyManagers = manager->findChildren<QtFontPropertyManager *>();
    for (QtFontPropertyManager *manager : fontPropertyManagers) {
        d_ptr->m_fontEditorFactory->removePropertyManager(manager);
        d_ptr->m_spinBoxFactory->removePropertyManager(manager->subIntPropertyManager());
        d_ptr->m_comboBoxFactory->removePropertyManager(manager->subEnumPropertyManager());
        d_ptr->m_checkBoxFactory->removePropertyManager(manager->subBoolPropertyManager());
    }

    const auto cursorPropertyManagers = manager->findChildren<QtCursorPropertyManager *>();
    for (QtCursorPropertyManager *manager : cursorPropertyManagers)
        d_ptr->m_cursorEditorFactory->removePropertyManager(manager);

    const auto flagPropertyManagers = manager->findChildren<QtFlagPropertyManager *>();
    for (QtFlagPropertyManager *manager : flagPropertyManagers)
        d_ptr->m_checkBoxFactory->removePropertyManager(manager->subBoolPropertyManager());
}

QT_END_NAMESPACE

#include "moc_qtvariantproperty.cpp"
