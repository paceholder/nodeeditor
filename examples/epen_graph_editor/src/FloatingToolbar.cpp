#include "FloatingToolbar.hpp"
#include "GraphEditorMainWindow.hpp"
#include "QPushButton"
#include <QApplication>
#include <QDebug>
#include <QFontDatabase>
#include <QFontMetrics>
#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>

FloatingToolbar::FloatingToolbar(GraphEditorWindow *parent)
    : FloatingPanelBase(parent, "Tools")
{
    // Set panel-specific dimensions
    setFloatingWidth(150);
    setDockedWidth(200);

    // Initialize UI
    setupUI();
    connectSignals();

    // Initial floating size and position
    setFixedWidth(floatingWidth());
    if (parent) {
        m_floatingGeometry = QRect((parent->width() - floatingWidth()) / 2,
                                   (parent->height() - height()) / 2,
                                   floatingWidth(),
                                   height());
        setGeometry(m_floatingGeometry);
    }

    // Ensure toolbar is on top
    raise();
    m_floatHeight = height();

    // Start docked to the left
    setDockPosition(DockPosition::DockedLeft);
}

QString FloatingToolbar::createSafeButtonText(const QString &icon, const QString &text)
{
    QFont testFont = QApplication::font();
    QFontMetrics fm(testFont);
    QRect boundingRect = fm.boundingRect(icon);

    if (boundingRect.width() < 5 || boundingRect.height() < 5) {
        if (text.contains("Video Input"))
            return "< " + text;
        if (text.contains("Video Output"))
            return "> " + text;
        if (text.contains("Process"))
            return "* " + text;
        if (text.contains("Image"))
            return "# " + text;
        if (text.contains("FixBuffer"))
            return "= " + text;
        if (text.contains("Zoom In"))
            return "+ " + text;
        if (text.contains("Zoom Out"))
            return "- " + text;
        if (text.contains("Reset"))
            return "R " + text;
        return text;
    }

    return icon + " " + text;
}

