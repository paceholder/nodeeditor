#include "ObjectPropertyBrowserNew.hpp"
#include <QDebug>
#include <QMetaType>
#include <QMetaEnum>

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
    
    // Disconnect all connections
    for (auto conn : activeConnections) {
        disconnect(conn);
    }
    activeConnections.clear();
    
    currentlyConnectedObject = obj;
    if (!obj)
        return;

    // Start from 1 to skip the objectName property inherited from QObject
    for (int i = 1; i < obj->metaObject()->propertyCount(); i++) {
        QMetaProperty mp = obj->metaObject()->property(i);
        QtVariantProperty *property = createPropertyForMetaProperty(obj, mp, "");
        if (property) {
            addProperty(property);
        }
    }
    
    // Connect the main object's propertyChanged signal
    QMetaObject::Connection conn = connect(obj, SIGNAL(propertyChanged()), this, SLOT(objectUpdated()));
    if (conn) {
        activeConnections.append(conn);
    }
    
    objectUpdated();
}

QtVariantProperty* ObjectPropertyBrowser::createPropertyForMetaProperty(QObject *parentObj, const QMetaProperty &mp, const QString &parentPath)
{
    QVariant value = mp.read(parentObj);
    
    // Check if this is an enum property
    if (mp.isEnumType()) {
        // Create enum property as a combobox
        QtVariantProperty *property = variantManager->addProperty(QtVariantPropertyManager::enumTypeId(), setupName(mp.name()));
        property->setEnabled(mp.isWritable());
        
        // Get the enum metadata
        QMetaEnum metaEnum = mp.enumerator();
        
        // Build list of enum names
        QStringList enumNames;
        for (int i = 0; i < metaEnum.keyCount(); i++) {
            enumNames << QString::fromLatin1(metaEnum.key(i));
        }
        
        // Set the enum names for the combobox
        property->setAttribute("enumNames", enumNames);
        
        // Build the property path
        QString path = parentPath.isEmpty() ? QString(mp.name()) : QString("%1.%2").arg(parentPath).arg(mp.name());
        propertyMap[property] = path;
        
        return property;
    }
    
    // More robust check for QObject pointers
    bool isQObjectPointer = false;
    QObject *subObj = nullptr;
    
    // Method 1: Direct QObject* check
    if (value.canConvert<QObject*>()) {
        subObj = value.value<QObject*>();
        if (subObj) {
            isQObjectPointer = true;
        }
    }
    
    // Method 2: Check if it's registered as QObjectStar
    if (!isQObjectPointer && mp.type() == QMetaType::QObjectStar) {
        subObj = value.value<QObject*>();
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
                isQObjectPointer = true;
            }
        }
    }
    
    if (isQObjectPointer && subObj) {

        // Create a group property for the QObject
        QtVariantProperty *groupProperty = variantManager->addProperty(
            QtVariantPropertyManager::groupTypeId(), 
            setupName(mp.name())
        );
        
        // Build the property path
        QString path = parentPath.isEmpty() ? QString(mp.name()) : QString("%1.%2").arg(parentPath).arg(mp.name());
        propertyMap[groupProperty] = path;
        
        // Connect the sub-object's propertyChanged signal if it exists
        int propertyChangedIndex = subObj->metaObject()->indexOfSignal("propertyChanged()");
        if (propertyChangedIndex != -1) {
            QMetaObject::Connection conn = connect(subObj, SIGNAL(propertyChanged()), this, SLOT(objectUpdated()));
            if (conn) {
                activeConnections.append(conn);
            }
        }
        
        // Recursively add sub-properties
        addQObjectProperties(subObj, groupProperty, path);
        
        return groupProperty;
    }
    
    // Handle regular properties
    int typeId = mp.type();
    if (typeId == QMetaType::UnknownType) {
        typeId = QMetaType::type(mp.typeName());
        if (typeId == QMetaType::UnknownType) {
            return nullptr;
        }
    }
    
    
    QtVariantProperty *property = variantManager->addProperty(typeId, setupName(mp.name()));
    property->setEnabled(mp.isWritable());
    
    // Build the property path
    QString path = parentPath.isEmpty() ? QString(mp.name()) : QString("%1.%2").arg(parentPath).arg(mp.name());
    propertyMap[property] = path;
    
    return property;
}

