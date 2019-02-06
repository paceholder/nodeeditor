#include "ImageLoaderModel.hpp"

#include <QtCore/QEvent>
#include <QtCore/QDir>

#include <QtWidgets/QFileDialog>

ImageLoaderModel::
ImageLoaderModel()
  : _label(new QLabel("Double click to load image"))
{
  _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

  QFont f = _label->font();
  f.setBold(true);
  f.setItalic(true);

  _label->setFont(f);

  _label->setFixedSize(200, 200);

  _label->installEventFilter(this);
}


unsigned int
ImageLoaderModel::
nPorts(PortType portType) const
{
  unsigned int result = 1;

  switch (portType)
  {
    case PortType::In:
      result = 0;
      break;

    case PortType::Out:
      result = 1;

    default:
      break;
  }

  return result;
}


bool
ImageLoaderModel::
eventFilter(QObject *object, QEvent *event)
{
  if (object == _label)
  {
    int w = _label->width();
    int h = _label->height();

    if (event->type() == QEvent::MouseButtonPress)
    {

      QString fileName =
        QFileDialog::getOpenFileName(nullptr,
                                     tr("Open Image"),
                                     QDir::homePath(),
                                     tr("Image Files (*.png *.jpg *.bmp)"));

      _pixmap = QPixmap(fileName);

      _label->setPixmap(_pixmap.scaled(w, h, Qt::KeepAspectRatio));

      Q_EMIT dataUpdated(0);

      return true;
    }
    else if (event->type() == QEvent::Resize)
    {
      if (!_pixmap.isNull())
        _label->setPixmap(_pixmap.scaled(w, h, Qt::KeepAspectRatio));
    }
  }

  return false;
}


NodeDataType
ImageLoaderModel::
dataType(PortType, PortIndex) const
{
  return PixmapData().type();
}


std::shared_ptr<NodeData>
ImageLoaderModel::
outData(PortIndex)
{
  return std::make_shared<PixmapData>(_pixmap);
}
