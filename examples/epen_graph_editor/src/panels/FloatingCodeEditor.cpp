#include "panels/FloatingCodeEditor.hpp"
#include "GPULanguageLexer.hpp"
#include "GraphEditorMainWindow.hpp"
#include "compilers/SimpleGPUCompiler.hpp"
#include "compilers/OpenCLCompiler.hpp"
#include "compilers/CUDACompiler.hpp"
#include "compilers/MetalCompiler.hpp"
#include "panels/CompileResultsWidget.hpp"
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QFont>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QTimer>
#include <QMoveEvent>
#include <Qsci/qsciapis.h>
#include <Qsci/qsciscintilla.h>

FloatingCodeEditor::FloatingCodeEditor(GraphEditorWindow *parent)
    : FloatingPanelBase(parent, "Code Editor")
    , m_languageCombo(nullptr)
    , m_codeEditor(nullptr)
    , m_compileButton(nullptr)
    , m_darkModeCheckBox(nullptr)
    , m_resultsWidget(nullptr)
    , m_lexer(nullptr)
    , m_maximizeButton(nullptr)
    , m_isDarkMode(false) // Default to light mode
    , m_isMaximized(false)
    , m_preMaximizeDockPosition(FloatingPanelBase::Floating)
    , m_preMaximizeDockedHeight(450)
    , m_preMaximizeDockedWidth(700)
    , m_preMaximizeFloatHeight(500)
    , m_resizing(false)
    , m_resizeEdge(NoEdge)
    , m_resizeStartHeight(0)
{
    // Set panel-specific dimensions
    setFloatingWidth(600);
    setDockedWidth(700);
    setDockedHeight(450);  // Larger default height for code editor
    
    // Set higher priority for code editor
    setPanelPriority(2);  // Higher priority than other panels (default is 1)

    // Support bottom docking in addition to left/right
    setDockingDistance(40);
    
    // Enable mouse tracking for resize cursor
    setMouseTracking(true);

    // Setup languages
    m_supportedLanguages = {"OpenCL", "CUDA", "Metal"};

    // Initialize UI
    setupUI();
    connectSignals();

    // Initialize compilers
    initializeCompilers();
    
    // Initial floating size and position
    setMinimumSize(400, 300);  // Set minimum size for resizing
    resize(floatingWidth(), 500);
    if (parent) {
        m_floatingGeometry = QRect((parent->width() - floatingWidth()) / 2,
                                   (parent->height() - 500) / 2,
                                   floatingWidth(),
                                   500);
        setGeometry(m_floatingGeometry);
    }

    // Ensure editor is on top
    raise();
    m_floatHeight = height();
    
    // Start docked to bottom after a short delay to ensure proper initialization
    QTimer::singleShot(300, this, [this]() {
        setDockPosition(FloatingPanelBase::DockedBottom);
    });
}

FloatingCodeEditor::~FloatingCodeEditor() = default;

