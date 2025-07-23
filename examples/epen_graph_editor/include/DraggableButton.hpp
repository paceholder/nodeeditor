#ifndef DRAGGABLE_BUTTON
#define DRAGGABLE_BUTTON
#include <QPushButton>

class DraggableButton : public QPushButton
{
public:
    explicit DraggableButton(QString actionName, QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *event) override;

private:
    QString _actionName;
};
#endif