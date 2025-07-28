#include "FloatingCodeEditor.hpp"
#include "GraphEditorMainWindow.hpp"
#include <QApplication>
#include <QComboBox>
#include <QCheckBox>
#include <QDebug>
#include <QFont>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <Qsci/qsciapis.h>
#include <QRegularExpression>

// GPULanguageLexer Implementation
GPULanguageLexer::GPULanguageLexer(QObject *parent)
    : QsciLexerCustom(parent)
    , m_language("OpenCL")
    , m_isDarkMode(false)
{
    // C++ keywords
    m_keywords = {
        "auto", "break", "case", "char", "class", "const", "continue",
        "default", "delete", "do", "double", "else", "enum", "explicit",
        "export", "extern", "false", "float", "for", "friend", "goto",
        "if", "inline", "int", "long", "namespace", "new", "operator",
        "private", "protected", "public", "register", "return", "short",
        "signed", "sizeof", "static", "struct", "switch", "template",
        "this", "throw", "true", "try", "typedef", "typeid", "typename",
        "union", "unsigned", "using", "virtual", "void", "volatile",
        "while", "bool"
    };
    
    // Common GPU types
    m_types = {
        "size_t", "ptrdiff_t", "uint", "ushort", "uchar", "ulong",
        "int2", "int3", "int4", "int8", "int16",
        "float2", "float3", "float4", "float8", "float16",
        "uint2", "uint3", "uint4", "uint8", "uint16",
        "double2", "double3", "double4", "double8", "double16",
        "char2", "char3", "char4", "char8", "char16",
        "uchar2", "uchar3", "uchar4", "uchar8", "uchar16",
        "short2", "short3", "short4", "short8", "short16",
        "ushort2", "ushort3", "ushort4", "ushort8", "ushort16",
        "long2", "long3", "long4", "long8", "long16",
        "ulong2", "ulong3", "ulong4", "ulong8", "ulong16"
    };
}

const char *GPULanguageLexer::language() const
{
    return m_language.toUtf8().constData();
}

QString GPULanguageLexer::description(int style) const
{
    switch (style) {
        case Default: return "Default";
        case Comment: return "Comment";
        case CommentLine: return "Comment Line";
        case Number: return "Number";
        case Keyword: return "Keyword";
        case LanguageKeyword: return "Language Keyword";
        case String: return "String";
        case Operator: return "Operator";
        case Identifier: return "Identifier";
        case Preprocessor: return "Preprocessor";
        default: return "";
    }
}

QColor GPULanguageLexer::defaultColor(int style) const
{
    if (m_isDarkMode) {
        switch (style) {
            case Default: return QColor("#d4d4d4");
            case Comment:
            case CommentLine: return QColor("#608b4e");
            case Number: return QColor("#b5cea8");
            case Keyword: return QColor("#569cd6");
            case LanguageKeyword:
                if (m_language == "OpenCL") return QColor("#4fc1ff");
                else if (m_language == "CUDA") return QColor("#ff9f40");
                else if (m_language == "Metal") return QColor("#c586c0");
                return QColor("#4fc1ff");
            case String: return QColor("#ce9178");
            case Operator: return QColor("#d7ba7d");
            case Identifier: return QColor("#9cdcfe");
            case Preprocessor: return QColor("#c586c0");
            default: return QColor("#d4d4d4");
        }
    } else {
        switch (style) {
            case Default: return QColor("#000000");
            case Comment:
            case CommentLine: return QColor("#008000");
            case Number: return QColor("#09885a");
            case Keyword: return QColor("#0000ff");
            case LanguageKeyword:
                if (m_language == "OpenCL") return QColor("#0033b3");
                else if (m_language == "CUDA") return QColor("#ff6600");
                else if (m_language == "Metal") return QColor("#9b009b");
                return QColor("#0033b3");
            case String: return QColor("#a31515");
            case Operator: return QColor("#000000");
            case Identifier: return QColor("#001080");
            case Preprocessor: return QColor("#9b9b9b");
            default: return QColor("#000000");
        }
    }
}

QColor GPULanguageLexer::defaultPaper(int style) const
{
    Q_UNUSED(style)
    return m_isDarkMode ? QColor("#1e1e1e") : QColor("#ffffff");
}

QFont GPULanguageLexer::defaultFont(int style) const
{
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setPointSize(10);
    
    if (style == Keyword || style == LanguageKeyword) {
        font.setBold(true);
    }
    
    return font;
}