void FloatingCodeEditor::setupUI()
{
    // Call base class setup
    setupBaseUI("Code Editor");
    
    // Install event filter on scroll area for resize handling when docked
    if (m_scrollArea) {
        m_scrollArea->installEventFilter(this);
        m_scrollArea->setMouseTracking(true);
        m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }

    QVBoxLayout *layout = getContentLayout();
    layout->setSpacing(5);

    // Top controls section
    QWidget *topWidget = new QWidget();
    QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
    topLayout->setContentsMargins(0, 0, 0, 0);

    // Language selection
    QLabel *langLabel = new QLabel("Language:");
    langLabel->setStyleSheet("font-weight: bold; color: #333;");
    topLayout->addWidget(langLabel);

    m_languageCombo = new QComboBox();
    m_languageCombo->setMinimumWidth(100);

    // Compilers will be added by updateCompilerAvailability()

    topLayout->addWidget(m_languageCombo);

    // Add some spacing
    topLayout->addSpacing(20);

    // Compile button
    m_compileButton = new QPushButton("Compile");
    m_compileButton->setStyleSheet("QPushButton {"
                                   "   padding: 5px 16px;"
                                   "   background-color: #4285f4;"
                                   "   color: white;"
                                   "   border: none;"
                                   "   border-radius: 3px;"
                                   "   font-weight: bold;"
                                   "}"
                                   "QPushButton:hover {"
                                   "   background-color: #3367d6;"
                                   "}"
                                   "QPushButton:pressed {"
                                   "   background-color: #2851a3;"
                                   "}"
                                   "QPushButton:disabled {"
                                   "   background-color: #ccc;"
                                   "   color: #666;"
                                   "}");
    topLayout->addWidget(m_compileButton);

    topLayout->addStretch();

    // Dark mode toggle
    m_darkModeCheckBox = new QCheckBox("Dark Mode");
    m_darkModeCheckBox->setChecked(m_isDarkMode);
    topLayout->addWidget(m_darkModeCheckBox);
    
    // Add spacing before maximize button
    topLayout->addSpacing(10);
    
    // Maximize button
    m_maximizeButton = new QPushButton();
    m_maximizeButton->setFixedSize(24, 24);
    m_maximizeButton->setToolTip("Maximize");
    m_maximizeButton->setStyleSheet("QPushButton {"
                                    "   background-color: transparent;"
                                    "   border: 1px solid #ccc;"
                                    "   border-radius: 3px;"
                                    "   font-family: monospace;"
                                    "   font-size: 16px;"
                                    "   padding: 0px;"
                                    "}"
                                    "QPushButton:hover {"
                                    "   background-color: #e0e0e0;"
                                    "   border-color: #999;"
                                    "}"
                                    "QPushButton:pressed {"
                                    "   background-color: #d0d0d0;"
                                    "}");
    m_maximizeButton->setText("□");  // Unicode square symbol for maximize
    topLayout->addWidget(m_maximizeButton);

    layout->addWidget(topWidget);

    // Create splitter for code editor and results
    QSplitter *splitter = new QSplitter(Qt::Vertical);

    // Code editor with QScintilla
    m_codeEditor = new QsciScintilla();

    // Setup common editor features
    setupCommonEditorFeatures();

    // Create and set custom lexer
    m_lexer = new GPULanguageLexer(m_codeEditor);
    m_codeEditor->setLexer(m_lexer);

    // Apply initial theme (light mode by default)
    if (m_isDarkMode) {
        applyDarkTheme();
    } else {
        applyLightTheme();
    }

    // Initial highlighter setup
    updateHighlighter();

    // Set default code
    QString defaultCode;
    if (m_languageCombo->count() > 0 && m_languageCombo->isEnabled()) {
        QString firstLang = m_languageCombo->itemText(0);
        if (firstLang == "OpenCL") {
            defaultCode = "__kernel void example(__global float* input,\n"
                          "                      __global float* output,\n"
                          "                      const int size) {\n"
                          "    int idx = get_global_id(0);\n"
                          "    if (idx < size) {\n"
                          "        output[idx] = input[idx] * 2.0f;\n"
                          "    }\n"
                          "}";
        } else if (firstLang == "CUDA") {
            defaultCode = "__global__ void example(float* input,\n"
                          "                        float* output,\n"
                          "                        int size) {\n"
                          "    int idx = blockIdx.x * blockDim.x + threadIdx.x;\n"
                          "    if (idx < size) {\n"
                          "        output[idx] = input[idx] * 2.0f;\n"
                          "    }\n"
                          "}";
        } else if (firstLang == "Metal") {
            defaultCode = "#include <metal_stdlib>\n"
                          "using namespace metal;\n\n"
                          "kernel void example(device float* input [[buffer(0)]],\n"
                          "                    device float* output [[buffer(1)]],\n"
                          "                    uint idx [[thread_position_in_grid]],\n"
                          "                    uint size [[threads_per_grid]]) {\n"
                          "    if (idx < size) {\n"
                          "        output[idx] = input[idx] * 2.0f;\n"
                          "    }\n"
                          "}";
        }
    }

    m_codeEditor->setText(defaultCode);

    // Create results widget
    m_resultsWidget = new CompileResultsWidget();
    m_resultsWidget->hide();
    connect(m_resultsWidget,
            &CompileResultsWidget::closeRequested,
            this,
            &FloatingCodeEditor::onResultsCloseRequested);
    connect(m_resultsWidget,
            &CompileResultsWidget::messageClicked,
            this,
            &FloatingCodeEditor::onMessageClicked);

    splitter->addWidget(m_codeEditor);
    splitter->addWidget(m_resultsWidget);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);

    layout->addWidget(splitter, 1); // Give maximum space to editor

    // Initial size
    getContentWidget()->adjustSize();
    adjustSize();
}

