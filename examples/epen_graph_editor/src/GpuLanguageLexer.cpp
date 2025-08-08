#include "GPULanguageLexer.hpp"
#include <QColor>
#include <QFont>
#include <QFontDatabase>
#include <QRegularExpression>
#include <Qsci/qsciscintilla.h>

GPULanguageLexer::GPULanguageLexer(QObject *parent)
    : QsciLexerCustom(parent)
    , m_language("OpenCL")
    , m_isDarkMode(false)
{
    // C++ keywords
    m_keywords = {"auto",    "break",     "case",    "char",     "class",     "const",  "continue",
                  "default", "delete",    "do",      "double",   "else",      "enum",   "explicit",
                  "export",  "extern",    "false",   "float",    "for",       "friend", "goto",
                  "if",      "inline",    "int",     "long",     "namespace", "new",    "operator",
                  "private", "protected", "public",  "register", "return",    "short",  "signed",
                  "sizeof",  "static",    "struct",  "switch",   "template",  "this",   "throw",
                  "true",    "try",       "typedef", "typeid",   "typename",  "union",  "unsigned",
                  "using",   "virtual",   "void",    "volatile", "while",     "bool"};

    // Common GPU types
    m_types = {"size_t",  "ptrdiff_t", "uint",    "ushort",   "uchar",    "ulong",   "int2",
               "int3",    "int4",      "int8",    "int16",    "float2",   "float3",  "float4",
               "float8",  "float16",   "uint2",   "uint3",    "uint4",    "uint8",   "uint16",
               "double2", "double3",   "double4", "double8",  "double16", "char2",   "char3",
               "char4",   "char8",     "char16",  "uchar2",   "uchar3",   "uchar4",  "uchar8",
               "uchar16", "short2",    "short3",  "short4",   "short8",   "short16", "ushort2",
               "ushort3", "ushort4",   "ushort8", "ushort16", "long2",    "long3",   "long4",
               "long8",   "long16",    "ulong2",  "ulong3",   "ulong4",   "ulong8",  "ulong16"};
}

const char *GPULanguageLexer::language() const
{
    return m_language.toUtf8().constData();
}

QString GPULanguageLexer::description(int style) const
{
    switch (style) {
    case Default:
        return "Default";
    case Comment:
        return "Comment";
    case CommentLine:
        return "Comment Line";
    case Number:
        return "Number";
    case Keyword:
        return "Keyword";
    case LanguageKeyword:
        return "Language Keyword";
    case String:
        return "String";
    case Operator:
        return "Operator";
    case Identifier:
        return "Identifier";
    case Preprocessor:
        return "Preprocessor";
    default:
        return "";
    }
}

