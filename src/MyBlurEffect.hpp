#include <QtWidgets/QGraphicsBlurEffect>

#include <QtWidgets/QGraphicsItem>

class MyBlurEffect : public QGraphicsBlurEffect
{

public:

  MyBlurEffect(QGraphicsItem* item)
    : _item(item)
  {
    //
  }

  void draw(QPainter* painter) override
  {
    //this->setBlurRadius(4);

    QGraphicsBlurEffect::draw(painter);

    _item->paint(painter, nullptr, nullptr);

  }

private:

  QGraphicsItem* _item;
};