void FloatingCodeEditor::setupCommonEditorFeatures()
{
    // Editor general settings
    m_codeEditor->setIndentationsUseTabs(false);
    m_codeEditor->setIndentationWidth(4);
    m_codeEditor->setAutoIndent(true);
    m_codeEditor->setBraceMatching(QsciScintilla::StrictBraceMatch);
    m_codeEditor->setCaretLineVisible(true);
    m_codeEditor->setCaretWidth(2);

    // Line numbers
    m_codeEditor->setMarginType(0, QsciScintilla::NumberMargin);
    m_codeEditor->setMarginWidth(0, "0000");
    m_codeEditor->setMarginLineNumbers(0, true);

    // Code folding
    m_codeEditor->setFolding(QsciScintilla::BoxedTreeFoldStyle);

    // Enable code completion
    m_codeEditor->setAutoCompletionSource(QsciScintilla::AcsAPIs);
    m_codeEditor->setAutoCompletionThreshold(2);
    m_codeEditor->setAutoCompletionCaseSensitivity(false);
    m_codeEditor->setAutoCompletionShowSingle(true);

    // Set font
    QFont codeFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    codeFont.setPointSize(10);
    m_codeEditor->setFont(codeFont);

    // Whitespace
    m_codeEditor->setWhitespaceVisibility(QsciScintilla::WsInvisible);
    m_codeEditor->setIndentationGuides(true);

    // Edge marker
    m_codeEditor->setEdgeMode(QsciScintilla::EdgeLine);
    m_codeEditor->setEdgeColumn(80);
}

void FloatingCodeEditor::updateLexerColors()
{
    if (!m_lexer)
        return;

    // Manually set all colors, papers, and fonts for each style
    for (int style = GPULanguageLexer::Default; style <= GPULanguageLexer::Preprocessor; ++style) {
        m_lexer->setColor(m_lexer->defaultColor(style), style);
        m_lexer->setPaper(m_lexer->defaultPaper(style), style);
        m_lexer->setFont(m_lexer->defaultFont(style), style);
    }
}

void FloatingCodeEditor::forceRefreshLexer()
{
    if (m_lexer && m_codeEditor) {
        // Store current cursor position and text
        int line, col;
        m_codeEditor->getCursorPosition(&line, &col);
        QString currentText = m_codeEditor->text();

        // Update lexer colors explicitly
        updateLexerColors();

        // Force complete refresh
        m_codeEditor->SendScintilla(QsciScintilla::SCI_CLEARDOCUMENTSTYLE);
        m_codeEditor->SendScintilla(QsciScintilla::SCI_STYLERESETDEFAULT);

        // Remove and re-add lexer
        m_codeEditor->setLexer(nullptr);
        QApplication::processEvents();
        m_codeEditor->setLexer(m_lexer);

        // Force recoloring
        m_codeEditor->SendScintilla(QsciScintilla::SCI_COLOURISE, 0, currentText.length());
        m_codeEditor->recolor();

        // Restore cursor position
        m_codeEditor->setCursorPosition(line, col);
    }
}

void FloatingCodeEditor::applyDarkTheme()
{
    // Update lexer mode first
    if (m_lexer) {
        m_lexer->setDarkMode(true);
    }

    // Dark theme colors for editor
    m_codeEditor->setPaper(QColor("#1e1e1e"));
    m_codeEditor->setColor(QColor("#d4d4d4"));

    // Margins
    m_codeEditor->setMarginsBackgroundColor(QColor("#2b2b2b"));
    m_codeEditor->setMarginsForegroundColor(QColor("#858585"));

    // Current line
    m_codeEditor->setCaretLineBackgroundColor(QColor("#2a2a2a"));
    m_codeEditor->setCaretForegroundColor(QColor("#ffffff"));

    // Selection
    m_codeEditor->setSelectionBackgroundColor(QColor("#264f78"));
    m_codeEditor->setSelectionForegroundColor(QColor("#ffffff"));

    // Indentation guides
    m_codeEditor->setIndentationGuidesBackgroundColor(QColor("#404040"));
    m_codeEditor->setIndentationGuidesForegroundColor(QColor("#404040"));

    // Edge
    m_codeEditor->setEdgeColor(QColor("#404040"));

    // Folding
    m_codeEditor->setFoldMarginColors(QColor("#2b2b2b"), QColor("#2b2b2b"));

    // Update lexer colors explicitly and force refresh
    updateLexerColors();
    forceRefreshLexer();
}

void FloatingCodeEditor::applyLightTheme()
{
    // Update lexer mode first
    if (m_lexer) {
        m_lexer->setDarkMode(false);
    }

    // Light theme colors for editor
    m_codeEditor->setPaper(QColor("#ffffff"));
    m_codeEditor->setColor(QColor("#000000"));

    // Margins
    m_codeEditor->setMarginsBackgroundColor(QColor("#f0f0f0"));
    m_codeEditor->setMarginsForegroundColor(QColor("#6e7681"));

    // Current line
    m_codeEditor->setCaretLineBackgroundColor(QColor("#f5f5f5"));
    m_codeEditor->setCaretForegroundColor(QColor("#000000"));

    // Selection
    m_codeEditor->setSelectionBackgroundColor(QColor("#add6ff"));
    m_codeEditor->setSelectionForegroundColor(QColor("#000000"));

    // Indentation guides
    m_codeEditor->setIndentationGuidesBackgroundColor(QColor("#d1d5da"));
    m_codeEditor->setIndentationGuidesForegroundColor(QColor("#d1d5da"));

    // Edge
    m_codeEditor->setEdgeColor(QColor("#d1d5da"));

    // Folding
    m_codeEditor->setFoldMarginColors(QColor("#f0f0f0"), QColor("#f0f0f0"));

    // Update lexer colors explicitly and force refresh
    updateLexerColors();
    forceRefreshLexer();
}

