#include "data_models/Process.hpp"
#include "CodeEditor.hpp"
#include "DataFlowModel.hpp"

Process::Process()
    : _grid(new Size("", "", "", this))
    , _block(new Size("", "", "", this))
{}

Process::~Process() {}

QString Process::caption() const
{
    return QStringLiteral("Process");
}

bool Process::portCaptionVisible(PortType portType, PortIndex portIndex) const
{
    Q_UNUSED(portType);
    Q_UNUSED(portIndex);
    return true;
}

QString Process::portCaption(PortType portType, PortIndex portIndex) const
{
    switch (portType) {
    case PortType::Out:

        return _rightPorts.at(portIndex)->isImage() ? QStringLiteral("Image")
                                                    : QStringLiteral("Buffer");

    default:
    case PortType::In:
        return _leftPorts.at(portIndex)->isImage() ? QStringLiteral("Image")
                                                   : QStringLiteral("Buffer");
    }
    return QString();
}

unsigned int Process::nPorts(PortType portType) const
{
    unsigned int result;

    if (portType == PortType::In)
        result = _leftPorts.size();
    else
        result = _rightPorts.size();

    return result;
}

void Process::setPortTypeRight(PortIndex portIndex, PortBase *port)
{
    _rightPorts.insert(portIndex - 1, port);
}

void Process::removePortTypeRight(PortIndex portIndex)
{
    if (portIndex < _rightPorts.size()) {
        _rightPorts.at(portIndex);
        _rightPorts.removeAt(portIndex);
    }
    if (_editor) {
        _editor->updateCode();
    }
}

void Process::setPortTypeLeft(PortIndex portIndex, PortBase *port)
{
    _leftPorts.insert(portIndex - 1, port);
}

void Process::removePortTypeLeft(PortIndex portIndex)
{
    if (portIndex < _leftPorts.size()) {
        delete _leftPorts.at(portIndex);
        _leftPorts.removeAt(portIndex);
    }
    if (_editor) {
        _editor->updateCode();
    }
}

NodeDataType Process::dataType(PortType portType, PortIndex portIndex) const
{
    NodeDataType result = BUFFER_DATA_TYPE;

    switch (portType) {
    case PortType::Out:

        return _rightPorts.at(portIndex)->isImage() ? IMAGE_DATA_TYPE : BUFFER_DATA_TYPE;

    default:
    case PortType::In:
        return _leftPorts.at(portIndex)->isImage() ? IMAGE_DATA_TYPE : BUFFER_DATA_TYPE;
    }
    return result;
}

QObject *Process::findPort(int portIndex, bool isRightPort)
{
    if (isRightPort) {
        return _rightPorts.at(portIndex);
    }
    return _leftPorts.at(portIndex);
}

QString Process::getCudaPrototype(bool raw)
{
    QString rawPrototype = "__global__ void <MainFunctionName>(<MainFunctionParams>)\n";
    if (raw)
        return rawPrototype;
    rawPrototype.replace("<MainFunctionName>", _name);
    QString parameters = "";
    QString paramDelimiter = ",\n\t\t\t";
    for (PortBase *p : _leftPorts) {
        if (!p->isImage()) {
            BufferPort *port = dynamic_cast<BufferPort *>(p);
            UIBufferBase *bufferNode = port->getBufferNode();
            if (!bufferNode)
                continue;
            if (parameters != "")
                parameters += paramDelimiter;
            parameters += "const "
                          + bufferNode->getVariableType(UIBufferBase::CUDA, p->getName(), true);
        }
    }
    for (PortBase *p : _leftPorts) {
        if (p->isImage()) {
            if (parameters != "")
                parameters += paramDelimiter;
            parameters += "cudaTextureObject_t " + p->getName() + paramDelimiter + "unsigned int "
                          + p->getName() + "Width" + paramDelimiter + "unsigned int " + p->getName()
                          + "Height";
        }
    }
    for (PortBase *p : _rightPorts) {
        if (p->isImage()) {
            if (parameters != "")
                parameters += paramDelimiter;
            parameters += "cudaSurfaceObject_t " + p->getName() + paramDelimiter + "unsigned int "
                          + p->getName() + "Width" + paramDelimiter + "unsigned int " + p->getName()
                          + "Height";
        }
    }
    for (PortBase *p : _rightPorts) {
        if (!p->isImage()) {
            BufferPort *port = dynamic_cast<BufferPort *>(p);
            UIBufferBase *bufferNode = port->getBufferNode();
            if (!bufferNode)
                continue;
            if (parameters != "")
                parameters += paramDelimiter;
            parameters += bufferNode->getVariableType(UIBufferBase::CUDA, p->getName(), false);
        }
    }
    rawPrototype.replace("<MainFunctionParams>", parameters);
    return rawPrototype;
}

