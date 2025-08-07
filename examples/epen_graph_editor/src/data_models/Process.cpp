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

QString Process::getCudaProgram()
{
    return _cudaProgram.replace("<MainFunctionName>", _name);
}

QString Process::getMetalProgram()
{
    return _metalProgram.replace("<MainFunctionName>", _name);
}

QString Process::getOpenclProgram()
{
    return _openclProgram.replace("<MainFunctionName>", _name);
}

void Process::setCudaProgram(QString code)
{
    _cudaProgram = code.replace("void " + _name + "(", "void <MainFunctionName>(");
}

void Process::setMetalProgram(QString code)
{
    _metalProgram = code.replace("void " + _name + "(", "void <MainFunctionName>(");
}

void Process::setOpenclProgram(QString code)
{
    _openclProgram = code.replace("void " + _name + "(", "void <MainFunctionName>(");
}

QSet<int> Process::findReadonlyLines(QString programCode)
{
    QSet<int> lineSet{};
    QStringList lines = programCode.split('\n');
    bool functionNameFound = false;
    for (int i = 0; i < lines.size(); ++i) {
        if (functionNameFound) {
            if (lines[i].contains("{")) {
                break;
            }
            lineSet << i;
        }
        if (lines[i].contains("<MainFunctionName>")) {
            functionNameFound = true;
            lineSet << i;
            continue;
        }
    }
    return lineSet;
}

QSet<int> Process::getCudaReadonlyLines()
{
    return findReadonlyLines(_cudaProgram);
}

QSet<int> Process::getOpenclReadonlyLines()
{
    return findReadonlyLines(_openclProgram);
}

QSet<int> Process::getMetalReadonlyLines()
{
    return findReadonlyLines(_metalProgram);
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

PortBase *Process::addInput(DataFlowModel *model, bool isImage, QString name)
{
    PortBase *newPort = new ImagePort(name, 100, 100, this);
    PortAddRemoveWidget *widget = model->widget(_nodeId);
    if (widget) {
        widget->addLeftPort(newPort);
    }
    return newPort;
}