#ifndef FLOATINGPROPERTIES_HPP
#define FLOATINGPROPERTIES_HPP

#include "FloatingPanelBase.hpp"
#include "data_models/OperationDataModel.hpp"
#include "ObjectPropertyBrowserNew.hpp"
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
    void unsetNode();
signals:
    void nodeSelected(int nodeId);
    void nodeDeselected();

public slots:
    void clearProperties();

protected:
    // Implement virtual functions from base class
    void setupUI() override;
    void connectSignals() override;
    void resizeEvent(QResizeEvent *event) override;

private:
    NodeId m_currentNodeId;

    OperationDataModel *_currentNode;
    ObjectPropertyBrowser *_properties;
    QtVariantPropertyManager *_variantManager;
    QtVariantEditorFactory *_variantFactory;
    QMap<QtProperty *, const char*> propertyMap;
};

#endif // FLOATINGPROPERTIES_HPP