QString Process::getOpenclPrototype(bool raw)
{
    QString rawPrototype = "void __kernel <MainFunctionName>(<MainFunctionParams>)\n";
    if (raw)
        return rawPrototype;
    rawPrototype.replace("<MainFunctionName>", _name);
    QString parameters = "";
    QString paramDelimiter = ",\n\t\t\t";
    for (PortBase *p : _leftPorts) {
        if (!p->isImage()) {
            BufferPort *port = dynamic_cast<BufferPort *>(p);
            UIBufferBase *bufferNode = port->getBufferNode();
            if (!bufferNode)
                continue;
            if (parameters != "")
                parameters += paramDelimiter;
            parameters += (bufferNode->isPointer() ? QString("__global") : QString("")) + " const "
                          + bufferNode->getVariableType(UIBufferBase::OPENCL, p->getName(), true);
        }
    }
    for (PortBase *p : _leftPorts) {
        if (p->isImage()) {
            if (parameters != "")
                parameters += paramDelimiter;
            parameters += "__read_only image2d_t " + p->getName();
        }
    }
    for (PortBase *p : _rightPorts) {
        if (p->isImage()) {
            OutputImagePort *port = dynamic_cast<OutputImagePort *>(p);
            if (parameters != "")
                parameters += paramDelimiter;
            parameters += (port->isReadWrite() ? QString("read_write") : QString("__write_only"))
                          + " image2d_t " + p->getName();
        }
    }
    for (PortBase *p : _rightPorts) {
        if (!p->isImage()) {
            BufferPort *port = dynamic_cast<BufferPort *>(p);
            UIBufferBase *bufferNode = port->getBufferNode();
            if (!bufferNode)
                continue;
            if (parameters != "")
                parameters += paramDelimiter;
            parameters += "__global "
                          + bufferNode->getVariableType(UIBufferBase::OPENCL, p->getName(), false);
        }
    }
    rawPrototype.replace("<MainFunctionParams>", parameters);
    return rawPrototype;
}

