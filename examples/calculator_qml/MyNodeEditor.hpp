#pragma once

#include <QtNodes/QmlWrapper>

class MyNodeEditor : public QtNodes::QmlWrapper
{
    Q_OBJECT

public:
    explicit MyNodeEditor(QQuickItem *parent = nullptr)
        : QtNodes::QmlWrapper(parent)
    {}

protected:
    std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registerDataModels() const override;
};
