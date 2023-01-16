#include "ImageShowModel.hpp"

#include "PixmapData.hpp"

#include <QtNodes/NodeDelegateModelRegistry>

#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtWidgets/QFileDialog>

void ImageShowModel::init()
{
    createPort(PortType::In, "pixmap", "P");
    createPort(PortType::Out, "pixmap", "P", QtNodes::ConnectionPolicy::Many);
}

bool ImageShowModel::eventFilter(QObject *object, QEvent *event)
{
    if (object == _label) {
        int w = _label->width();
        int h = _label->height();

        if (event->type() == QEvent::Resize) {
            if (!_pixmap.isNull()) {
                _label->setPixmap(_pixmap.scaled(w, h, Qt::KeepAspectRatio));
            }
        }
    }

    return false;
}

void ImageShowModel::setInData(QVariant const nodeData, PortIndex portIndex)
{
    if (!nodeData.isNull()) {
        updateOutPortData(0, nodeData);

        _pixmap = nodeData.value<QPixmap>();

        int w = _label->width();
        int h = _label->height();

        _label->setPixmap(_pixmap.scaled(w, h, Qt::KeepAspectRatio));
    } else {
        _label->setPixmap(QPixmap());
    }

    Q_EMIT dataUpdated(0);
}

QWidget *ImageShowModel::embeddedWidget()
{
    if (!_label) {
        _label = new QLabel("Image will appear here");
        _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

        QFont f = _label->font();
        f.setBold(true);
        f.setItalic(true);

        _label->setFont(f);

        _label->setMinimumSize(200, 200);

        _label->installEventFilter(this);
    }
    return _label;
}