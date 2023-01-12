#include "ImageShowModel.hpp"

#include "PixmapData.hpp"

#include <QtNodes/NodeDelegateModelRegistry>

#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtWidgets/QFileDialog>

void ImageShowModel::init()
{
    createPort(PortType::In, std::make_shared<PixmapData>(), "P");
    createPort(PortType::Out, std::make_shared<PixmapData>(), "P", QtNodes::ConnectionPolicy::Many);
}

bool ImageShowModel::eventFilter(QObject *object, QEvent *event)
{
    if (object == _label) {
        int w = _label->width();
        int h = _label->height();

        if (event->type() == QEvent::Resize) {
            auto d = portData<PixmapData>(PortType::Out, 0);
            if (d && !d->data.isNull()) {
                _label->setPixmap(d->data.scaled(w, h, Qt::KeepAspectRatio));
            }
        }
    }

    return false;
}

void ImageShowModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex portIndex)
{
    if (nodeData) {
        auto d = std::dynamic_pointer_cast<PixmapData>(nodeData);

        setPortData(PortType::Out, portIndex, nodeData);

        int w = _label->width();
        int h = _label->height();

        _label->setPixmap(d->data.scaled(w, h, Qt::KeepAspectRatio));
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