#pragma once

#include <QtCore/QObject>

#include <QtNodes/NodeDelegateModel>

#include <iostream>

using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class QLineEdit;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class TextSourceDataModel : public NodeDelegateModel
{
    Q_OBJECT

public:
    void init() override;

    static QString Name() { return QString("TextSourceDataModel"); }

    QString name() const override { return TextSourceDataModel::Name(); }

public:
    void setInData(QVariant const, PortIndex const) override {}

    QWidget *embeddedWidget() override;

private Q_SLOTS:

    void onTextEdited(QString const &string);

private:
    QLineEdit *_lineEdit = nullptr;
};
