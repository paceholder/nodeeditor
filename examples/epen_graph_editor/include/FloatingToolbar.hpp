#ifndef FLOATING_TOOLBAR_HPP
#define FLOATING_TOOLBAR_HPP

#include "DraggableButton.hpp"
#include "ExpandableCategoryWidget.hpp"
#include "FloatingPanelBase.hpp"
#include <QMap>
#include <QComboBox>
#include <QVector>

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

    struct SubCategory
    {
        QString name;
        QVector<NodeButtonInfo> buttons;
    };

    struct Category
    {
        QString name;
        QVector<SubCategory> subCategories;
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
    void onCategoryChanged(int index);
    void onSubCategoryChanged(int index);
    void updateNodeButtons();
    void clearNodeButtons();

    QFont m_buttonFont;
    
    // Combo boxes for category selection
    QComboBox *m_categoryCombo;
    QComboBox *m_subCategoryCombo;
    QWidget *m_nodeButtonContainer;
    QVBoxLayout *m_nodeButtonLayout;
    
    // Data structure to store categories
    QVector<Category> m_categories;
    
    // Currently displayed node buttons
    QVector<DraggableButton*> m_currentNodeButtons;
};

#endif // FLOATING_TOOLBAR_HPP