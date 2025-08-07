#ifndef PROCESS_HPP
#define PROCESS_HPP

#include "OperationDataModel.hpp"
#include "PortAddRemoveWidget.hpp"
#include "ports/BufferPort.hpp"
#include "ports/ImagePort.hpp"
#include <QMetaType>
#include <QObject>

class DataFlowMode;
class CodeEditor;

class Size : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString X MEMBER _x)
    Q_PROPERTY(QString Y MEMBER _y)
    Q_PROPERTY(QString Z MEMBER _z)

public:
    Size(QString x, QString y, QString z, QObject *parent = nullptr)
        : QObject(parent)
        , _x(x)
        , _y(y)
        , _z(z)
    {}

    // Copy constructor
    Size(const Size &other, QObject *parent = nullptr)
        : QObject(parent)
        , _x(other._x)
        , _y(other._y)
        , _z(other._z)
    {}

    // Assignment operator
    Size &operator=(const Size &other)
    {
        if (this != &other) {
            _x = other._x;
            _y = other._y;
            _z = other._z;
        }
        return *this;
    }

    // Comparison operators
    bool operator==(const Size &other) const
    {
        return _x == other._x && _y == other._y && _z == other._z;
    }

    bool operator!=(const Size &other) const { return !(*this == other); }

private:
    QString _x;
    QString _y;
    QString _z;
};

Q_DECLARE_METATYPE(Size *)
Q_DECLARE_METATYPE(BufferPort *)
Q_DECLARE_METATYPE(ImagePort *)

class Process : public OperationDataModel
{
    Q_OBJECT
public:
    Process();
    virtual ~Process();

    Q_PROPERTY(Size *Grid_Size MEMBER _grid NOTIFY propertyChanged)
    Q_PROPERTY(Size *Block_Size MEMBER _block NOTIFY propertyChanged)

    QString caption() const override;

    bool portCaptionVisible(PortType portType, PortIndex portIndex) const override;

    QString portCaption(PortType portType, PortIndex portIndex) const override;

    unsigned int nPorts(PortType portType) const override;

    void setPortTypeRight(PortIndex portIndex, PortBase *port);

    void removePortTypeRight(PortIndex portIndex);

    void setPortTypeLeft(PortIndex portIndex, PortBase *port);

    void removePortTypeLeft(PortIndex portIndex);

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    QObject *findPort(int portIndex, bool isRightPort);

    void setCudaProgram(QString code);
    void setMetalProgram(QString code);
    void setOpenclProgram(QString code);

    QString getCudaProgram();
    QString getMetalProgram();
    QString getOpenclProgram();

    QSet<int> getCudaReadonlyLines();

    QSet<int> getOpenclReadonlyLines();

    QSet<int> getMetalReadonlyLines();

    void setEditor(CodeEditor *editor);

    PortBase *addInput(DataFlowModel *model, bool isImage, QString name);
    PortBase *addOutput(DataFlowModel *model, bool isImage, QString name);

protected:
    void setName(QString newName) override;

private:
    QString getCudaPrototype(bool raw);

    QString getOpenclPrototype(bool raw);

    QString getMetalPrototype(bool raw);

    QSet<int> findReadonlyLines(QString programCode);
    PortBase *createPortObject(bool isImage, QString name);

    Size *_grid;
    Size *_block;

    QVector<PortBase *> _leftPorts{};
    QVector<PortBase *> _rightPorts{};

    CodeEditor *_editor;
    QString _cudaProgram = "<FUNCTION_PROTOTYPE>\n"
                           "{\n"
                           "    int idx = blockIdx.x * blockDim.x + threadIdx.x;\n"
                           "    if (idx < size) {\n"
                           "        output[idx] = input[idx] * 2.0f;\n"
                           "    }\n"
                           "}";

    QString _metalProgram = "#include <metal_stdlib>\n"
                            "using namespace metal;\n\n"
                            "<FUNCTION_PROTOTYPE>\n"
                            "{\n"
                            "    if (idx < size) {\n"
                            "        output[idx] = input[idx] * 2.0f;\n"
                            "    }\n"
                            "}";

    QString _openclProgram = "<FUNCTION_PROTOTYPE>\n"
                             "{\n"
                             "    int idx = get_global_id(0);\n"
                             "    if (idx < size) {\n"
                             "        output[idx] = input[idx] * 2.0f;\n"
                             "    }\n"
                             "}";
private slots:
    void portPropertyChanged();
};

#endif