QString Process::getMetalPrototype(bool raw)
{
    QString rawPrototype
        = "<MainFunctionInputParams>kernel void <MainFunctionName>(<MainFunctionParams>\n";
    if (raw)
        return rawPrototype;

    QString MainFunctionInputParams = "";

    int idIndex = 0;
    for (PortBase *p : _leftPorts) {
        if (!p->isImage()) {
            BufferPort *port = dynamic_cast<BufferPort *>(p);
            UIBufferBase *bufferNode = port->getBufferNode();
            if (!bufferNode)
                continue;
            if (bufferNode->isPointer())
                continue;
            if (MainFunctionInputParams == "") {
                MainFunctionInputParams = "struct InputParameters {";
            }

            MainFunctionInputParams += "\n\t";
            MainFunctionInputParams += bufferNode->getVariableType(UIBufferBase::METAL,
                                                                   p->getName(),
                                                                   true)
                                       + " [[id(" + QString::number(idIndex) + ")]];";

            idIndex++;
        }
    }
    int bufferIndex = 0;
    if (MainFunctionInputParams != "") {
        MainFunctionInputParams += "\n};\n";
        bufferIndex++;
    }
    rawPrototype.replace("<MainFunctionInputParams>", MainFunctionInputParams);
    rawPrototype.replace("<MainFunctionName>", _name);
    QString paramDelimiter = ",\n\t\t\t";
    QString parameters = MainFunctionInputParams == ""
                             ? ""
                             : "constant InputParameters& params [[buffer(0)]]";
    int textureId = 0;

    for (PortBase *p : _leftPorts) {
        if (!p->isImage()) {
            BufferPort *port = dynamic_cast<BufferPort *>(p);
            UIBufferBase *bufferNode = port->getBufferNode();
            if (!bufferNode)
                continue;
            if (!bufferNode->isPointer())
                continue;
            if (parameters != "")
                parameters += paramDelimiter;
            parameters += "device const "
                          + bufferNode->getVariableType(UIBufferBase::OPENCL, p->getName(), true)
                          + +" [[buffer(" + QString::number(bufferIndex) + ")]]";
            bufferIndex++;
        }
    }

    for (PortBase *p : _leftPorts) {
        if (p->isImage()) {
            if (parameters != "")
                parameters += paramDelimiter;
            parameters += "texture2d<half, access::read> " + p->getName() + " [[texture("
                          + QString::number(textureId) + ")]]";
            textureId++;
        }
    }
    for (PortBase *p : _rightPorts) {
        if (p->isImage()) {
            OutputImagePort *port = dynamic_cast<OutputImagePort *>(p);
            if (parameters != "")
                parameters += paramDelimiter;
            parameters += "texture2d<half, access::"
                          + (port->isReadWrite() ? QString("read_write") : QString("write")) + "> "
                          + p->getName() + " [[texture(" + QString::number(textureId) + ")]]";
            textureId++;
        }
    }
    for (PortBase *p : _rightPorts) {
        if (!p->isImage()) {
            BufferPort *port = dynamic_cast<BufferPort *>(p);
            UIBufferBase *bufferNode = port->getBufferNode();
            if (!bufferNode)
                continue;
            if (parameters != "")
                parameters += paramDelimiter;
            parameters += "device "
                          + bufferNode->getVariableType(UIBufferBase::METAL, p->getName(), false)
                          + " [[ buffer(" + QString::number(bufferIndex) + ") ]]";
            bufferIndex++;
        }
    }
    rawPrototype.replace("<MainFunctionParams>", parameters);
    return rawPrototype;
}

QString Process::getCudaProgram()
{
    QString cudaProgram(_cudaProgram);
    return cudaProgram.replace("<FUNCTION_PROTOTYPE>", getCudaPrototype(false));
}

QString Process::getMetalProgram()
{
    QString metalProgram(_metalProgram);
    return metalProgram.replace("<FUNCTION_PROTOTYPE>", getMetalPrototype(false));
}

QString Process::getOpenclProgram()
{
    QString openclProgram(_openclProgram);
    return openclProgram.replace("<FUNCTION_PROTOTYPE>", getOpenclPrototype(false));
}

void Process::setCudaProgram(QString code)
{
    _cudaProgram = code.replace(getCudaPrototype(false), "<FUNCTION_PROTOTYPE>");
}

void Process::setMetalProgram(QString code)
{
    _metalProgram = code.replace(getMetalPrototype(false), "<FUNCTION_PROTOTYPE>");
}

void Process::setOpenclProgram(QString code)
{
    _openclProgram = code.replace(getOpenclPrototype(false), "<FUNCTION_PROTOTYPE>");
}