void FloatingCodeEditor::connectSignals()
{
    connect(m_languageCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &FloatingCodeEditor::onLanguageChanged);

    connect(m_compileButton, &QPushButton::clicked, this, &FloatingCodeEditor::onCompileClicked);

    connect(m_codeEditor, &QsciScintilla::textChanged, this, &FloatingCodeEditor::onCodeChanged);

    connect(m_darkModeCheckBox, &QCheckBox::toggled, this, &FloatingCodeEditor::onThemeToggled);
    
    connect(m_maximizeButton, &QPushButton::clicked, this, &FloatingCodeEditor::onMaximizeClicked);
}

void FloatingCodeEditor::onLanguageChanged(int index)
{
    Q_UNUSED(index)
    updateHighlighter();

    // Update example code based on language
    QString language = getCurrentLanguage();
    if (language == "OpenCL") {
        m_codeEditor->setText("__kernel void example(__global float* input,\n"
                              "                      __global float* output,\n"
                              "                      const int size) {\n"
                              "    int idx = get_global_id(0);\n"
                              "    if (idx < size) {\n"
                              "        output[idx] = input[idx] * 2.0f;\n"
                              "    }\n"
                              "}");
    } else if (language == "CUDA") {
        m_codeEditor->setText("__global__ void example(float* input,\n"
                              "                        float* output,\n"
                              "                        int size) {\n"
                              "    int idx = blockIdx.x * blockDim.x + threadIdx.x;\n"
                              "    if (idx < size) {\n"
                              "        output[idx] = input[idx] * 2.0f;\n"
                              "    }\n"
                              "}");
    } else if (language == "Metal") {
        m_codeEditor->setText("#include <metal_stdlib>\n"
                              "using namespace metal;\n\n"
                              "kernel void example(device float* input [[buffer(0)]],\n"
                              "                    device float* output [[buffer(1)]],\n"
                              "                    uint idx [[thread_position_in_grid]],\n"
                              "                    uint size [[threads_per_grid]]) {\n"
                              "    if (idx < size) {\n"
                              "        output[idx] = input[idx] * 2.0f;\n"
                              "    }\n"
                              "}");
    }

    emit languageChanged(getCurrentLanguage());
}

void FloatingCodeEditor::onCompileClicked()
{
    performCompilation();
    emit compileRequested(getCode(), getCurrentLanguage());
}

void FloatingCodeEditor::onCodeChanged()
{
    emit codeChanged();
}

void FloatingCodeEditor::onThemeToggled(bool checked)
{
    m_isDarkMode = checked;
    if (m_isDarkMode) {
        applyDarkTheme();
    } else {
        applyLightTheme();
    }

    // Additional delayed refresh as backup
    QTimer::singleShot(100, this, &FloatingCodeEditor::forceRefreshLexer);
}

void FloatingCodeEditor::onMaximizeClicked()
{
    toggleMaximize();
}

void FloatingCodeEditor::toggleMaximize()
{
    if (m_isMaximized) {
        restoreFromMaximized();
    } else {
        maximizePanel();
    }
}

void FloatingCodeEditor::maximizePanel()
{
    if (!parentWidget()) return;
    
    // Store current state
    m_preMaximizeDockPosition = dockPosition();
    m_preMaximizeDockedHeight = dockedHeight();
    m_preMaximizeDockedWidth = dockedWidth();
    
    if (isDocked()) {
        m_preMaximizeGeometry = m_floatingGeometry;
        m_preMaximizeFloatHeight = m_floatHeight;
    } else {
        m_preMaximizeGeometry = geometry();
        m_preMaximizeFloatHeight = height();
    }
    
    // Get parent dimensions
    QRect parentRect = parentWidget()->rect();
    
    // If docked, undock first
    if (isDocked()) {
        setDockPosition(FloatingPanelBase::Floating);
        QTimer::singleShot(0, this, [this, parentRect]() {
            setGeometry(0, 0, parentRect.width(), parentRect.height());
            setMinimumSize(parentRect.size());
            setMaximumSize(parentRect.size());
            m_maximizeButton->setText("◱");
            m_maximizeButton->setToolTip("Restore");
            raise();
            m_isMaximized = true;
        });
    } else {
        setGeometry(0, 0, parentRect.width(), parentRect.height());
        setMinimumSize(parentRect.size());
        setMaximumSize(parentRect.size());
        m_maximizeButton->setText("◱");
        m_maximizeButton->setToolTip("Restore");
        raise();
        m_isMaximized = true;
    }
}

