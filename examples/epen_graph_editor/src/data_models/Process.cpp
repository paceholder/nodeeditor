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
    QString rawPrototype = "__global__ void <MainFunctionName>() \n";
    if (raw)
        return rawPrototype;
    rawPrototype.replace("<MainFunctionName>", _name);
    return rawPrototype;
}

QString Process::getOpenclPrototype(bool raw)
{
    QString rawPrototype = "void __kernel <MainFunctionName>(<MainFunctionParams>)\n";
    if (raw)
        return rawPrototype;
    rawPrototype.replace("<MainFunctionName>", _name);
    QString parameters = "";
    for (PortBase *p : _leftPorts) {
        if (p->isImage()) {
            if (parameters != "")
                parameters += ",\n";
            parameters += "__read_only image2d_t " + p->getName();
        }
    }
    for (PortBase *p : _rightPorts) {
        if (p->isImage()) {
            if (parameters != "")
                parameters += ",\n";
            parameters += "__write_only image2d_t " + p->getName();
        }
    }
    rawPrototype.replace("<MainFunctionParams>", parameters);
    return rawPrototype;
}

QString Process::getMetalPrototype(bool raw)
{
    QString rawPrototype = "kernel void <MainFunctionName>(<MainFunctionParams>)\n";
    if (raw)
        return rawPrototype;
    rawPrototype.replace("<MainFunctionName>", _name);
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

PortBase *Process::createPortObject(bool isImage, QString name)
{
    PortBase *newPort = nullptr;
    if (isImage)
        newPort = new ImagePort(name, 100, 100, this);
    else
        newPort = new BufferPort(name, 1000, this);
    connect(newPort, &PortBase::propertyChanged, this, &Process::portPropertyChanged);
    return newPort;
}

void Process::portPropertyChanged()
{
    if (_editor) {
        _editor->updateCode();
    }
}

PortBase *Process::addInput(DataFlowModel *model, bool isImage, QString name)
{
    PortAddRemoveWidget *widget = model->widget(_nodeId);
    PortBase *newPort = createPortObject(isImage, name);
    if (widget) {
        widget->addLeftPort(newPort);
    }
    if (_editor) {
        _editor->updateCode();
    }
    return newPort;
}

PortBase *Process::addOutput(DataFlowModel *model, bool isImage, QString name)
{
    PortBase *newPort = createPortObject(isImage, name);
    PortAddRemoveWidget *widget = model->widget(_nodeId);
    if (widget) {
        widget->addRightPort(newPort);
    }
    if (_editor) {
        _editor->updateCode();
    }
    return newPort;
}