#include "FloatingCodeEditor.hpp"
#include "GraphEditorMainWindow.hpp"
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFont>
#include <QFontDatabase>
#include <QRegularExpression>
#include <QTextDocument>
#include <QApplication>
#include <QDebug>

FloatingCodeEditor::FloatingCodeEditor(GraphEditorWindow *parent)
    : FloatingPanelBase(parent, "Code Editor")
    , m_languageCombo(nullptr)
    , m_codeEditor(nullptr)
    , m_compileButton(nullptr)
{
    // Set panel-specific dimensions
    setFloatingWidth(500);
    setDockedWidth(600);
    setDockedHeight(350);  // Set height for bottom docking
    
    // Support bottom docking in addition to left/right
    setDockingDistance(40);
    
    // Setup languages
    m_supportedLanguages = {"OpenCL", "CUDA", "Metal"};
    
    // Initialize UI
    setupUI();
    connectSignals();
    
    // Initial floating size and position
    setFixedWidth(floatingWidth());
    setFixedHeight(400);
    if (parent) {
        m_floatingGeometry = QRect((parent->width() - floatingWidth()) / 2,
                                   (parent->height() - 400) / 2,
                                   floatingWidth(),
                                   400);
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
    
    // Language selection section
    QWidget *langWidget = new QWidget();
    QHBoxLayout *langLayout = new QHBoxLayout(langWidget);
    langLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel *langLabel = new QLabel("Language:");
    langLabel->setStyleSheet("font-weight: bold; color: #333;");
    langLayout->addWidget(langLabel);
    
    m_languageCombo = new QComboBox();
    m_languageCombo->addItems(m_supportedLanguages);
    m_languageCombo->setStyleSheet(
        "QComboBox {"
        "   padding: 4px 8px;"
        "   border: 1px solid #ccc;"
        "   border-radius: 3px;"
        "   background-color: white;"
        "}"
        "QComboBox:hover {"
        "   border-color: #999;"
        "}"
        "QComboBox:drop-down {"
        "   border: none;"
        "}"
        "QComboBox::down-arrow {"
        "   image: none;"
        "   border-left: 4px solid transparent;"
        "   border-right: 4px solid transparent;"
        "   border-top: 4px solid #666;"
        "   margin-right: 5px;"
        "}"
    );
    langLayout->addWidget(m_languageCombo);
    langLayout->addStretch();
    
    layout->addWidget(langWidget);
    
    // Code editor
    m_codeEditor = new QPlainTextEdit();
    
    // Set monospace font
    QFont codeFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    codeFont.setPointSize(10);
    m_codeEditor->setFont(codeFont);
    
    // Set editor style
    m_codeEditor->setStyleSheet(
        "QPlainTextEdit {"
        "   background-color: #2b2b2b;"
        "   color: #e0e0e0;"
        "   border: 1px solid #444;"
        "   border-radius: 3px;"
        "   padding: 5px;"
        "   selection-background-color: #3a5a7a;"
        "}"
        "QPlainTextEdit:focus {"
        "   border-color: #4285f4;"
        "}"
    );
    
    // Set tab width
    QFontMetrics metrics(codeFont);
    m_codeEditor->setTabStopDistance(4 * metrics.horizontalAdvance(' '));
    
    // Set default code
    m_codeEditor->setPlainText(
        "__kernel void example(__global float* input,\n"
        "                      __global float* output,\n"
        "                      const int size) {\n"
        "    int idx = get_global_id(0);\n"
        "    if (idx < size) {\n"
        "        output[idx] = input[idx] * 2.0f;\n"
        "    }\n"
        "}"
    );
    
    layout->addWidget(m_codeEditor, 1); // Make editor take available space
    
    // Compile button section
    QWidget *buttonWidget = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setContentsMargins(0, 5, 0, 0);
    
    buttonLayout->addStretch();
    
    m_compileButton = new QPushButton("Compile");
    m_compileButton->setStyleSheet(
        "QPushButton {"
        "   padding: 6px 20px;"
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
        "}"
    );
    buttonLayout->addWidget(m_compileButton);
    
    layout->addWidget(buttonWidget);
    
    // Initial highlighter
    updateHighlighter();
    
    // Initial size
    getContentWidget()->adjustSize();
    adjustSize();
}

void FloatingCodeEditor::connectSignals()
{
    connect(m_languageCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FloatingCodeEditor::onLanguageChanged);
    
    connect(m_compileButton, &QPushButton::clicked,
            this, &FloatingCodeEditor::onCompileClicked);
    
    connect(m_codeEditor, &QPlainTextEdit::textChanged,
            this, &FloatingCodeEditor::onCodeChanged);
}

void FloatingCodeEditor::onLanguageChanged(int index)
{
    Q_UNUSED(index)
    updateHighlighter();
    emit languageChanged(getCurrentLanguage());
}

void FloatingCodeEditor::onCompileClicked()
{
    emit compileRequested(getCode(), getCurrentLanguage());
}

void FloatingCodeEditor::onCodeChanged()
{
    emit codeChanged();
}

void FloatingCodeEditor::updateHighlighter()
{
    QString language = getCurrentLanguage();
    
    if (language == "OpenCL") {
        m_highlighter = std::make_unique<OpenCLHighlighter>(m_codeEditor->document());
    } else if (language == "CUDA") {
        m_highlighter = std::make_unique<CUDAHighlighter>(m_codeEditor->document());
    } else if (language == "Metal") {
        m_highlighter = std::make_unique<MetalHighlighter>(m_codeEditor->document());
    }
}

void FloatingCodeEditor::setCode(const QString &code)
{
    m_codeEditor->setPlainText(code);
}

QString FloatingCodeEditor::getCode() const
{
    return m_codeEditor->toPlainText();
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
    return m_languageCombo->currentText();
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

// CodeHighlighter implementation
CodeHighlighter::CodeHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    // Keyword format
    m_keywordFormat.setForeground(QColor(86, 156, 214)); // Blue
    m_keywordFormat.setFontWeight(QFont::Bold);
    
    // Type format
    m_typeFormat.setForeground(QColor(78, 201, 176)); // Cyan
    
    // Comment format
    m_commentFormat.setForeground(QColor(106, 153, 85)); // Green
    m_commentFormat.setFontItalic(true);
    
    // String format
    m_stringFormat.setForeground(QColor(206, 145, 120)); // Orange
    
    // Number format
    m_numberFormat.setForeground(QColor(181, 206, 168)); // Light green
    
    // Function format
    m_functionFormat.setForeground(QColor(220, 220, 170)); // Yellow
}

void CodeHighlighter::applyFormat(int start, int length, const QTextCharFormat &format)
{
    setFormat(start, length, format);
}

// OpenCL Highlighter
OpenCLHighlighter::OpenCLHighlighter(QTextDocument *parent)
    : CodeHighlighter(parent)
{
    m_keywords = {
        "__kernel", "__global", "__local", "__constant", "__private",
        "kernel", "global", "local", "constant", "private",
        "if", "else", "for", "while", "do", "switch", "case", "default",
        "break", "continue", "return", "goto", "typedef", "struct", "union",
        "enum", "sizeof", "volatile", "const", "static", "extern", "inline"
    };
    
    m_types = {
        "void", "char", "short", "int", "long", "float", "double",
        "uchar", "ushort", "uint", "ulong", "bool", "size_t", "ptrdiff_t",
        "float2", "float3", "float4", "float8", "float16",
        "int2", "int3", "int4", "int8", "int16",
        "uint2", "uint3", "uint4", "uint8", "uint16"
    };
}

void OpenCLHighlighter::highlightBlock(const QString &text)
{
    // Highlight keywords
    for (const QString &keyword : m_keywords) {
        QRegularExpression expression("\\b" + keyword + "\\b");
        QRegularExpressionMatchIterator it = expression.globalMatch(text);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), m_keywordFormat);
        }
    }
    
    // Highlight types
    for (const QString &type : m_types) {
        QRegularExpression expression("\\b" + type + "\\b");
        QRegularExpressionMatchIterator it = expression.globalMatch(text);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), m_typeFormat);
        }
    }
    
    // Highlight numbers
    QRegularExpression numberExpression("\\b[0-9]+(\\.[0-9]+)?[fFlLuU]?\\b");
    QRegularExpressionMatchIterator numberIt = numberExpression.globalMatch(text);
    while (numberIt.hasNext()) {
        QRegularExpressionMatch match = numberIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), m_numberFormat);
    }
    
    // Highlight strings
    QRegularExpression stringExpression("\".*\"|'.'");
    QRegularExpressionMatchIterator stringIt = stringExpression.globalMatch(text);
    while (stringIt.hasNext()) {
        QRegularExpressionMatch match = stringIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), m_stringFormat);
    }
    
    // Highlight single-line comments
    QRegularExpression commentExpression("//[^\n]*");
    QRegularExpressionMatchIterator commentIt = commentExpression.globalMatch(text);
    while (commentIt.hasNext()) {
        QRegularExpressionMatch match = commentIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), m_commentFormat);
    }
    
    // Highlight multi-line comments
    setCurrentBlockState(0);
    
    QRegularExpression startExpression("/\\*");
    QRegularExpression endExpression("\\*/");
    
    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(startExpression);
    
    while (startIndex >= 0) {
        QRegularExpressionMatch endMatch = endExpression.match(text, startIndex);
        int endIndex = endMatch.capturedStart();
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + endMatch.capturedLength();
        }
        setFormat(startIndex, commentLength, m_commentFormat);
        startIndex = text.indexOf(startExpression, startIndex + commentLength);
    }
}

