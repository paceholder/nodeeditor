#ifndef FLOATINGPROPERTIES_HPP
#define FLOATINGPROPERTIES_HPP

#include "FloatingPanelBase.hpp"
#include "data_models/OperationDataModel.hpp"
#include "qtpropertymanager.h"
#include "qttreepropertybrowser.h"
#include "qtvariantproperty.h"
#include <QDate>
#include <QLineEdit>
#include <QLocale>
#include <QtNodes/DataFlowGraphModel>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

using QtNodes::InvalidNodeId;
using QtNodes::NodeId;

class FloatingProperties : public FloatingPanelBase
{
    Q_OBJECT

public:
    explicit FloatingProperties(GraphEditorWindow *parent = nullptr);
    ~FloatingProperties();

    void setNode(OperationDataModel *node);
signals:
    void propertyChanged(const QString &name, const QVariant &value);
    void nodeSelected(int nodeId);
    void nodeDeselected();
    

public slots:
    void updatePropertiesForNode(NodeId nodeId);
    void clearProperties();

protected:
    // Implement virtual functions from base class
    void setupUI() override;
    void connectSignals() override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void clearPropertyWidgets();
    void addPropertyWidget(const QString &label, QWidget *widget);
    QtTreePropertyBrowser *getPropertyWidget();

    // Properties layout
    QVBoxLayout *m_propertiesLayout;

    // Current node
    NodeId m_currentNodeId;

    // Property widgets
    QList<QWidget *> m_propertyWidgets;
    QtTreePropertyBrowser *_properties;
    QtVariantPropertyManager *_variantManager;
    QtVariantEditorFactory *_variantFactory;
};

#endif // FLOATINGPROPERTIES_HPP