QColor GPULanguageLexer::defaultColor(int style) const
{
    if (m_isDarkMode) {
        switch (style) {
        case Default:
            return QColor("#d4d4d4");
        case Comment:
        case CommentLine:
            return QColor("#608b4e");
        case Number:
            return QColor("#b5cea8");
        case Keyword:
            return QColor("#569cd6");
        case LanguageKeyword:
            if (m_language == "OpenCL")
                return QColor("#4fc1ff");
            else if (m_language == "CUDA")
                return QColor("#ff9f40");
            else if (m_language == "Metal")
                return QColor("#c586c0");
            return QColor("#4fc1ff");
        case String:
            return QColor("#ce9178");
        case Operator:
            return QColor("#d7ba7d");
        case Identifier:
            return QColor("#9cdcfe");
        case Preprocessor:
            return QColor("#c586c0");
        default:
            return QColor("#d4d4d4");
        }
    } else {
        switch (style) {
        case Default:
            return QColor("#000000");
        case Comment:
        case CommentLine:
            return QColor("#008000");
        case Number:
            return QColor("#09885a");
        case Keyword:
            return QColor("#0000ff");
        case LanguageKeyword:
            if (m_language == "OpenCL")
                return QColor("#0033b3");
            else if (m_language == "CUDA")
                return QColor("#ff6600");
            else if (m_language == "Metal")
                return QColor("#9b009b");
            return QColor("#0033b3");
        case String:
            return QColor("#a31515");
        case Operator:
            return QColor("#000000");
        case Identifier:
            return QColor("#001080");
        case Preprocessor:
            return QColor("#9b9b9b");
        default:
            return QColor("#000000");
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
        m_languageKeywords = {"__kernel",
                              "kernel",
                              "__global",
                              "global",
                              "__local",
                              "local",
                              "__constant",
                              "constant",
                              "__private",
                              "private",
                              "__read_only",
                              "read_only",
                              "__write_only",
                              "write_only",
                              "__read_write",
                              "read_write"};

        m_builtinFunctions = {"get_global_id",
                              "get_global_size",
                              "get_local_id",
                              "get_local_size",
                              "get_group_id",
                              "get_num_groups",
                              "get_work_dim",
                              "get_global_offset",
                              "barrier",
                              "mem_fence",
                              "read_mem_fence",
                              "write_mem_fence",
                              "CLK_LOCAL_MEM_FENCE",
                              "CLK_GLOBAL_MEM_FENCE",
                              // Math functions
                              "sin",
                              "cos",
                              "tan",
                              "asin",
                              "acos",
                              "atan",
                              "sinh",
                              "cosh",
                              "tanh",
                              "exp",
                              "log",
                              "log2",
                              "log10",
                              "pow",
                              "sqrt",
                              "cbrt",
                              "hypot",
                              "fabs",
                              "fmin",
                              "fmax",
                              "fma",
                              "mad",
                              "clamp",
                              "mix",
                              "step",
                              "smoothstep",
                              "sign",
                              "cross",
                              "dot",
                              "distance",
                              "length",
                              "normalize",
                              // Conversion functions
                              "convert_char",
                              "convert_uchar",
                              "convert_short",
                              "convert_ushort",
                              "convert_int",
                              "convert_uint",
                              "convert_long",
                              "convert_ulong",
                              "convert_float",
                              "convert_double",
                              // Vector functions
                              "vload",
                              "vstore",
                              "shuffle",
                              "shuffle2"};
    } else if (language == "CUDA") {
        m_languageKeywords = {"__global__",
                              "__device__",
                              "__host__",
                              "__constant__",
                              "__shared__",
                              "__restrict__",
                              "__noinline__",
                              "__forceinline__"};

        m_builtinFunctions = {"threadIdx",
                              "blockIdx",
                              "blockDim",
                              "gridDim",
                              "warpSize",
                              "__syncthreads",
                              "__threadfence",
                              "__threadfence_block",
                              "atomicAdd",
                              "atomicSub",
                              "atomicExch",
                              "atomicMin",
                              "atomicMax",
                              "atomicInc",
                              "atomicDec",
                              "atomicCAS",
                              "atomicAnd",
                              "atomicOr",
                              "atomicXor",
                              // Math functions
                              "sinf",
                              "cosf",
                              "tanf",
                              "asinf",
                              "acosf",
                              "atanf",
                              "atan2f",
                              "sinhf",
                              "coshf",
                              "tanhf",
                              "expf",
                              "exp2f",
                              "exp10f",
                              "expm1f",
                              "logf",
                              "log2f",
                              "log10f",
                              "log1pf",
                              "powf",
                              "sqrtf",
                              "rsqrtf",
                              "cbrtf",
                              "hypotf",
                              "fabsf",
                              "fminf",
                              "fmaxf",
                              "fmaf",
                              "truncf",
                              "roundf",
                              "floorf",
                              "ceilf",
                              // CUDA runtime
                              "cudaMalloc",
                              "cudaFree",
                              "cudaMemcpy",
                              "cudaMemset",
                              "cudaMallocManaged",
                              "cudaDeviceSynchronize",
                              "cudaGetLastError"};
    } else if (language == "Metal") {
        m_languageKeywords = {"kernel",
                              "vertex",
                              "fragment",
                              "device",
                              "constant",
                              "threadgroup",
                              "thread",
                              "[[buffer]]",
                              "[[texture]]",
                              "[[sampler]]",
                              "[[thread_position_in_grid]]",
                              "[[threads_per_grid]]",
                              "[[thread_position_in_threadgroup]]",
                              "[[threadgroup_position_in_grid]]",
                              "[[stage_in]]",
                              "[[position]]",
                              "metal",
                              "half",
                              "half2",
                              "half3",
                              "half4"};

        m_builtinFunctions = {"sin",
                              "cos",
                              "tan",
                              "asin",
                              "acos",
                              "atan",
                              "atan2",
                              "sinh",
                              "cosh",
                              "tanh",
                              "exp",
                              "exp2",
                              "exp10",
                              "log",
                              "log2",
                              "log10",
                              "pow",
                              "sqrt",
                              "rsqrt",
                              "fabs",
                              "fmin",
                              "fmax",
                              "fma",
                              "clamp",
                              "mix",
                              "step",
                              "smoothstep",
                              "sign",
                              "cross",
                              "dot",
                              "distance",
                              "length",
                              "normalize",
                              "reflect",
                              "refract",
                              "all",
                              "any",
                              // Metal specific
                              "threadgroup_barrier",
                              "simdgroup_barrier",
                              "atomic_store_explicit",
                              "atomic_load_explicit",
                              "atomic_exchange_explicit",
                              "atomic_fetch_add_explicit"};
    }
}

void GPULanguageLexer::setDarkMode(bool dark)
{
    if (m_isDarkMode != dark) {
        m_isDarkMode = dark;
        // Emit property changed signal to trigger lexer refresh
        emit propertyChanged("font", "");
        emit propertyChanged("color", "");
    }
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
    if (len <= 0)
        return;

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
        if ((ch >= '0' && ch <= '9')
            || (ch == '.' && i + 1 < len && data[i + 1] >= '0' && data[i + 1] <= '9')) {
            int numLen = 1;
            i++;
            while (i < len) {
                ch = data[i];
                if ((ch >= '0' && ch <= '9') || ch == '.' || ch == 'f' || ch == 'F' || ch == 'l'
                    || ch == 'L' || ch == 'u' || ch == 'U' || ch == 'x' || ch == 'X'
                    || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F')) {
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
                if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9')
                    || ch == '_') {
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