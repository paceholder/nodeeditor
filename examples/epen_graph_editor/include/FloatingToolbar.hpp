#ifndef FLOATING_TOOLBAR_HPP
#define FLOATING_TOOLBAR_HPP

#include "FloatingPanelBase.hpp"
#include "DraggableButton.hpp"

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
};

#endif // FLOATING_TOOLBAR_HPP