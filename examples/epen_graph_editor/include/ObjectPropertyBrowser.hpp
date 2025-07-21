#ifndef OBJECTPROPERTYBROWSER_H
#define OBJECTPROPERTYBROWSER_H

#include <QObject>
#include <QPushButton>
#include <QMap>
#include <QMetaProperty>
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "qtpropertymanager.h"
#include "qtvariantproperty.h"
#include "data_models/OperationDataModel.hpp"

class ObjectPropertyBrowser : public QtTreePropertyBrowser
{
    Q_OBJECT

public:
    ObjectPropertyBrowser(QWidget* parent);
    void setActiveObject(OperationDataModel *obj);

private:
    QtVariantPropertyManager *variantManager;
    OperationDataModel *currentlyConnectedObject = nullptr;
    QMap<QtProperty *, const char*> propertyMap;

private slots:
    void valueChanged(QtProperty *property, const QVariant &value);

public slots:
    void objectUpdated();
};

#endif // OBJECTPROPERTYBROWSER_H
