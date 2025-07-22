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
            qDebug() << "  -> Skipping unknown type";
            return nullptr;
        }
    }
    
    qDebug() << "  -> Creating regular property with typeId:" << typeId;
    
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
        
        // Set the final property
        bool success = obj->setProperty(parts.last().toLatin1(), value);
        if (!success) {
            qDebug() << "Failed to set property:" << parts.last() << "on object:" << obj;
        }
    } else {
        // Regular property
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
                QVariant value = obj->property(parts.last().toLatin1());
                variantManager->setValue(property, value);
            }
        } else {
            // Regular property
            QVariant value = rootObj->property(propertyPath.toLatin1());
            variantManager->setValue(property, value);
        }
    }
}

QString ObjectPropertyBrowser::setupName(QString name)
{
    name.replace('_', ' ');
    return name;
}