void ObjectPropertyBrowser::addQObjectProperties(QObject *obj, QtVariantProperty *parentProperty, const QString &parentPath)
{
    if (!obj || !parentProperty)
        return;
    
    const QMetaObject *metaObj = obj->metaObject();
    
    // Start from 1 to skip objectName
    for (int i = 1; i < metaObj->propertyCount(); i++) {
        QMetaProperty mp = metaObj->property(i);
        QtVariantProperty *subProperty = createPropertyForMetaProperty(obj, mp, parentPath);
        if (subProperty) {
            parentProperty->addSubProperty(subProperty);
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
            if (!obj) {
                qDebug() << "Failed to navigate to nested object at:" << parts[i];
                return;
            }
        }
        
        // Get the property name
        QString propName = parts.last();
        
        // Check if this is an enum property
        const QMetaObject *metaObj = obj->metaObject();
        int propIndex = metaObj->indexOfProperty(propName.toLatin1());
        if (propIndex != -1) {
            QMetaProperty mp = metaObj->property(propIndex);
            if (mp.isEnumType()) {
                // Convert enum index back to enum value
                QMetaEnum metaEnum = mp.enumerator();
                int enumValue = metaEnum.value(value.toInt());
                bool success = obj->setProperty(propName.toLatin1(), enumValue);
                if (!success) {
                    qDebug() << "Failed to set enum property:" << propName << "on object:" << obj;
                }
                return;
            }
        }
        
        // Set the final property (non-enum)
        bool success = obj->setProperty(propName.toLatin1(), value);
        if (!success) {
            qDebug() << "Failed to set property:" << propName << "on object:" << obj;
        }
    } else {
        // Regular property
        // Check if this is an enum property
        const QMetaObject *metaObj = currentlyConnectedObject->metaObject();
        int propIndex = metaObj->indexOfProperty(propertyPath.toLatin1());
        if (propIndex != -1) {
            QMetaProperty mp = metaObj->property(propIndex);
            if (mp.isEnumType()) {
                // Convert enum index back to enum value
                QMetaEnum metaEnum = mp.enumerator();
                int enumValue = metaEnum.value(value.toInt());
                bool success = currentlyConnectedObject->setProperty(propertyPath.toLatin1(), enumValue);
                if (!success) {
                    qDebug() << "Failed to set enum property:" << propertyPath;
                }
                return;
            }
        }
        
        bool success = currentlyConnectedObject->setProperty(propertyPath.toLatin1(), value);
        if (!success) {
            qDebug() << "Failed to set property:" << propertyPath;
        }
    }
    
    // Don't call objectUpdated() here - let the propertyChanged signal handle it
}

void ObjectPropertyBrowser::objectUpdated()
{
    // Temporarily disconnect to avoid recursive updates
    disconnect(variantManager,
               SIGNAL(valueChanged(QtProperty *, QVariant)),
               this,
               SLOT(valueChanged(QtProperty *, QVariant)));
    
    updatePropertyValues(currentlyConnectedObject);
    
    // Reconnect
    connect(variantManager,
            SIGNAL(valueChanged(QtProperty *, QVariant)),
            this,
            SLOT(valueChanged(QtProperty *, QVariant)));
}

void ObjectPropertyBrowser::updatePropertyValues(QObject *rootObj)
{
    QMapIterator<QtProperty *, QString> i(propertyMap);
    while (i.hasNext()) {
        i.next();
        QtProperty *property = i.key();
        QString propertyPath = i.value();
        
        // Skip group properties (they don't have values)
        QtVariantProperty *varProp = variantManager->variantProperty(property);
        if (varProp && varProp->propertyType() == QtVariantPropertyManager::groupTypeId()) {
            continue;
        }
        
        if (propertyPath.contains('.')) {
            // Handle nested property
            QStringList parts = propertyPath.split('.');
            QObject *obj = rootObj;
            
            // Navigate to the object
            for (int j = 0; j < parts.size() - 1; j++) {
                QVariant var = obj->property(parts[j].toLatin1());
                obj = var.value<QObject*>();
                if (!obj) {
                    qDebug() << "Failed to navigate to nested object for update at:" << parts[j];
                    break;
                }
            }
            
            if (obj) {
                QString propName = parts.last();
                QVariant value = obj->property(propName.toLatin1());
                
                // Check if this is an enum property
                const QMetaObject *metaObj = obj->metaObject();
                int propIndex = metaObj->indexOfProperty(propName.toLatin1());
                if (propIndex != -1) {
                    QMetaProperty mp = metaObj->property(propIndex);
                    if (mp.isEnumType() && varProp) {
                        // Convert enum value to index for the combobox
                        QMetaEnum metaEnum = mp.enumerator();
                        int enumIndex = metaEnum.keyToValue(metaEnum.valueToKey(value.toInt()));
                        // Find the index in the enum
                        for (int k = 0; k < metaEnum.keyCount(); k++) {
                            if (metaEnum.value(k) == value.toInt()) {
                                variantManager->setValue(property, k);
                                break;
                            }
                        }
                        continue;
                    }
                }
                
                variantManager->setValue(property, value);
            }
        } else {
            // Regular property
            QVariant value = rootObj->property(propertyPath.toLatin1());
            
            // Check if this is an enum property
            const QMetaObject *metaObj = rootObj->metaObject();
            int propIndex = metaObj->indexOfProperty(propertyPath.toLatin1());
            if (propIndex != -1) {
                QMetaProperty mp = metaObj->property(propIndex);
                if (mp.isEnumType() && varProp) {
                    // Convert enum value to index for the combobox
                    QMetaEnum metaEnum = mp.enumerator();
                    // Find the index in the enum
                    for (int k = 0; k < metaEnum.keyCount(); k++) {
                        if (metaEnum.value(k) == value.toInt()) {
                            variantManager->setValue(property, k);
                            break;
                        }
                    }
                    continue;
                }
            }
            
            variantManager->setValue(property, value);
        }
    }
}

QString ObjectPropertyBrowser::setupName(QString name)
{
    name.replace('_', ' ');
    return name;
}