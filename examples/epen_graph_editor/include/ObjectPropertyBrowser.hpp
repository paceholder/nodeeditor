#ifndef OBJECTPROPERTYBROWSER_H
#define OBJECTPROPERTYBROWSER_H

#include "data_models/OperationDataModel.hpp"
#include "qttreepropertybrowser_p.h"
#include "qtvariantproperty_p.h"
#include <QMap>
#include <QMetaProperty>
#include <QObject>
#include <QPushButton>

class ObjectPropertyBrowser : public QtTreePropertyBrowser
{
    Q_OBJECT

public:
    ObjectPropertyBrowser(QWidget *parent);
    void setActiveObject(OperationDataModel *obj);

private:
    QString setupName(QString name);
    QtVariantPropertyManager *variantManager;
    OperationDataModel *currentlyConnectedObject = nullptr;
    QMap<QtProperty *, const char *> propertyMap;

private slots:
    void valueChanged(QtProperty *property, const QVariant &value);

public slots:
    void objectUpdated();
};

#endif // OBJECTPROPERTYBROWSER_H