void FloatingCodeEditor::restoreFromMaximized()
{
    // Restore size constraints
    setMinimumSize(QSize(0, 0));
    setMaximumSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
    
    // Unset fixed size
    setFixedSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    
    // Restore previous dock state
    if (m_preMaximizeDockPosition != FloatingPanelBase::Floating) {
        // Restore docked dimensions first
        setDockedHeight(m_preMaximizeDockedHeight);
        setDockedWidth(m_preMaximizeDockedWidth);
        
        // Set appropriate size before docking
        if (m_preMaximizeDockPosition == FloatingPanelBase::DockedLeft || 
            m_preMaximizeDockPosition == FloatingPanelBase::DockedRight) {
            setFixedWidth(m_preMaximizeDockedWidth);
        } else if (m_preMaximizeDockPosition == FloatingPanelBase::DockedBottom) {
            setFixedHeight(m_preMaximizeDockedHeight);
        }
        
        // Restore dock position
        setDockPosition(m_preMaximizeDockPosition);
    } else {
        // Restore floating state
        // First restore the proper floating size
        setFixedWidth(floatingWidth());
        setFixedHeight(m_preMaximizeGeometry.height());
        
        // Then restore position and size
        setGeometry(m_preMaximizeGeometry);
        
        // Update the stored floating geometry
        m_floatingGeometry = m_preMaximizeGeometry;
    }
    
    // Update button appearance
    m_maximizeButton->setText("□");  // Unicode maximize symbol
    m_maximizeButton->setToolTip("Maximize");
    
    m_isMaximized = false;
}

void FloatingCodeEditor::setMaximized(bool maximized)
{
    if (maximized != m_isMaximized) {
        toggleMaximize();
    }
}

void FloatingCodeEditor::updateHighlighter()
{
    QString language = getCurrentLanguage();
    m_lexer->setLanguageMode(language);

    // Force refresh with the new method
    forceRefreshLexer();

    // Setup auto-completion
    QsciAPIs *api = new QsciAPIs(m_lexer);

    if (language == "OpenCL") {
        QStringList keywords = {"__kernel",
                                "kernel",
                                "__global",
                                "global",
                                "__local",
                                "local",
                                "__constant",
                                "constant",
                                "__private",
                                "private",
                                "get_global_id",
                                "get_global_size",
                                "get_local_id",
                                "get_local_size",
                                "barrier",
                                "CLK_LOCAL_MEM_FENCE",
                                "CLK_GLOBAL_MEM_FENCE"};
        for (const QString &keyword : keywords) {
            api->add(keyword);
        }
    } else if (language == "CUDA") {
        QStringList keywords = {"__global__",
                                "__device__",
                                "__host__",
                                "__constant__",
                                "__shared__",
                                "threadIdx",
                                "blockIdx",
                                "blockDim",
                                "gridDim",
                                "warpSize",
                                "__syncthreads",
                                "atomicAdd",
                                "atomicSub"};
        for (const QString &keyword : keywords) {
            api->add(keyword);
        }
    } else if (language == "Metal") {
        QStringList keywords = {"kernel",
                                "vertex",
                                "fragment",
                                "device",
                                "constant",
                                "threadgroup",
                                "thread",
                                "metal",
                                "half",
                                "float2",
                                "float3",
                                "float4"};
        for (const QString &keyword : keywords) {
            api->add(keyword);
        }
    }

    api->prepare();
    m_lexer->setAPIs(api);
}

void FloatingCodeEditor::setCode(const QString &code)
{
    m_codeEditor->setText(code);
}

QString FloatingCodeEditor::getCode() const
{
    return m_codeEditor->text();
}

void FloatingCodeEditor::setLanguage(const QString &language)
{
    int index = m_supportedLanguages.indexOf(language);
    if (index >= 0) {
        m_languageCombo->setCurrentIndex(index);
    }
}

QString FloatingCodeEditor::getCurrentLanguage() const
{
    QString text = m_languageCombo->currentText();
    // Remove "(Not Available)" suffix if present
    if (text.endsWith(" (Not Available)")) {
        return text.left(text.indexOf(" (Not Available)"));
    }
    return text;
}

