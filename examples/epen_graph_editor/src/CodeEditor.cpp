#include "CodeEditor.hpp"
#include "GPULanguageLexer.hpp"
#include "data_models/Process.hpp"
#include <QApplication>
#include <QFont>
#include <QFontDatabase>
#include <QTimer>
#include <QVBoxLayout>
#include <Qsci/qsciapis.h>

CodeEditor::CodeEditor(QWidget *parent)
    : QWidget(parent)
    , m_editor(nullptr)
    , m_lexer(nullptr)
    , m_currentLanguage("OpenCL")
    , m_isDarkMode(false)
    , _processNode(nullptr)
{
    // Setup supported languages
    m_supportedLanguages = {"OpenCL", "CUDA", "Metal"};

    // Setup UI
    setupEditor();

    // Set initial language and code
    setLanguage(m_currentLanguage);
}

CodeEditor::~CodeEditor() = default;

void CodeEditor::setupEditor()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // Create editor
    m_editor = new ReadOnlyLinesEditor(this);
    layout->addWidget(m_editor);

    // Setup common features
    setupCommonEditorFeatures();

    // Create and set custom lexer
    m_lexer = new GPULanguageLexer(m_editor);
    m_editor->setLexer(m_lexer);

    // Apply initial theme
    if (m_isDarkMode) {
        applyDarkTheme();
    } else {
        applyLightTheme();
    }

    // Connect signals
    connect(m_editor, &QsciScintilla::textChanged, this, &CodeEditor::onTextChanged);
}

void CodeEditor::setupCommonEditorFeatures()
{
    // Editor general settings
    m_editor->setIndentationsUseTabs(false);
    m_editor->setIndentationWidth(4);
    m_editor->setAutoIndent(true);
    m_editor->setBraceMatching(QsciScintilla::StrictBraceMatch);
    m_editor->setCaretLineVisible(true);
    m_editor->setCaretWidth(2);

    // Line numbers
    m_editor->setMarginType(0, QsciScintilla::NumberMargin);
    m_editor->setMarginWidth(0, "0000");
    m_editor->setMarginLineNumbers(0, true);

    // Code folding
    m_editor->setFolding(QsciScintilla::BoxedTreeFoldStyle);

    // Enable code completion
    m_editor->setAutoCompletionSource(QsciScintilla::AcsAPIs);
    m_editor->setAutoCompletionThreshold(2);
    m_editor->setAutoCompletionCaseSensitivity(false);
    m_editor->setAutoCompletionShowSingle(true);

    // Set font
    QFont codeFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    codeFont.setPointSize(10);
    m_editor->setFont(codeFont);

    // Whitespace
    m_editor->setWhitespaceVisibility(QsciScintilla::WsInvisible);
    m_editor->setIndentationGuides(true);

    // Edge marker
    m_editor->setEdgeMode(QsciScintilla::EdgeLine);
    m_editor->setEdgeColumn(80);
}

void CodeEditor::setCode(const QString &code)
{
    if (!_processNode)
        return;
    _codeUpdateLock = true;
    m_editor->setText(code);
    setReadOnlyLines();
    _codeUpdateLock = false;
}

QString CodeEditor::getCode() const
{
    return m_editor->text();
}

void CodeEditor::setLanguage(const QString &language)
{
    if (m_supportedLanguages.contains(language) && language != m_currentLanguage) {
        m_currentLanguage = language;
        updateHighlighter();

        // Set default code for the language
        setCode(getDefaultCode(language));

        emit languageChanged(language);
    }
}

QString CodeEditor::getCurrentLanguage() const
{
    return m_currentLanguage;
}

void CodeEditor::setReadOnly(bool readOnly)
{
    m_editor->setReadOnly(readOnly);
}

bool CodeEditor::isReadOnly() const
{
    return m_editor->isReadOnly();
}

void CodeEditor::setDarkMode(bool dark)
{
    if (m_isDarkMode != dark) {
        m_isDarkMode = dark;

        if (m_isDarkMode) {
            applyDarkTheme();
        } else {
            applyLightTheme();
        }

        // Additional delayed refresh as backup
        QTimer::singleShot(100, this, &CodeEditor::forceRefreshLexer);
    }
}

void CodeEditor::setCursorPosition(int line, int column)
{
    // QScintilla uses 0-based line numbers
    m_editor->setCursorPosition(line - 1, column);
}

void CodeEditor::getCursorPosition(int &line, int &column) const
{
    m_editor->getCursorPosition(&line, &column);
    line++; // Convert to 1-based for external use
}

void CodeEditor::ensureLineVisible(int line)
{
    m_editor->ensureLineVisible(line - 1);
}

void CodeEditor::highlightLine(int line, int duration)
{
    if (line > 0) {
        // QScintilla uses 0-based line numbers
        int sciLine = line - 1;

        // Highlight the line temporarily
        m_editor->setSelectionBackgroundColor(QColor(255, 200, 200));
        m_editor->setSelection(sciLine, 0, sciLine, m_editor->lineLength(sciLine));

        // Clear selection after a delay
        QTimer::singleShot(duration, [this]() {
            m_editor->setSelection(-1, -1, -1, -1); // Clear selection
        });
    }
}

void CodeEditor::setFocus()
{
    m_editor->setFocus();
}

void CodeEditor::updateHighlighter()
{
    m_lexer->setLanguageMode(m_currentLanguage);

    // Force refresh with the new method
    forceRefreshLexer();

    // Setup auto-completion
    setupAutoCompletion();
}

