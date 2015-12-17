#include <QtWidgets/QGraphicsBlurEffect>

#include <QtWidgets/QGraphicsItem>

class ConnectionGraphicsObject;

class ConnectionBlurEffect : public QGraphicsBlurEffect
{

public:

  ConnectionBlurEffect(ConnectionGraphicsObject* item);

  void draw(QPainter* painter) override;

private:

  ConnectionGraphicsObject* _object;
};
