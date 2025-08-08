#ifndef OBJECTPROPERTYBROWSER_HPP
#define OBJECTPROPERTYBROWSER_HPP

#include "data_models/OperationDataModel.hpp"
#include "qttreepropertybrowser_p.h"
#include "qtvariantproperty_p.h"
#include <QObject>
#include <QMap>
#include <QMetaProperty>

class ObjectPropertyBrowser : public QtTreePropertyBrowser
{
    Q_OBJECT

public:
    explicit ObjectPropertyBrowser(QWidget *parent = nullptr);
    void setActiveObject(QObject *obj);  // Changed from OperationDataModel* to QObject*

private slots:
    void valueChanged(QtProperty *property, const QVariant &value);
    void objectUpdated();

private:
    QtVariantProperty* createPropertyForMetaProperty(QObject *parentObj, const QMetaProperty &mp, const QString &parentPath = "");
    void addQObjectProperties(QObject *obj, QtVariantProperty *parentProperty, const QString &parentPath);
    void updatePropertyValues(QObject *rootObj);
    QString setupName(QString name);

    QtVariantPropertyManager *variantManager;
    QMap<QtProperty*, QString> propertyMap;  // Changed from const char* to QString
    QObject *currentlyConnectedObject = nullptr;
    QList<QMetaObject::Connection> activeConnections;  // Track all active connections
};

#endif // OBJECTPROPERTYBROWSER_HPP