void GPULanguageLexer::setLanguageMode(const QString &language)
{
    m_language = language;
    m_languageKeywords.clear();
    m_builtinFunctions.clear();
    
    if (language == "OpenCL") {
        m_languageKeywords = {
            "__kernel", "kernel", "__global", "global", "__local", "local",
            "__constant", "constant", "__private", "private", "__read_only",
            "read_only", "__write_only", "write_only", "__read_write", "read_write"
        };
        
        m_builtinFunctions = {
            "get_global_id", "get_global_size", "get_local_id", "get_local_size",
            "get_group_id", "get_num_groups", "get_work_dim", "get_global_offset",
            "barrier", "mem_fence", "read_mem_fence", "write_mem_fence",
            "CLK_LOCAL_MEM_FENCE", "CLK_GLOBAL_MEM_FENCE",
            // Math functions
            "sin", "cos", "tan", "asin", "acos", "atan", "sinh", "cosh", "tanh",
            "exp", "log", "log2", "log10", "pow", "sqrt", "cbrt", "hypot",
            "fabs", "fmin", "fmax", "fma", "mad", "clamp", "mix", "step",
            "smoothstep", "sign", "cross", "dot", "distance", "length", "normalize",
            // Conversion functions
            "convert_char", "convert_uchar", "convert_short", "convert_ushort",
            "convert_int", "convert_uint", "convert_long", "convert_ulong",
            "convert_float", "convert_double",
            // Vector functions
            "vload", "vstore", "shuffle", "shuffle2"
        };
    } else if (language == "CUDA") {
        m_languageKeywords = {
            "__global__", "__device__", "__host__", "__constant__", "__shared__",
            "__restrict__", "__noinline__", "__forceinline__"
        };
        
        m_builtinFunctions = {
            "threadIdx", "blockIdx", "blockDim", "gridDim", "warpSize",
            "__syncthreads", "__threadfence", "__threadfence_block",
            "atomicAdd", "atomicSub", "atomicExch", "atomicMin", "atomicMax",
            "atomicInc", "atomicDec", "atomicCAS", "atomicAnd", "atomicOr", "atomicXor",
            // Math functions
            "sinf", "cosf", "tanf", "asinf", "acosf", "atanf", "atan2f",
            "sinhf", "coshf", "tanhf", "expf", "exp2f", "exp10f", "expm1f",
            "logf", "log2f", "log10f", "log1pf", "powf", "sqrtf", "rsqrtf",
            "cbrtf", "hypotf", "fabsf", "fminf", "fmaxf", "fmaf",
            "truncf", "roundf", "floorf", "ceilf",
            // CUDA runtime
            "cudaMalloc", "cudaFree", "cudaMemcpy", "cudaMemset",
            "cudaMallocManaged", "cudaDeviceSynchronize", "cudaGetLastError"
        };
    } else if (language == "Metal") {
        m_languageKeywords = {
            "kernel", "vertex", "fragment", "device", "constant", "threadgroup",
            "thread", "[[buffer]]", "[[texture]]", "[[sampler]]",
            "[[thread_position_in_grid]]", "[[threads_per_grid]]",
            "[[thread_position_in_threadgroup]]", "[[threadgroup_position_in_grid]]",
            "[[stage_in]]", "[[position]]", "metal", "half", "half2", "half3", "half4"
        };
        
        m_builtinFunctions = {
            "sin", "cos", "tan", "asin", "acos", "atan", "atan2",
            "sinh", "cosh", "tanh", "exp", "exp2", "exp10", "log", "log2", "log10",
            "pow", "sqrt", "rsqrt", "fabs", "fmin", "fmax", "fma",
            "clamp", "mix", "step", "smoothstep", "sign",
            "cross", "dot", "distance", "length", "normalize",
            "reflect", "refract", "all", "any",
            // Metal specific
            "threadgroup_barrier", "simdgroup_barrier",
            "atomic_store_explicit", "atomic_load_explicit",
            "atomic_exchange_explicit", "atomic_fetch_add_explicit"
        };
    }
}

void GPULanguageLexer::setDarkMode(bool dark)
{
    m_isDarkMode = dark;
}

bool GPULanguageLexer::isKeyword(const QString &word) const
{
    return m_keywords.contains(word);
}

bool GPULanguageLexer::isLanguageKeyword(const QString &word) const
{
    return m_languageKeywords.contains(word);
}

