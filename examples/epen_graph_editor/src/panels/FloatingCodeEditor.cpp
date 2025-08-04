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
#include <QRegularExpression>
#include <QSplitter>
#include <QStandardItemModel>
#include <QVBoxLayout>
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
    , m_isDarkMode(false) // Default to light mode
    , m_errorCheckTimer(nullptr)
    , m_errorIndicator(0)
{
    // Set panel-specific dimensions
    setFloatingWidth(600);
    setDockedWidth(700);
    setDockedHeight(400);

    // Support bottom docking in addition to left/right
    setDockingDistance(40);

    // Setup languages
    m_supportedLanguages = {"OpenCL", "CUDA", "Metal"};

    // Initialize UI
    setupUI();
    connectSignals();

    // Initialize compilers
    initializeCompilers();
    // Initial floating size and position
    setFixedWidth(floatingWidth());
    setFixedHeight(500);
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
}

FloatingCodeEditor::~FloatingCodeEditor() = default;

void FloatingCodeEditor::setupUI()
{
    // Call base class setup
    setupBaseUI("Code Editor");

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

    // Setup error indicator
    setupErrorIndicator();

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

    // Setup error checking timer
    m_errorCheckTimer = new QTimer(this);
    m_errorCheckTimer->setSingleShot(true);
    m_errorCheckTimer->setInterval(500); // Check after 500ms of no typing

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

void FloatingCodeEditor::setupErrorIndicator()
{
    // Define a custom indicator for errors
    m_errorIndicator = 1; // Use indicator number 1

    // Set up the error indicator style (squiggly underline)
    m_codeEditor->indicatorDefine(QsciScintilla::SquiggleIndicator, m_errorIndicator);
    m_codeEditor->setIndicatorForegroundColor(QColor("#ff0000"), m_errorIndicator);
    m_codeEditor->setIndicatorDrawUnder(true, m_errorIndicator);
}

void FloatingCodeEditor::checkForErrors()
{
    // Clear previous error indicators
    m_codeEditor->clearIndicatorRange(0, 0, m_codeEditor->lines(), 0, m_errorIndicator);
    m_errors.clear();

    // Get all text
    QString text = m_codeEditor->text();
    QStringList lines = text.split('\n');

    // Clear lexer's identifier tracking
    m_lexer->clearIdentifiers();

    // Force a re-lex to collect identifiers
    m_codeEditor->recolor();

    // Get declared identifiers from lexer
    QSet<QString> declaredIdentifiers = m_lexer->getIdentifiers();

    // Track variable declarations
    QSet<QString> declaredVariables;
    QSet<QString> usedVariables;

    // Simple parsing to find declarations and usage
    for (int lineNum = 0; lineNum < lines.size(); ++lineNum) {
        QString line = lines[lineNum].trimmed();

        // Skip empty lines and comments
        if (line.isEmpty() || line.startsWith("//"))
            continue;

        // Check for variable declarations (simplified)
        QRegularExpression declRegex;
        if (getCurrentLanguage() == "OpenCL") {
            declRegex.setPattern(
                "(?:__global|__local|__private|__constant|global|local|private|constant)?\\s*(?:"
                "const\\s+)?(?:unsigned\\s+)?(?:char|uchar|short|ushort|int|uint|long|ulong|float|"
                "double|half|bool|size_t|ptrdiff_t|int2|int3|int4|float2|float3|float4|uint2|uint3|"
                "uint4|double2|double3|double4)(?:\\s*\\*)?\\s+(\\w+)");
        } else if (getCurrentLanguage() == "CUDA") {
            declRegex.setPattern(
                "(?:__device__|__shared__|__constant__)?\\s*(?:const\\s+)?(?:unsigned\\s+)?(?:char|"
                "short|int|long|float|double|bool|size_t|dim3|int2|int3|int4|float2|float3|float4|"
                "uint2|uint3|uint4)(?:\\s*\\*)?\\s+(\\w+)");
        } else if (getCurrentLanguage() == "Metal") {
            declRegex.setPattern(
                "(?:device|constant|threadgroup|thread)?\\s*(?:const\\s+)?(?:char|short|int|long|"
                "float|double|half|bool|size_t|int2|int3|int4|float2|float3|float4|uint2|uint3|"
                "uint4|half2|half3|half4)(?:\\s*\\*)?\\s+(\\w+)");
        }

        QRegularExpressionMatchIterator it = declRegex.globalMatch(line);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            declaredVariables.insert(match.captured(1));
        }

        // Check for undefined variables (simplified - looks for word usage)
        QRegularExpression usageRegex(
            "\\b(\\w+)\\s*(?:\\[|\\.|\\(|\\)|;|,|\\+|-|\\*|/|=|<|>|&|\\||!)");
        QRegularExpressionMatchIterator usageIt = usageRegex.globalMatch(line);
        while (usageIt.hasNext()) {
            QRegularExpressionMatch match = usageIt.next();
            QString word = match.captured(1);

            // Skip if it's a keyword, type, or number
            if (m_lexer->isKeyword(word) || m_lexer->isLanguageKeyword(word)
                || m_lexer->isBuiltinFunction(word) || m_lexer->isBuiltinType(word)
                || word.at(0).isDigit()) {
                continue;
            }

            // Check if variable is used but not declared
            if (!declaredVariables.contains(word) && !word.isEmpty()) {
                // Find the position in the line
                int columnStart = match.capturedStart(1);
                int columnEnd = match.capturedEnd(1);

                // Calculate absolute position
                int lineStartPos = 0;
                for (int i = 0; i < lineNum; ++i) {
                    lineStartPos += lines[i].length() + 1; // +1 for newline
                }

                int startPos = lineStartPos + columnStart;
                int endPos = lineStartPos + columnEnd;

                // Add error indicator
                m_codeEditor->fillIndicatorRange(lineNum,
                                                 columnStart,
                                                 lineNum,
                                                 columnEnd,
                                                 m_errorIndicator);

                // Store error info
                ErrorInfo error;
                error.line = lineNum;
                error.indexStart = startPos;
                error.indexEnd = endPos;
                error.message = QString("Undefined variable '%1'").arg(word);
                m_errors.append(error);
            }
        }

        // Check for language-specific syntax errors
        if (getCurrentLanguage() == "OpenCL") {
            // Check for missing semicolons
            if (!line.endsWith(';') && !line.endsWith('{') && !line.endsWith('}')
                && !line.startsWith('#') && !line.contains("//") && line.length() > 2) {
                if (line.contains("__kernel") || line.contains("if") || line.contains("for")
                    || line.contains("while") || line.contains("else")) {
                    // These don't need semicolons
                } else {
                    m_codeEditor->fillIndicatorRange(lineNum,
                                                     line.length(),
                                                     lineNum,
                                                     line.length() + 1,
                                                     m_errorIndicator);
                    ErrorInfo error;
                    error.line = lineNum;
                    error.message = "Missing semicolon";
                    m_errors.append(error);
                }
            }
        }
    }
}

void FloatingCodeEditor::onTextChanged()
{
    // Reset the timer whenever text changes
    m_errorCheckTimer->stop();
    m_errorCheckTimer->start();
}

void FloatingCodeEditor::performErrorCheck()
{
    checkForErrors();
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

    // Connect text change to error checking
    connect(m_codeEditor, &QsciScintilla::textChanged, this, &FloatingCodeEditor::onTextChanged);

    // Connect timer to error checking
    connect(m_errorCheckTimer, &QTimer::timeout, this, &FloatingCodeEditor::performErrorCheck);
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

void FloatingCodeEditor::resizeEvent(QResizeEvent *event)
{
    FloatingPanelBase::resizeEvent(event);
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
        if (isDocked() && dockPosition() == DockedBottom) {
            setDockedHeight(600); // Increase height when showing results
            updatePosition();
        }
    } else {
        m_resultsWidget->hide();
        // Restore original size
        if (isDocked() && dockPosition() == DockedBottom) {
            setDockedHeight(400);
            updatePosition();
        }
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