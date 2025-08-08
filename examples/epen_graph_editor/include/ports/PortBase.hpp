#ifndef PORTBASE_HPP
#define PORTBASE_HPP

#include <QObject>

class DataFlowMode;
class CodeEditor;

class PortBase : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QString Name MEMBER _name NOTIFY propertyChanged)
    PortBase(QString name, QObject *parent = nullptr)
        : QObject(parent)
        , _name(name)
    {}
    virtual bool isImage() { return true; }
    QString getName() { return _name; }
signals:
    void propertyChanged();

protected:
    QString _name;
};

#endif