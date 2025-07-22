#include "ObjectPropertyBrowserNew.hpp"
#include <QDebug>
#include <QMetaType>

ObjectPropertyBrowser::ObjectPropertyBrowser(QWidget *parent)
    : QtTreePropertyBrowser(parent)
{
    variantManager = new QtVariantPropertyManager(this);
    setFactoryForManager(variantManager, new QtVariantEditorFactory);
}

void ObjectPropertyBrowser::setActiveObject(QObject *obj)
{
    clear();
    variantManager->clear();
    propertyMap.clear();
    if (currentlyConnectedObject)
        currentlyConnectedObject->disconnect(this);
    currentlyConnectedObject = obj;
    if (!obj)
        return;

    // Start from 1 to skip the objectName property inherited from QObject
    for (int i = 1; i < obj->metaObject()->propertyCount(); i++) {
        QMetaProperty mp = obj->metaObject()->property(i);
        QtVariantProperty *property = createPropertyForMetaProperty(obj, mp);
        if (property) {
            addProperty(property);
        }
    }
    
    connect(obj, SIGNAL(propertyChanged()), this, SLOT(objectUpdated()));
    objectUpdated();
}

QtVariantProperty* ObjectPropertyBrowser::createPropertyForMetaProperty(QObject *parentObj, const QMetaProperty &mp)
{
    QVariant value = mp.read(parentObj);
    
    // Debug output
    qDebug() << "Property:" << mp.name() 
             << "Type:" << mp.typeName() 
             << "TypeId:" << mp.type()
             << "Value type:" << value.typeName()
             << "Value userType:" << value.userType();
    
    // More robust check for QObject pointers
    bool isQObjectPointer = false;
    QObject *subObj = nullptr;
    
    // Method 1: Direct QObject* check
    if (value.canConvert<QObject*>()) {
        subObj = value.value<QObject*>();
        if (subObj) {
            qDebug() << "  -> Detected as QObject* via canConvert";
            isQObjectPointer = true;
        }
    }
    
    // Method 2: Check if it's registered as QObjectStar
    if (!isQObjectPointer && mp.type() == QMetaType::QObjectStar) {
        subObj = value.value<QObject*>();
        qDebug() << "  -> Detected as QMetaType::QObjectStar";
        isQObjectPointer = true;
    }
    
    // Method 3: Manual type name check
    if (!isQObjectPointer && mp.typeName()) {
        QString typeName = QString(mp.typeName());
        if (typeName.endsWith('*')) {
            // Try various extraction methods
            subObj = value.value<QObject*>();
            if (!subObj) {
                // Try with QVariant's data
                void *ptr = value.data();
                if (ptr) {
                    subObj = *static_cast<QObject**>(ptr);
                }
            }
            if (subObj) {
                qDebug() << "  -> Detected via type name:" << typeName;
                isQObjectPointer = true;
            }
        }
    }
    
    if (isQObjectPointer && subObj) {
        qDebug() << "  -> Creating group for QObject:" << subObj->metaObject()->className();
        
        // Create a group property for the QObject
        QtVariantProperty *groupProperty = variantManager->addProperty(
            QtVariantPropertyManager::groupTypeId(), 
            setupName(mp.name())
        );
        
        // Add the property to our map
        propertyMap[groupProperty] = mp.name();
        
        // Recursively add sub-properties
        addQObjectProperties(subObj, groupProperty);
        
        return groupProperty;
    }
    
    // Handle regular properties
    int typeId = mp.type();
    if (typeId == QMetaType::UnknownType) {
        typeId = QMetaType::type(mp.typeName());
        if (typeId == QMetaType::UnknownType) {
            qDebug() << "  -> Skipping unknown type";
            return nullptr;
        }
    }
    
    qDebug() << "  -> Creating regular property with typeId:" << typeId;
    
    QtVariantProperty *property = variantManager->addProperty(typeId, setupName(mp.name()));
    property->setEnabled(mp.isWritable());
    propertyMap[property] = mp.name();
    
    return property;
}