bool GPULanguageLexer::isBuiltinFunction(const QString &word) const
{
    return m_builtinFunctions.contains(word);
}

bool GPULanguageLexer::isBuiltinType(const QString &word) const
{
    return m_types.contains(word);
}

bool GPULanguageLexer::isNumber(const QString &text) const
{
    static QRegularExpression numberRegex("^[0-9]+(\\.[0-9]+)?[fFlLuU]?$");
    return numberRegex.match(text).hasMatch();
}

void GPULanguageLexer::styleText(int start, int end)
{
    if (!editor() || start >= end)
        return;

    // Get the text to style
    int len = end - start;
    if (len <= 0) return;
    
    char *data = new char[len + 1];
    int result = editor()->SendScintilla(QsciScintilla::SCI_GETTEXTRANGE, start, end, data);
    if (result < 0) {
        delete[] data;
        return;
    }
    
    // Start styling
    startStyling(start);

    int i = 0;
    while (i < len) {
        char ch = data[i];
        
        // Handle newlines and whitespace
        if (ch == '\r' || ch == '\n' || ch == ' ' || ch == '\t') {
            setStyling(1, Default);
            i++;
            continue;
        }
        
        // Single line comment
        if (i < len - 1 && ch == '/' && data[i + 1] == '/') {
            int commentLen = 2;
            i += 2;
            while (i < len && data[i] != '\n' && data[i] != '\r') {
                commentLen++;
                i++;
            }
            setStyling(commentLen, CommentLine);
            continue;
        }
        
        // Multi-line comment
        if (i < len - 1 && ch == '/' && data[i + 1] == '*') {
            int commentLen = 2;
            i += 2;
            while (i < len - 1) {
                if (data[i] == '*' && data[i + 1] == '/') {
                    commentLen += 2;
                    i += 2;
                    break;
                }
                commentLen++;
                i++;
            }
            setStyling(commentLen, Comment);
            continue;
        }
        
        // Preprocessor
        if (ch == '#') {
            int preprocLen = 1;
            i++;
            while (i < len && data[i] != '\n' && data[i] != '\r') {
                preprocLen++;
                i++;
            }
            setStyling(preprocLen, Preprocessor);
            continue;
        }
        
        // String literals
        if (ch == '"' || ch == '\'') {
            char quote = ch;
            int stringLen = 1;
            i++;
            while (i < len && data[i] != quote) {
                if (data[i] == '\\' && i + 1 < len) {
                    stringLen += 2;
                    i += 2;
                } else {
                    stringLen++;
                    i++;
                }
            }
            if (i < len) {
                stringLen++;
                i++;
            }
            setStyling(stringLen, String);
            continue;
        }
        
        // Numbers
        if ((ch >= '0' && ch <= '9') || (ch == '.' && i + 1 < len && data[i + 1] >= '0' && data[i + 1] <= '9')) {
            int numLen = 1;
            i++;
            while (i < len) {
                ch = data[i];
                if ((ch >= '0' && ch <= '9') || ch == '.' || 
                    ch == 'f' || ch == 'F' || ch == 'l' || ch == 'L' || 
                    ch == 'u' || ch == 'U' || ch == 'x' || ch == 'X' ||
                    (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F')) {
                    numLen++;
                    i++;
                } else {
                    break;
                }
            }
            setStyling(numLen, Number);
            continue;
        }
        
        // Identifiers and keywords
        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_') {
            int wordStart = i;
            int wordLen = 1;
            i++;
            while (i < len) {
                ch = data[i];
                if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || 
                    (ch >= '0' && ch <= '9') || ch == '_') {
                    wordLen++;
                    i++;
                } else {
                    break;
                }
            }
            
            // Extract the word
            QString word = QString::fromLatin1(data + wordStart, wordLen);
            
            if (isLanguageKeyword(word)) {
                setStyling(wordLen, LanguageKeyword);
            } else if (isKeyword(word)) {
                setStyling(wordLen, Keyword);
            } else if (isBuiltinType(word)) {
                setStyling(wordLen, Keyword);
            } else if (isBuiltinFunction(word)) {
                setStyling(wordLen, LanguageKeyword);
            } else {
                setStyling(wordLen, Identifier);
                // Track this identifier
                m_identifiers.insert(word);
            }
            continue;
        }
        
        // Metal attributes [[...]]
        if (ch == '[' && i + 1 < len && data[i + 1] == '[') {
            int attrLen = 2;
            i += 2;
            while (i < len - 1) {
                if (data[i] == ']' && data[i + 1] == ']') {
                    attrLen += 2;
                    i += 2;
                    break;
                }
                attrLen++;
                i++;
            }
            // Check if it's a known attribute
            QString attr = QString::fromLatin1(data + i - attrLen, attrLen);
            if (m_language == "Metal" && isLanguageKeyword(attr)) {
                setStyling(attrLen, LanguageKeyword);
            } else {
                setStyling(attrLen, Operator);
            }
            continue;
        }
        
        // Default: operators and other characters
        setStyling(1, Operator);
        i++;
    }
    
    delete[] data;
}

