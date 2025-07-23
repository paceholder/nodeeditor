#ifndef FLOATINGPROPERTIES_HPP
#define FLOATINGPROPERTIES_HPP

#include "FloatingPanelBase.hpp"
#include "ObjectPropertyBrowserNew.hpp"
#include <QDate>
#include <QLineEdit>
#include <QLocale>


QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE



class FloatingProperties : public FloatingPanelBase
{
    Q_OBJECT

public:
    explicit FloatingProperties(GraphEditorWindow *parent = nullptr);
    ~FloatingProperties();

    void setObject(QObject *obj);
    void unsetObject();

public slots:
    void clearProperties();

protected:
    // Implement virtual functions from base class
    void setupUI() override;
    void connectSignals() override;
    void resizeEvent(QResizeEvent *event) override;

private:
    ObjectPropertyBrowser *_properties;
    QtVariantPropertyManager *_variantManager;
    QtVariantEditorFactory *_variantFactory;
    QMap<QtProperty *, const char *> propertyMap;
};

#endif // FLOATINGPROPERTIES_HPP