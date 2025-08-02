#include "DraggableButton.hpp"
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QWidget>

DraggableButton::DraggableButton(QString actionName, QWidget *parent)
    : QPushButton(parent)
    , _actionName(actionName)
{
    setMinimumSize(20, 20);
    setStyleSheet("background-color: lightgray; border: 1px solid black;");
}

void DraggableButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(_actionName);
        drag->setMimeData(mimeData);
        drag->exec(Qt::CopyAction);
    }
}