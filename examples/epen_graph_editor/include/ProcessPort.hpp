#ifndef PROCESSPORT_HPP
#define PROCESSPORT_HPP

#include "data_models/OperationDataModel.hpp"
#include <QObject>

class DataFlowMode;
class CodeEditor;

class ProcessPort : public QObject
{
    Q_OBJECT
public:
    ProcessPort(bool isImage, QObject *parent = nullptr)
        : QObject(parent)
        , _isImage(isImage)
    {}
    virtual bool isImage() { return _isImage; }
    QString getName() { return _connectedNode->name(); }
    void setConnection(OperationDataModel *connectedNode) { _connectedNode = connectedNode; }

    OperationDataModel *getConnectedNode() { return _connectedNode; }
    void removeConnection() { _connectedNode = nullptr; }

    bool isConnected() { return _connectedNode != nullptr; }
    bool isReadWrite() { return true; }
signals:
    void propertyChanged();

protected:
    OperationDataModel *_connectedNode = nullptr;
    bool _isImage;
};

#endif