void FloatingToolbar::setupUI()
{
    // Call base class setup
    setupBaseUI("Tools");

    QFont buttonFont = QApplication::font();
#ifdef Q_OS_MAC
    buttonFont.setFamily("Helvetica Neue");
#elif defined(Q_OS_WIN)
    buttonFont.setFamily("Segoe UI");
#endif
    buttonFont.setPointSize(11);

    // Additional style for buttons
    QString additionalStyle = "DraggableButton {"
                              "   padding: 6px 8px;"
                              "   margin: 2px;"
                              "   border: 1px solid #bbb;"
                              "   border-radius: 4px;"
                              "   background-color: white;"
                              "   text-align: left;"
                              "   font-size: 11px;"
                              "}"
                              "DraggableButton:hover {"
                              "   background-color: #e0e0e0;"
                              "   border-color: #999;"
                              "}"
                              "DraggableButton:pressed {"
                              "   background-color: #d0d0d0;"
                              "}"
                              "DraggableButton:disabled {"
                              "   background-color: #f0f0f0;"
                              "   color: #999;"
                              "}";

    getContentWidget()->setStyleSheet(getContentWidget()->styleSheet() + additionalStyle);

    QVBoxLayout *layout = getContentLayout();

    // Node creation section
    addCategory(layout, "Nodes:");

    addNodeButton("Video Input",
                  QString::fromUtf8("\u25C0"),
                  "<",
                  "Create a video input node",
                  true,
                  "VideoInput",
                  buttonFont,
                  layout);

    addNodeButton("Video Output",
                  QString::fromUtf8("\u25B6"),
                  "<",
                  "Create a video output node",
                  false,
                  "VideoOutput",
                  buttonFont,
                  layout);

    addNodeButton("Process",
                  QString::fromUtf8("\u2666"),
                  "*",
                  "Create a processing node",
                  true,
                  "Process",
                  buttonFont,
                  layout);

    addSeparator(layout);

    addCategory(layout, "Images:");

    addNodeButton("Input Image",
                  QString::fromUtf8("\u2B06"),
                  "<<",
                  "Create an Input Image node",
                  true,
                  "InImage",
                  buttonFont,
                  layout);

    addNodeButton("Output Image",
                  QString::fromUtf8("\u2B07"),
                  ">>",
                  "Create an Output Image node",
                  true,
                  "OutImage",
                  buttonFont,
                  layout);

    addSeparator(layout);

    addCategory(layout, "Buffers:");

    addNodeButton("Fixed Buffer",
                  QString::fromUtf8("\u2B1B"),
                  "<<",
                  "Create a Fixed Buffer node",
                  true,
                  "FixBuffer",
                  buttonFont,
                  layout);

    addNodeButton("Gpu Input Buffer",
                  QString::fromUtf8("\u2B16"),
                  ">>",
                  "Create a Gpu Input Buffer node",
                  true,
                  "InBuffer",
                  buttonFont,
                  layout);

    addNodeButton("Gpu Output Buffer",
                  QString::fromUtf8("\u2B17"),
                  ">>",
                  "Create a Gpu Output Buffer node",
                  true,
                  "OutBuffer",
                  buttonFont,
                  layout);

    addNodeButton("Slider Input Buffer",
                  QString::fromUtf8("\u2B30"),
                  ">>",
                  "Create a UI Slider Buffer node",
                  true,
                  "SliderBuffer",
                  buttonFont,
                  layout);

    addNodeButton("Checkbox Input Buffer",
                  QString::fromUtf8("\u2B1A"),
                  ">>",
                  "Create a UI Checkbox Buffer node",
                  true,
                  "CheckboxBuffer",
                  buttonFont,
                  layout);

    addNodeButton("Color4 Input Buffer",
                  QString::fromUtf8("\u2B24"),
                  ">>",
                  "Create a UI Color4 Buffer node",
                  true,
                  "Color4Buffer",
                  buttonFont,
                  layout);

    addNodeButton("Plain Number Input Buffer",
                  QString::fromUtf8("\u2B04"),
                  ">>",
                  "Create a UI Plain Number Buffer node",
                  true,
                  "PlainNumberBuffer",
                  buttonFont,
                  layout);
    // View controls
    QLabel *viewLabel = new QLabel("View:");
    viewLabel->setStyleSheet("font-weight: bold; color: #333;");
    layout->addWidget(viewLabel);

    struct ViewControl
    {
        QString name;
        QString icon;
        QString fallback;
    };

    QVector<ViewControl> viewControls = {{"Zoom In", QString::fromUtf8("\u2295"), "+"},
                                         {"Zoom Out", QString::fromUtf8("\u2296"), "-"},
                                         {"Reset View", QString::fromUtf8("\u21BA"), "R"}};

    QPushButton *btnZoomIn = new QPushButton(
        createSafeButtonText(viewControls[0].icon, viewControls[0].name));
    QPushButton *btnZoomOut = new QPushButton(
        createSafeButtonText(viewControls[1].icon, viewControls[1].name));
    QPushButton *btnResetView = new QPushButton(
        createSafeButtonText(viewControls[2].icon, viewControls[2].name));

    btnZoomIn->setFont(buttonFont);
    btnZoomOut->setFont(buttonFont);
    btnResetView->setFont(buttonFont);

    layout->addWidget(btnZoomIn);
    layout->addWidget(btnZoomOut);
    layout->addWidget(btnResetView);

    connect(btnZoomIn, &QPushButton::clicked, this, &FloatingToolbar::zoomInRequested);
    connect(btnZoomOut, &QPushButton::clicked, this, &FloatingToolbar::zoomOutRequested);
    connect(btnResetView, &QPushButton::clicked, this, &FloatingToolbar::resetViewRequested);

    layout->addStretch();

    // Initial size
    getContentWidget()->adjustSize();
    adjustSize();
}

void FloatingToolbar::addCategory(QVBoxLayout *layout, QString name)
{
    layout->addSpacing(5);
    QLabel *nodeLabel = new QLabel(name);
    nodeLabel->setStyleSheet("font-weight: bold; color: #333;");
    layout->addWidget(nodeLabel);
}

void FloatingToolbar::addSeparator(QVBoxLayout *layout)
{
    layout->addSpacing(10);
    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    layout->addWidget(separator);
    layout->addSpacing(5);
}
void FloatingToolbar::addNodeButton(QString name,
                                    QString icon,
                                    QString fallback,
                                    QString tooltip,
                                    bool enabled,
                                    QString actionName,
                                    QFont buttonFont,
                                    QVBoxLayout *layout)
{
    DraggableButton *btn = new DraggableButton(actionName, this);
    QString buttonText = createSafeButtonText(icon, name);
    btn->setText(buttonText);
    btn->setToolTip(tooltip);
    btn->setEnabled(enabled);
    btn->setFont(buttonFont);
    btn->setProperty("nodeType", name);
    layout->addWidget(btn);
}
void FloatingToolbar::connectSignals()
{
    if (getGraphEditor()) {
        connect(this, &FloatingToolbar::zoomInRequested, [this]() {
            getGraphEditor()->scale(1.2, 1.2);
        });

        connect(this, &FloatingToolbar::zoomOutRequested, [this]() {
            getGraphEditor()->scale(0.8, 0.8);
        });

        connect(this, &FloatingToolbar::resetViewRequested, [this]() {
            getGraphEditor()->resetTransform();
        });
    }
}