QSet<int> Process::findReadonlyLines(QString programCode, QString prototype)
{
    QSet<int> lineSet{};
    QStringList lines = programCode.split('\n');
    QStringList prototypeLines = prototype.split('\n');
    prototypeLines.removeAll("");
    bool functionNameFound = false;
    for (int i = 0; i < lines.size(); ++i) {
        if (functionNameFound) {
            lineSet << i;
            if (lines[i] == prototypeLines[prototypeLines.size() - 1]) {
                break;
            }
        }
        if (lines[i] == prototypeLines[0]) {
            functionNameFound = true;
            lineSet << i;
            if (prototypeLines.size() == 1)
                break;
            continue;
        }
    }
    return lineSet;
}

QSet<int> Process::getCudaReadonlyLines()
{
    return findReadonlyLines(getCudaProgram(), getCudaPrototype(false));
}

QSet<int> Process::getOpenclReadonlyLines()
{
    return findReadonlyLines(getOpenclProgram(), getOpenclPrototype(false));
}

QSet<int> Process::getMetalReadonlyLines()
{
    return findReadonlyLines(getMetalProgram(), getMetalPrototype(false));
}

void Process::setEditor(CodeEditor *editor)
{
    _editor = editor;
}

void Process::setName(QString newName)
{
    if (newName == "")
        return;
    _name = newName;
    if (_editor) {
        _editor->updateCode();
    }
}

PortBase *Process::createPortObject(DataFlowModel *model, bool isImage, bool isRight)
{
    PortBase *newPort = nullptr;
    if (isImage) {
        if (isRight)
            newPort = new OutputImagePort(model->getOutputImagePortName(), 100, 100, this);
        else
            newPort = new ImagePort(model->getInputImagePortName(), 100, 100, this);
    } else
        newPort = new BufferPort(isRight ? model->getOutputBufferPortName()
                                         : model->getInputBufferPortName(),
                                 1000,
                                 this);
    return newPort;
}

PortBase *Process::addInput(DataFlowModel *model, bool isImage)
{
    PortAddRemoveWidget *widget = model->widget(_nodeId);
    PortBase *newPort = createPortObject(model, isImage, false);
    if (widget) {
        widget->addLeftPort(newPort);
    }
    if (_editor) {
        _editor->updateCode();
    }
    return newPort;
}

PortBase *Process::addOutput(DataFlowModel *model, bool isImage)
{
    PortBase *newPort = createPortObject(model, isImage, true);
    PortAddRemoveWidget *widget = model->widget(_nodeId);
    if (widget) {
        widget->addRightPort(newPort);
    }
    if (_editor) {
        _editor->updateCode();
    }
    return newPort;
}

void Process::addInPortConnection(UIBufferBase *bufferNode, int inPortIndex)
{
    if (_leftPorts.at(inPortIndex)->isImage())
        throw std::runtime_error("Image port");
    BufferPort *port = dynamic_cast<BufferPort *>(_leftPorts.at(inPortIndex));
    port->setBuffer(bufferNode);
    if (_editor) {
        _editor->updateCode();
    }
}

void Process::addOutPortConnection(UIBufferBase *bufferNode, int outPortIndex)
{
    if (_rightPorts.at(outPortIndex)->isImage())
        throw std::runtime_error("Image port");
    BufferPort *port = dynamic_cast<BufferPort *>(_rightPorts.at(outPortIndex));
    port->setBuffer(bufferNode);
    if (_editor) {
        _editor->updateCode();
    }
}

void Process::removeInPortConnection(int inPortIndex)
{
    if (_leftPorts.at(inPortIndex)->isImage())
        throw std::runtime_error("Image port");
    BufferPort *port = dynamic_cast<BufferPort *>(_leftPorts.at(inPortIndex));
    port->removeBuffer();
    if (_editor) {
        _editor->updateCode();
    }
}

void Process::removeOutPortConnection(int outPortIndex)
{
    if (_rightPorts.at(outPortIndex)->isImage())
        throw std::runtime_error("Image port");
    BufferPort *port = dynamic_cast<BufferPort *>(_rightPorts.at(outPortIndex));
    port->removeBuffer();
    if (_editor) {
        _editor->updateCode();
    }
}