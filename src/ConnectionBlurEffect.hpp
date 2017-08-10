#include <QtWidgets/QGraphicsBlurEffect>

#include <QtWidgets/QGraphicsItem>

namespace QtNodes
{

class ConnectionGraphicsObject;

class ConnectionBlurEffect : public QGraphicsBlurEffect
{

public:

  ConnectionBlurEffect(ConnectionGraphicsObject* item);

  void
  draw(QPainter* painter) override;

private:
};
}