// CUDA Highlighter
CUDAHighlighter::CUDAHighlighter(QTextDocument *parent)
    : CodeHighlighter(parent)
{
    m_keywords = {
        "if", "else", "for", "while", "do", "switch", "case", "default",
        "break", "continue", "return", "goto", "typedef", "struct", "union",
        "enum", "sizeof", "volatile", "const", "static", "extern", "inline",
        "template", "class", "public", "private", "protected", "namespace"
    };
    
    m_types = {
        "void", "char", "short", "int", "long", "float", "double",
        "unsigned", "signed", "bool", "size_t", "ptrdiff_t",
        "float2", "float3", "float4", "double2", "double3", "double4",
        "int2", "int3", "int4", "uint2", "uint3", "uint4"
    };
    
    m_cudaSpecific = {
        "__global__", "__device__", "__host__", "__shared__", "__constant__",
        "__restrict__", "__forceinline__", "__noinline__",
        "blockIdx", "blockDim", "threadIdx", "gridDim",
        "warpSize", "syncthreads", "__syncthreads", "__threadfence"
    };
}

void CUDAHighlighter::highlightBlock(const QString &text)
{
    // Highlight keywords
    for (const QString &keyword : m_keywords) {
        QRegularExpression expression("\\b" + keyword + "\\b");
        QRegularExpressionMatchIterator it = expression.globalMatch(text);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), m_keywordFormat);
        }
    }
    
    // Highlight types
    for (const QString &type : m_types) {
        QRegularExpression expression("\\b" + type + "\\b");
        QRegularExpressionMatchIterator it = expression.globalMatch(text);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), m_typeFormat);
        }
    }
    
    // Highlight CUDA-specific
    for (const QString &cuda : m_cudaSpecific) {
        QRegularExpression expression("\\b" + cuda + "\\b");
        QRegularExpressionMatchIterator it = expression.globalMatch(text);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), m_functionFormat);
        }
    }
    
    // Rest of highlighting similar to OpenCL...
    // (Reuse the same number, string, and comment highlighting logic)
    
    // Highlight numbers
    QRegularExpression numberExpression("\\b[0-9]+(\\.[0-9]+)?[fFlLuU]?\\b");
    QRegularExpressionMatchIterator numberIt = numberExpression.globalMatch(text);
    while (numberIt.hasNext()) {
        QRegularExpressionMatch match = numberIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), m_numberFormat);
    }
    
    // Highlight strings
    QRegularExpression stringExpression("\".*\"|'.'");
    QRegularExpressionMatchIterator stringIt = stringExpression.globalMatch(text);
    while (stringIt.hasNext()) {
        QRegularExpressionMatch match = stringIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), m_stringFormat);
    }
    
    // Highlight comments
    QRegularExpression commentExpression("//[^\n]*");
    QRegularExpressionMatchIterator commentIt = commentExpression.globalMatch(text);
    while (commentIt.hasNext()) {
        QRegularExpressionMatch match = commentIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), m_commentFormat);
    }
}