void FloatingCodeEditor::setReadOnly(bool readOnly)
{
    m_codeEditor->setReadOnly(readOnly);
    m_compileButton->setEnabled(!readOnly);
}

void FloatingCodeEditor::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !m_isMaximized) {
        if (!isDocked()) {
            // Handle resize for floating mode
            m_resizeEdge = getResizeEdge(event->pos());
            if (m_resizeEdge != NoEdge) {
                m_resizing = true;
                m_resizeStartPos = event->globalPosition().toPoint();
                m_resizeStartGeometry = geometry();
                event->accept();
                return;
            }
        }
    }
    
    // Pass to base class for dragging
    FloatingPanelBase::mousePressEvent(event);
}

void FloatingCodeEditor::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_isMaximized) {
        if (m_resizing && (event->buttons() & Qt::LeftButton)) {
            // Handle resizing
            QPoint delta = event->globalPosition().toPoint() - m_resizeStartPos;
            QRect newGeometry = m_resizeStartGeometry;
            
            switch (m_resizeEdge) {
                case TopEdge:
                    newGeometry.setTop(m_resizeStartGeometry.top() + delta.y());
                    break;
                case BottomEdge:
                    newGeometry.setBottom(m_resizeStartGeometry.bottom() + delta.y());
                    break;
                case LeftEdge:
                    newGeometry.setLeft(m_resizeStartGeometry.left() + delta.x());
                    break;
                case RightEdge:
                    newGeometry.setRight(m_resizeStartGeometry.right() + delta.x());
                    break;
                case TopLeftCorner:
                    newGeometry.setTopLeft(m_resizeStartGeometry.topLeft() + delta);
                    break;
                case TopRightCorner:
                    newGeometry.setTop(m_resizeStartGeometry.top() + delta.y());
                    newGeometry.setRight(m_resizeStartGeometry.right() + delta.x());
                    break;
                case BottomLeftCorner:
                    newGeometry.setBottom(m_resizeStartGeometry.bottom() + delta.y());
                    newGeometry.setLeft(m_resizeStartGeometry.left() + delta.x());
                    break;
                case BottomRightCorner:
                    newGeometry.setBottomRight(m_resizeStartGeometry.bottomRight() + delta);
                    break;
                default:
                    break;
            }
            
            // Apply minimum size constraints
            if (newGeometry.width() >= minimumWidth() && newGeometry.height() >= minimumHeight()) {
                setGeometry(newGeometry);
                
                // Update floating geometry for base class
                if (!isDocked()) {
                    m_floatingGeometry = newGeometry;
                    m_floatHeight = newGeometry.height();
                }
            }
            
            event->accept();
            return;
        } else if (!isDocked()) {
            // Update cursor based on position
            updateCursor(event->pos());
        }
    }
    
    // Pass to base class for dragging
    FloatingPanelBase::mouseMoveEvent(event);
}

void FloatingCodeEditor::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_resizing = false;
        m_resizeEdge = NoEdge;
        setCursor(Qt::ArrowCursor);
    }
    
    FloatingPanelBase::mouseReleaseEvent(event);
}

void FloatingCodeEditor::paintEvent(QPaintEvent *event)
{
    FloatingPanelBase::paintEvent(event);
    
    // Draw resize grip for docked bottom mode
    if (isDocked() && dockPosition() == DockedBottom && !m_isMaximized) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // Draw resize handle at top edge
        int handleHeight = 4;
        QRect handleRect(0, 0, width(), handleHeight);
        painter.fillRect(handleRect, QColor(200, 200, 200, 100));
        
        // Draw grip lines
        painter.setPen(QPen(QColor(150, 150, 150), 1));
        int centerX = width() / 2;
        int lineWidth = 30;
        for (int i = -1; i <= 1; i++) {
            int y = 2 + i;
            painter.drawLine(centerX - lineWidth/2, y, centerX + lineWidth/2, y);
        }
    }
}

bool FloatingCodeEditor::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_scrollArea && isDocked() && dockPosition() == DockedBottom && !m_isMaximized) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton && mouseEvent->pos().y() < RESIZE_MARGIN) {
                m_resizing = true;
                m_resizeStartPos = mouseEvent->globalPosition().toPoint();
                m_resizeStartHeight = dockedHeight();
                return true;
            }
        } else if (event->type() == QEvent::MouseMove) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (m_resizing && (mouseEvent->buttons() & Qt::LeftButton)) {
                int delta = m_resizeStartPos.y() - mouseEvent->globalPosition().toPoint().y();
                int newHeight = qMax(200, qMin(parentWidget()->height() - 100, m_resizeStartHeight + delta));
                setDockedHeight(newHeight);
                updatePosition();
                return true;
            } else if (mouseEvent->pos().y() < RESIZE_MARGIN) {
                m_scrollArea->setCursor(Qt::SizeVerCursor);
            } else {
                m_scrollArea->setCursor(Qt::ArrowCursor);
            }
        } else if (event->type() == QEvent::MouseButtonRelease) {
            m_resizing = false;
            m_scrollArea->setCursor(Qt::ArrowCursor);
        }
    }
    
    return FloatingPanelBase::eventFilter(obj, event);
}