void CodeEditor::setupAutoCompletion()
{
    QsciAPIs *api = new QsciAPIs(m_lexer);

    if (m_currentLanguage == "OpenCL") {
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
    } else if (m_currentLanguage == "CUDA") {
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
    } else if (m_currentLanguage == "Metal") {
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

void CodeEditor::updateLexerColors()
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

void CodeEditor::forceRefreshLexer()
{
    if (m_lexer && m_editor) {
        // Store current cursor position and text
        int line, col;
        m_editor->getCursorPosition(&line, &col);
        QString currentText = m_editor->text();

        // Update lexer colors explicitly
        updateLexerColors();

        // Force complete refresh
        m_editor->SendScintilla(QsciScintilla::SCI_CLEARDOCUMENTSTYLE);
        m_editor->SendScintilla(QsciScintilla::SCI_STYLERESETDEFAULT);

        // Remove and re-add lexer
        m_editor->setLexer(nullptr);
        QApplication::processEvents();
        m_editor->setLexer(m_lexer);

        // Force recoloring
        m_editor->SendScintilla(QsciScintilla::SCI_COLOURISE, 0, currentText.length());
        m_editor->recolor();

        // Restore cursor position
        m_editor->setCursorPosition(line, col);
    }
}

void CodeEditor::applyDarkTheme()
{
    // Update lexer mode first
    if (m_lexer) {
        m_lexer->setDarkMode(true);
    }

    // Dark theme colors for editor
    m_editor->setPaper(QColor("#1e1e1e"));
    m_editor->setColor(QColor("#d4d4d4"));

    // Margins
    m_editor->setMarginsBackgroundColor(QColor("#2b2b2b"));
    m_editor->setMarginsForegroundColor(QColor("#858585"));

    // Current line
    m_editor->setCaretLineBackgroundColor(QColor("#2a2a2a"));
    m_editor->setCaretForegroundColor(QColor("#ffffff"));

    // Selection
    m_editor->setSelectionBackgroundColor(QColor("#264f78"));
    m_editor->setSelectionForegroundColor(QColor("#ffffff"));

    // Indentation guides
    m_editor->setIndentationGuidesBackgroundColor(QColor("#404040"));
    m_editor->setIndentationGuidesForegroundColor(QColor("#404040"));

    // Edge
    m_editor->setEdgeColor(QColor("#404040"));

    // Folding
    m_editor->setFoldMarginColors(QColor("#2b2b2b"), QColor("#2b2b2b"));

    // Update lexer colors explicitly and force refresh
    updateLexerColors();
    forceRefreshLexer();
}

void CodeEditor::applyLightTheme()
{
    // Update lexer mode first
    if (m_lexer) {
        m_lexer->setDarkMode(false);
    }

    // Light theme colors for editor
    m_editor->setPaper(QColor("#ffffff"));
    m_editor->setColor(QColor("#000000"));

    // Margins
    m_editor->setMarginsBackgroundColor(QColor("#f0f0f0"));
    m_editor->setMarginsForegroundColor(QColor("#6e7681"));

    // Current line
    m_editor->setCaretLineBackgroundColor(QColor("#f5f5f5"));
    m_editor->setCaretForegroundColor(QColor("#000000"));

    // Selection
    m_editor->setSelectionBackgroundColor(QColor("#add6ff"));
    m_editor->setSelectionForegroundColor(QColor("#000000"));

    // Indentation guides
    m_editor->setIndentationGuidesBackgroundColor(QColor("#d1d5da"));
    m_editor->setIndentationGuidesForegroundColor(QColor("#d1d5da"));

    // Edge
    m_editor->setEdgeColor(QColor("#d1d5da"));

    // Folding
    m_editor->setFoldMarginColors(QColor("#f0f0f0"), QColor("#f0f0f0"));

    // Update lexer colors explicitly and force refresh
    updateLexerColors();
    forceRefreshLexer();
}

QString CodeEditor::getDefaultCode(const QString &language) const
{
    if (!_processNode)
        return "";
    if (language == "OpenCL") {
        return _processNode->getOpenclProgram();
    } else if (language == "CUDA") {
        return _processNode->getCudaProgram();
    } else if (language == "Metal") {
        return _processNode->getMetalProgram();
    }

    return "";
}

void CodeEditor::setProcessNode(Process *processNode)
{
    _processNode = processNode;
    processNode->setEditor(this);
    setCode(getDefaultCode(getCurrentLanguage()));
}

void CodeEditor::onTextChanged()
{
    if (_codeUpdateLock)
        return;
    if (!_processNode)
        return;
    QString language = getCurrentLanguage();
    if (language == "OpenCL") {
        _processNode->setOpenclProgram(getCode());
    } else if (language == "CUDA") {
        _processNode->setCudaProgram(getCode());
    } else if (language == "Metal") {
        _processNode->setMetalProgram(getCode());
    }
    setReadOnlyLines();
    emit codeChanged();
}

void CodeEditor::updateCode()
{
    QString language = getCurrentLanguage();
    if (!_processNode)
        return;

    if (language == "OpenCL") {
        setCode(_processNode->getOpenclProgram());
    } else if (language == "CUDA") {
        setCode(_processNode->getCudaProgram());
    } else if (language == "Metal") {
        setCode(_processNode->getMetalProgram());
    }
}

void CodeEditor::setReadOnlyLines()
{
    if (m_currentLanguage == "OpenCL") {
        m_editor->setReadonlyLines(_processNode->getOpenclReadonlyLines());
    } else if (m_currentLanguage == "CUDA") {
        m_editor->setReadonlyLines(_processNode->getCudaReadonlyLines());
    } else if (m_currentLanguage == "Metal") {
        m_editor->setReadonlyLines(_processNode->getMetalReadonlyLines());
    }
}