// Metal Highlighter
MetalHighlighter::MetalHighlighter(QTextDocument *parent)
    : CodeHighlighter(parent)
{
    m_keywords = {
        "if", "else", "for", "while", "do", "switch", "case", "default",
        "break", "continue", "return", "goto", "typedef", "struct", "union",
        "enum", "sizeof", "volatile", "const", "static", "extern", "inline",
        "class", "public", "private", "namespace", "using", "template"
    };
    
    m_types = {
        "void", "char", "short", "int", "long", "float", "double", "half",
        "bool", "size_t", "ptrdiff_t", "uint", "ushort", "uchar",
        "float2", "float3", "float4", "half2", "half3", "half4",
        "int2", "int3", "int4", "uint2", "uint3", "uint4",
        "packed_float2", "packed_float3", "packed_float4"
    };
    
    m_metalSpecific = {
        "kernel", "vertex", "fragment", "device", "constant", "threadgroup",
        "thread", "[[buffer]]", "[[texture]]", "[[sampler]]", "[[position]]",
        "[[stage_in]]", "[[thread_position_in_grid]]", "[[thread_position_in_threadgroup]]",
        "[[threads_per_threadgroup]]", "[[threadgroup_position_in_grid]]",
        "texture2d", "texture3d", "sampler", "metal"
    };
}