void FloatingCodeEditor::moveEvent(QMoveEvent *event)
{
    FloatingPanelBase::moveEvent(event);
    
    // If we're maximized, prevent movement away from (0,0)
    if (m_isMaximized && (x() != 0 || y() != 0)) {
        move(0, 0);
    }
}

FloatingCodeEditor::ResizeEdge FloatingCodeEditor::getResizeEdge(const QPoint &pos)
{
    if (isDocked()) return NoEdge;
    
    QRect r = rect();
    bool onLeft = pos.x() < RESIZE_MARGIN;
    bool onRight = pos.x() > r.width() - RESIZE_MARGIN;
    bool onTop = pos.y() < RESIZE_MARGIN;
    bool onBottom = pos.y() > r.height() - RESIZE_MARGIN;
    
    if (onTop && onLeft) return TopLeftCorner;
    if (onTop && onRight) return TopRightCorner;
    if (onBottom && onLeft) return BottomLeftCorner;
    if (onBottom && onRight) return BottomRightCorner;
    if (onTop) return TopEdge;
    if (onBottom) return BottomEdge;
    if (onLeft) return LeftEdge;
    if (onRight) return RightEdge;
    
    return NoEdge;
}

void FloatingCodeEditor::updateCursor(const QPoint &pos)
{
    if (m_resizing || isDocked()) return;
    
    ResizeEdge edge = getResizeEdge(pos);
    switch (edge) {
        case TopEdge:
        case BottomEdge:
            setCursor(Qt::SizeVerCursor);
            break;
        case LeftEdge:
        case RightEdge:
            setCursor(Qt::SizeHorCursor);
            break;
        case TopLeftCorner:
        case BottomRightCorner:
            setCursor(Qt::SizeFDiagCursor);
            break;
        case TopRightCorner:
        case BottomLeftCorner:
            setCursor(Qt::SizeBDiagCursor);
            break;
        default:
            setCursor(Qt::ArrowCursor);
            break;
    }
}

void FloatingCodeEditor::initializeCompilers()
{
    // Always try to create all compilers
    // They will check availability at runtime

    // OpenCL Compiler
    {
        auto openclCompiler = std::make_unique<OpenCLCompiler>();
        if (openclCompiler->isAvailable()) {
            m_compilers["OpenCL"] = std::move(openclCompiler);
            qDebug() << "OpenCL compiler initialized successfully";
        } else {
            qDebug() << "OpenCL compiler not available:" << openclCompiler->getAvailabilityError();
            // Store unavailable compiler for error messages
            m_compilers["OpenCL"] = std::move(openclCompiler);
        }
    }

    // CUDA Compiler
    {
        auto cudaCompiler = std::make_unique<CUDACompiler>();
        if (cudaCompiler->isAvailable()) {
            m_compilers["CUDA"] = std::move(cudaCompiler);
            qDebug() << "CUDA compiler initialized successfully";
        } else {
            qDebug() << "CUDA compiler not available:" << cudaCompiler->getAvailabilityError();
            // Store unavailable compiler for error messages
            m_compilers["CUDA"] = std::move(cudaCompiler);
        }
    }

    // Metal Compiler
    {
        auto metalCompiler = std::make_unique<MetalCompiler>();
        if (metalCompiler->isAvailable()) {
            m_compilers["Metal"] = std::move(metalCompiler);
            qDebug() << "Metal compiler initialized successfully";
        } else {
            qDebug() << "Metal compiler not available:" << metalCompiler->getAvailabilityError();
            // Store unavailable compiler for error messages
            m_compilers["Metal"] = std::move(metalCompiler);
        }
    }

    // Update UI to show compiler availability
    updateCompilerAvailability();
}

