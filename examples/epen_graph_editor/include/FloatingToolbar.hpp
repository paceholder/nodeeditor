#ifndef FLOATING_TOOLBAR_HPP
#define FLOATING_TOOLBAR_HPP

#include "DraggableButton.hpp"
#include "ExpandableCategoryWidget.hpp"
#include "FloatingPanelBase.hpp"
#include <QMap>

class FloatingToolbar : public FloatingPanelBase
{
    Q_OBJECT

public:
    explicit FloatingToolbar(GraphEditorWindow *parent = nullptr);
    ~FloatingToolbar() = default;

signals:
    void specificNodeRequested(QString actionName);
    void fillColorChanged(const QColor &color);
    void zoomInRequested();
    void zoomOutRequested();
    void resetViewRequested();

protected:
    // Implement virtual functions from base class
    void setupUI() override;
    void connectSignals() override;

private:
    struct NodeButtonInfo
    {
        QString name;
        QString icon;
        QString fallback;
        QString tooltip;
        QString actionName;
        bool enabled;
    };

    DraggableButton* addNodeButton(QString name,
                       QString icon,
                       QString fallback,
                       QString tooltip,
                       bool enabled,
                       QString actionName
                       );
    QString createSafeButtonText(const QString &icon, const QString &text);
    DraggableButton *createNodeButton(const NodeButtonInfo &info, QWidget *parent = nullptr);

    void setupNodeCategories();
    void addNodeButtonsToCategory(ExpandableCategoryWidget *category,
                                  const QVector<NodeButtonInfo> &buttons);

    void addSeparator(QVBoxLayout *layout);

    QFont m_buttonFont;
};

#endif // FLOATING_TOOLBAR_HPP