void MetalHighlighter::highlightBlock(const QString &text)
{
    // Similar implementation to CUDA highlighter
    // Highlight keywords
    for (const QString &keyword : m_keywords) {
        QRegularExpression expression("\\b" + keyword + "\\b");
        QRegularExpressionMatchIterator it = expression.globalMatch(text);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), m_keywordFormat);
        }
    }
    
    // Highlight types
    for (const QString &type : m_types) {
        QRegularExpression expression("\\b" + type + "\\b");
        QRegularExpressionMatchIterator it = expression.globalMatch(text);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), m_typeFormat);
        }
    }
    
    // Highlight Metal-specific
    for (const QString &metal : m_metalSpecific) {
        QString pattern = metal;
        if (metal.startsWith("[[")) {
            pattern = QRegularExpression::escape(metal);
        } else {
            pattern = "\\b" + metal + "\\b";
        }
        QRegularExpression expression(pattern);
        QRegularExpressionMatchIterator it = expression.globalMatch(text);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), m_functionFormat);
        }
    }
    
    // Highlight numbers
    QRegularExpression numberExpression("\\b[0-9]+(\\.[0-9]+)?[fFhH]?\\b");
    QRegularExpressionMatchIterator numberIt = numberExpression.globalMatch(text);
    while (numberIt.hasNext()) {
        QRegularExpressionMatch match = numberIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), m_numberFormat);
    }
    
    // Highlight strings
    QRegularExpression stringExpression("\".*\"|'.'");
    QRegularExpressionMatchIterator stringIt = stringExpression.globalMatch(text);
    while (stringIt.hasNext()) {
        QRegularExpressionMatch match = stringIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), m_stringFormat);
    }
    
    // Highlight comments
    QRegularExpression commentExpression("//[^\n]*");
    QRegularExpressionMatchIterator commentIt = commentExpression.globalMatch(text);
    while (commentIt.hasNext()) {
        QRegularExpressionMatch match = commentIt.next();
        setFormat(match.capturedStart(), match.capturedLength(), m_commentFormat);
    }
}