#ifndef FLOATING_TOOLBAR_HPP
#define FLOATING_TOOLBAR_HPP

#include "DraggableButton.hpp"
#include "FloatingPanelBase.hpp"

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
    QString createSafeButtonText(const QString &icon, const QString &text);
    void addNodeButton(QString name,
                       QString icon,
                       QString fallback,
                       QString tooltip,
                       bool enabled,
                       QString actionName,
                       QFont buttonFont,
                       QVBoxLayout *layout);
    void addSeparator(QVBoxLayout *layout);
    void addCategory(QVBoxLayout *layout, QString name);
};

#endif // FLOATING_TOOLBAR_HPP