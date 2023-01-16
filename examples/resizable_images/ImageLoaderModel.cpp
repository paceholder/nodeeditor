#include "ImageLoaderModel.hpp"

#include <QtCore/QDir>
#include <QtCore/QEvent>

#include <QtWidgets/QFileDialog>

void ImageLoaderModel::init()
{
    createPort(PortType::Out, "pixmap", "P", QtNodes::ConnectionPolicy::Many);

    _label = new QLabel("Double click to load image");

    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    QFont f = _label->font();
    f.setBold(true);
    f.setItalic(true);

    _label->setFont(f);

    _label->setMinimumSize(200, 200);
    _label->setMaximumSize(500, 300);

    _label->installEventFilter(this);
}

bool ImageLoaderModel::eventFilter(QObject *object, QEvent *event)
{
    if (object == _label) {
        int w = _label->width();
        int h = _label->height();

        if (event->type() == QEvent::MouseButtonPress) {
            QString fileName = QFileDialog::getOpenFileName(nullptr,
                                                            tr("Open Image"),
                                                            QDir::homePath(),
                                                            tr("Image Files (*.png *.jpg *.bmp)"));

            _pixmap = QPixmap(fileName);

            updateOutPortData(0, QVariant::fromValue(_pixmap));

            _label->setPixmap(_pixmap.scaled(w, h, Qt::KeepAspectRatio));

            Q_EMIT dataUpdated(0);

            return true;
        } else if (event->type() == QEvent::Resize) {
            if (!_pixmap.isNull())
                _label->setPixmap(_pixmap.scaled(w, h, Qt::KeepAspectRatio));
        }
    }

    return false;
}
