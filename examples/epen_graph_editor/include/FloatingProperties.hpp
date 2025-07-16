#ifndef FLOATINGPROPERTIES_HPP
#define FLOATINGPROPERTIES_HPP

#include "FloatingPanelBase.hpp"
#include <QDate>
#include <QLocale>
#include <QLineEdit>
#include "qtpropertymanager.h"
#include "qtvariantproperty.h"
#include "qttreepropertybrowser.h"
#include <QtNodes/DataFlowGraphModel>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

using QtNodes::NodeId;
using QtNodes::InvalidNodeId;

class FloatingProperties : public FloatingPanelBase
{
    Q_OBJECT

public:
    explicit FloatingProperties(GraphEditorWindow *parent = nullptr);
    ~FloatingProperties();

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
    QtTreePropertyBrowser* getPropertyWidget();
    
    // Properties layout
    QVBoxLayout *m_propertiesLayout;
    
    // Current node
    NodeId m_currentNodeId;
    
    // Property widgets
    QList<QWidget*> m_propertyWidgets;
    QtTreePropertyBrowser *_properties;
};

#endif // FLOATINGPROPERTIES_HPP