// FloatingCodeEditor Implementation
FloatingCodeEditor::FloatingCodeEditor(GraphEditorWindow *parent)
    : FloatingPanelBase(parent, "Code Editor")
    , m_languageCombo(nullptr)
    , m_codeEditor(nullptr)
    , m_compileButton(nullptr)
    , m_darkModeCheckBox(nullptr)
    , m_lexer(nullptr)
    , m_isDarkMode(false)  // Default to light mode
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
    m_languageCombo->addItems(m_supportedLanguages);
    m_languageCombo->setMinimumWidth(100);
    topLayout->addWidget(m_languageCombo);

    // Add some spacing
    topLayout->addSpacing(20);

    // Compile button
    m_compileButton = new QPushButton("Compile");
    m_compileButton->setStyleSheet(
        "QPushButton {"
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
        "}"
    );
    topLayout->addWidget(m_compileButton);

    topLayout->addStretch();

    // Dark mode toggle
    m_darkModeCheckBox = new QCheckBox("Dark Mode");
    m_darkModeCheckBox->setChecked(m_isDarkMode);
    topLayout->addWidget(m_darkModeCheckBox);

    layout->addWidget(topWidget);

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

    // Set default OpenCL code
    m_codeEditor->setText(
        "__kernel void example(__global float* input,\n"
        "                      __global float* output,\n"
        "                      const int size) {\n"
        "    int idx = get_global_id(0);\n"
        "    if (idx < size) {\n"
        "        output[idx] = input[idx] * 2.0f;\n"
        "    }\n"
        "}"
    );

    layout->addWidget(m_codeEditor, 1);  // Give maximum space to editor

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
        if (line.isEmpty() || line.startsWith("//")) continue;
        
        // Check for variable declarations (simplified)
        QRegularExpression declRegex;
        if (getCurrentLanguage() == "OpenCL") {
            declRegex.setPattern("(?:__global|__local|__private|__constant|global|local|private|constant)?\\s*(?:const\\s+)?(?:unsigned\\s+)?(?:char|uchar|short|ushort|int|uint|long|ulong|float|double|half|bool|size_t|ptrdiff_t|int2|int3|int4|float2|float3|float4|uint2|uint3|uint4|double2|double3|double4)(?:\\s*\\*)?\\s+(\\w+)");
        } else if (getCurrentLanguage() == "CUDA") {
            declRegex.setPattern("(?:__device__|__shared__|__constant__)?\\s*(?:const\\s+)?(?:unsigned\\s+)?(?:char|short|int|long|float|double|bool|size_t|dim3|int2|int3|int4|float2|float3|float4|uint2|uint3|uint4)(?:\\s*\\*)?\\s+(\\w+)");
        } else { // Metal
            declRegex.setPattern("(?:device|constant|threadgroup|thread)?\\s*(?:const\\s+)?(?:char|short|int|long|float|double|half|bool|size_t|int2|int3|int4|float2|float3|float4|uint2|uint3|uint4|half2|half3|half4)(?:\\s*\\*)?\\s+(\\w+)");
        }
        
        QRegularExpressionMatchIterator it = declRegex.globalMatch(line);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            declaredVariables.insert(match.captured(1));
        }
        
        // Check for undefined variables (simplified - looks for word usage)
        QRegularExpression usageRegex("\\b(\\w+)\\s*(?:\\[|\\.|\\(|\\)|;|,|\\+|-|\\*|/|=|<|>|&|\\||!)");
        QRegularExpressionMatchIterator usageIt = usageRegex.globalMatch(line);
        while (usageIt.hasNext()) {
            QRegularExpressionMatch match = usageIt.next();
            QString word = match.captured(1);
            
            // Skip if it's a keyword, type, or number
            if (m_lexer->isKeyword(word) || m_lexer->isLanguageKeyword(word) || 
                m_lexer->isBuiltinFunction(word) || m_lexer->isBuiltinType(word) ||
                word.at(0).isDigit()) {
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
                m_codeEditor->fillIndicatorRange(lineNum, columnStart, lineNum, columnEnd, m_errorIndicator);
                
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
            if (!line.endsWith(';') && !line.endsWith('{') && !line.endsWith('}') && 
                !line.startsWith('#') && !line.contains("//") && line.length() > 2) {
                if (line.contains("__kernel") || line.contains("if") || line.contains("for") || 
                    line.contains("while") || line.contains("else")) {
                    // These don't need semicolons
                } else {
                    m_codeEditor->fillIndicatorRange(lineNum, line.length(), lineNum, line.length() + 1, m_errorIndicator);
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

void FloatingCodeEditor::applyDarkTheme()
{
    // Dark theme colors
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
    
    // Update lexer
    if (m_lexer) {
        m_lexer->setDarkMode(true);
        m_codeEditor->setLexer(nullptr);
        m_codeEditor->setLexer(m_lexer);
    }
}

void FloatingCodeEditor::applyLightTheme()
{
    // Light theme colors
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
    
    // Update lexer
    if (m_lexer) {
        m_lexer->setDarkMode(false);
        m_codeEditor->setLexer(nullptr);
        m_codeEditor->setLexer(m_lexer);
    }
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
        m_codeEditor->setText(
            "__kernel void example(__global float* input,\n"
            "                      __global float* output,\n"
            "                      const int size) {\n"
            "    int idx = get_global_id(0);\n"
            "    if (idx < size) {\n"
            "        output[idx] = input[idx] * 2.0f;\n"
            "    }\n"
            "}"
        );
    } else if (language == "CUDA") {
        m_codeEditor->setText(
            "__global__ void example(float* input,\n"
            "                        float* output,\n"
            "                        int size) {\n"
            "    int idx = blockIdx.x * blockDim.x + threadIdx.x;\n"
            "    if (idx < size) {\n"
            "        output[idx] = input[idx] * 2.0f;\n"
            "    }\n"
            "}"
        );
    } else if (language == "Metal") {
        m_codeEditor->setText(
            "#include <metal_stdlib>\n"
            "using namespace metal;\n\n"
            "kernel void example(device float* input [[buffer(0)]],\n"
            "                    device float* output [[buffer(1)]],\n"
            "                    uint idx [[thread_position_in_grid]],\n"
            "                    uint size [[threads_per_grid]]) {\n"
            "    if (idx < size) {\n"
            "        output[idx] = input[idx] * 2.0f;\n"
            "    }\n"
            "}"
        );
    }
    
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

void FloatingCodeEditor::onThemeToggled(bool checked)
{
    m_isDarkMode = checked;
    if (m_isDarkMode) {
        applyDarkTheme();
    } else {
        applyLightTheme();
    }
}

void FloatingCodeEditor::updateHighlighter()
{
    QString language = getCurrentLanguage();
    m_lexer->setLanguageMode(language);
    
    // Force refresh
    m_codeEditor->setLexer(nullptr);
    m_codeEditor->setLexer(m_lexer);
    
    // Setup auto-completion
    QsciAPIs *api = new QsciAPIs(m_lexer);
    
    if (language == "OpenCL") {
        QStringList keywords = {
            "__kernel", "kernel", "__global", "global", "__local", "local",
            "__constant", "constant", "__private", "private",
            "get_global_id", "get_global_size", "get_local_id", "get_local_size",
            "barrier", "CLK_LOCAL_MEM_FENCE", "CLK_GLOBAL_MEM_FENCE"
        };
        for (const QString &keyword : keywords) {
            api->add(keyword);
        }
    } else if (language == "CUDA") {
        QStringList keywords = {
            "__global__", "__device__", "__host__", "__constant__", "__shared__",
            "threadIdx", "blockIdx", "blockDim", "gridDim", "warpSize",
            "__syncthreads", "atomicAdd", "atomicSub"
        };
        for (const QString &keyword : keywords) {
            api->add(keyword);
        }
    } else if (language == "Metal") {
        QStringList keywords = {
            "kernel", "vertex", "fragment", "device", "constant", "threadgroup",
            "thread", "metal", "half", "float2", "float3", "float4"
        };
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

#include "FloatingCodeEditor.moc"