void ObjectPropertyBrowser::addQObjectProperties(QObject *obj, QtVariantProperty *parentProperty)
{
    if (!obj || !parentProperty)
        return;
    
    const QMetaObject *metaObj = obj->metaObject();
    
    // Start from 1 to skip objectName
    for (int i = 1; i < metaObj->propertyCount(); i++) {
        QMetaProperty mp = metaObj->property(i);
        QVariant value = mp.read(obj);
        
        // Check if this sub-property is also a QObject
        if (mp.type() == QMetaType::QObjectStar || 
            (mp.typeName() && QString(mp.typeName()).endsWith('*'))) {
            
            QObject *subObj = value.value<QObject*>();
            if (subObj) {
                // Create a sub-group for nested QObject
                QtVariantProperty *subGroup = variantManager->addProperty(
                    QtVariantPropertyManager::groupTypeId(), 
                    setupName(mp.name())
                );
                parentProperty->addSubProperty(subGroup);
                
                // Store the property path for nested objects
                QString path = QString("%1.%2").arg(propertyMap[parentProperty]).arg(mp.name());
                propertyMap[subGroup] = path.toLatin1().data();
                
                // Recursively add its properties
                addQObjectProperties(subObj, subGroup);
            }
        } else {
            // Regular property
            int typeId = mp.type();
            if (typeId == QMetaType::UnknownType) {
                typeId = QMetaType::type(mp.typeName());
                if (typeId == QMetaType::UnknownType) {
                    continue; // Skip unknown types
                }
            }
            
            QtVariantProperty *subProperty = variantManager->addProperty(typeId, setupName(mp.name()));
            subProperty->setEnabled(mp.isWritable());
            parentProperty->addSubProperty(subProperty);
            
            // Store the property path
            QString path = QString("%1.%2").arg(propertyMap[parentProperty]).arg(mp.name());
            propertyMap[subProperty] = path.toLatin1().data();
        }
    }
}

void ObjectPropertyBrowser::valueChanged(QtProperty *property, const QVariant &value)
{
    QString propertyPath = propertyMap[property];
    
    if (propertyPath.contains('.')) {
        // Handle nested property
        QStringList parts = propertyPath.split('.');
        QObject *obj = currentlyConnectedObject;
        
        // Navigate to the parent object
        for (int i = 0; i < parts.size() - 1; i++) {
            QVariant var = obj->property(parts[i].toLatin1());
            obj = var.value<QObject*>();
            if (!obj) return;
        }
        
        // Set the final property
        obj->setProperty(parts.last().toLatin1(), value);
    } else {
        // Regular property
        currentlyConnectedObject->setProperty(propertyPath.toLatin1(), value);
    }
    
    objectUpdated();
}

void ObjectPropertyBrowser::objectUpdated()
{
    disconnect(variantManager,
               SIGNAL(valueChanged(QtProperty *, QVariant)),
               this,
               SLOT(valueChanged(QtProperty *, QVariant)));
    
    updatePropertyValues(currentlyConnectedObject);
    
    connect(variantManager,
            SIGNAL(valueChanged(QtProperty *, QVariant)),
            this,
            SLOT(valueChanged(QtProperty *, QVariant)));
}

void ObjectPropertyBrowser::updatePropertyValues(QObject *rootObj)
{
    QMapIterator<QtProperty *, const char *> i(propertyMap);
    while (i.hasNext()) {
        i.next();
        QtProperty *property = i.key();
        QString propertyPath = i.value();
        
        if (propertyPath.contains('.')) {
            // Handle nested property
            QStringList parts = propertyPath.split('.');
            QObject *obj = rootObj;
            
            // Navigate to the object
            for (int j = 0; j < parts.size() - 1; j++) {
                QVariant var = obj->property(parts[j].toLatin1());
                obj = var.value<QObject*>();
                if (!obj) break;
            }
            
            if (obj) {
                QVariant value = obj->property(parts.last().toLatin1());
                variantManager->setValue(property, value);
            }
        } else {
            // Regular property
            variantManager->setValue(property, rootObj->property(propertyPath.toLatin1()));
        }
    }
}

QString ObjectPropertyBrowser::setupName(QString name)
{
    name.replace('_', ' ');
    return name;
}