void FloatingCodeEditor::updateCompilerAvailability()
{
    // Store current selection
    QString currentSelection = m_languageCombo->currentText();

    // Clear and rebuild combo box
    m_languageCombo->clear();

    bool hasAnyCompiler = false;
    for (const QString &lang : m_supportedLanguages) {
        auto it = m_compilers.find(lang);
        if (it != m_compilers.end() && it->second) {
            if (it->second->isAvailable()) {
                m_languageCombo->addItem(lang);
                hasAnyCompiler = true;
            } else {
                // Add disabled item to show what's not available
                QString itemText = lang + " (Not Available)";
                m_languageCombo->addItem(itemText);
                int index = m_languageCombo->count() - 1;

                // Get the model and disable the item
                QStandardItemModel *model = qobject_cast<QStandardItemModel *>(
                    m_languageCombo->model());
                if (model) {
                    QStandardItem *item = model->item(index);
                    if (item) {
                        item->setEnabled(false);
                        item->setToolTip(it->second->getAvailabilityError());
                    }
                }
            }
        }
    }

    // Restore selection if possible
    int index = m_languageCombo->findText(currentSelection);
    if (index >= 0 && m_languageCombo->itemText(index) == currentSelection) {
        m_languageCombo->setCurrentIndex(index);
    }

    // Enable/disable compile button
    m_compileButton->setEnabled(hasAnyCompiler);

    if (!hasAnyCompiler) {
        m_compileButton->setToolTip(
            "No GPU compilers are available. Please install appropriate drivers.");
    } else {
        m_compileButton->setToolTip("");
    }
}
void FloatingCodeEditor::performCompilation()
{
    QString language = getCurrentLanguage();
    QString code = getCode();

    // Clear previous results
    m_compiledBinary.clear();

    // Find compiler
    auto it = m_compilers.find(language);
    if (it == m_compilers.end() || !it->second) {
        m_resultsWidget->clear();
        m_resultsWidget->setCompilationStatus(false, "No compiler available for " + language);
        showCompileResults(true);
        return;
    }

    // Double-check availability
    if (!it->second->isAvailable()) {
        m_resultsWidget->clear();
        m_resultsWidget->setCompilationStatus(false, "Compiler not available");
        m_resultsWidget->setRawOutput(it->second->getAvailabilityError());
        showCompileResults(true);
        return;
    }

    // Disable compile button during compilation
    m_compileButton->setEnabled(false);
    m_compileButton->setText("Compiling...");

    // Perform compilation
    SimpleGPUCompiler::CompileResult result = it->second->compile(code);

    // Store compiled binary if successful
    if (result.success) {
        m_compiledBinary = result.compiledBinary;
    }

    // Update results widget
    m_resultsWidget->clear();
    m_resultsWidget->setCompilationStatus(result.success);
    m_resultsWidget->setRawOutput(result.buildLog);
    m_resultsWidget->addMessages(result.messages);

    // Show results
    showCompileResults(true);

    // Re-enable compile button
    m_compileButton->setEnabled(true);
    m_compileButton->setText("Compile");
}

void FloatingCodeEditor::showCompileResults(bool show)
{
    if (show) {
        m_resultsWidget->show();
        // Adjust splitter sizes if needed
        if (isDocked() && dockPosition() == DockedBottom && !m_isMaximized) {
            setDockedHeight(600); // Increase height when showing results
            updatePosition();
        }
    } else {
        m_resultsWidget->hide();
        // Restore original size
        if (isDocked() && dockPosition() == DockedBottom && !m_isMaximized) {
            setDockedHeight(450);
            updatePosition();
        }
    }
}

void FloatingCodeEditor::resizeEvent(QResizeEvent *event)
{
    FloatingPanelBase::resizeEvent(event);
    
    // If we're maximized and parent resized, update our size
    if (m_isMaximized && parentWidget()) {
        QRect parentRect = parentWidget()->rect();
        setGeometry(parentRect);
        setMinimumSize(parentRect.size());
        setMaximumSize(parentRect.size());
    }
}

void FloatingCodeEditor::onResultsCloseRequested()
{
    showCompileResults(false);
}

void FloatingCodeEditor::onMessageClicked(int line, int column)
{
    // Move cursor to the error location
    if (line > 0) {
        // QScintilla uses 0-based line numbers
        int sciLine = line - 1;
        m_codeEditor->setCursorPosition(sciLine, column);

        // Ensure the line is visible
        m_codeEditor->ensureLineVisible(sciLine);

        // Highlight the line temporarily
        m_codeEditor->setSelectionBackgroundColor(QColor(255, 200, 200));
        m_codeEditor->setSelection(sciLine, 0, sciLine, m_codeEditor->lineLength(sciLine));

        // Clear selection after a delay
        QTimer::singleShot(2000, [this]() {
            m_codeEditor->setSelection(-1, -1, -1, -1); // Clear selection
        });

        // Focus the editor
        m_codeEditor->setFocus();
    }
}