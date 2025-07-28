// FloatingCodeEditor.cpp
#include "FloatingCodeEditor.hpp"
#include "GraphEditorMainWindow.hpp"
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFont>
#include <QFontDatabase>
#include <QApplication>
#include <QDebug>
#include <Qsci/qscilexercpp.h>
#include <Qsci/qsciapis.h>

FloatingCodeEditor::FloatingCodeEditor(GraphEditorWindow *parent)
    : FloatingPanelBase(parent, "Code Editor")
    , m_languageCombo(nullptr)
    , m_codeEditor(nullptr)
    , m_compileButton(nullptr)
    , m_lexer(nullptr)
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
    
    // Code editor with QScintilla
    m_codeEditor = new QsciScintilla();
    
    // Setup common editor features
    setupCommonEditorFeatures();
    
    // Create and set C++ lexer (we'll use it for all GPU languages)
    m_lexer = new QsciLexerCPP(m_codeEditor);
    m_codeEditor->setLexer(m_lexer);
    
    // Initial highlighter setup
    updateHighlighter();
    
    // Set default code
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
    
    layout->addWidget(m_codeEditor, 1);
    
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
    m_codeEditor->setCaretLineBackgroundColor(QColor("#3a3a3a"));
    m_codeEditor->setCaretWidth(2);
    m_codeEditor->setCaretForegroundColor(QColor("#ffffff"));
    
    // Margins
    m_codeEditor->setMarginsBackgroundColor(QColor("#2b2b2b"));
    m_codeEditor->setMarginsForegroundColor(QColor("#888888"));
    
    // Line numbers
    m_codeEditor->setMarginType(0, QsciScintilla::NumberMargin);
    m_codeEditor->setMarginWidth(0, "0000");
    m_codeEditor->setMarginLineNumbers(0, true);
    
    // Code folding
    m_codeEditor->setFolding(QsciScintilla::BoxedTreeFoldStyle);
    m_codeEditor->setFoldMarginColors(QColor("#2b2b2b"), QColor("#2b2b2b"));
    
    // Selection colors
    m_codeEditor->setSelectionBackgroundColor(QColor("#3a5a7a"));
    m_codeEditor->setSelectionForegroundColor(QColor("#ffffff"));
    
    // Enable code completion
    m_codeEditor->setAutoCompletionSource(QsciScintilla::AcsAPIs);
    m_codeEditor->setAutoCompletionThreshold(2);
    m_codeEditor->setAutoCompletionCaseSensitivity(false);
    m_codeEditor->setAutoCompletionShowSingle(true);
    
    // Set font
    QFont codeFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    codeFont.setPointSize(10);
    m_codeEditor->setFont(codeFont);
    
    // Dark theme colors
    m_codeEditor->setPaper(QColor("#1e1e1e"));
    m_codeEditor->setColor(QColor("#d4d4d4"));
    
    // Whitespace
    m_codeEditor->setWhitespaceVisibility(QsciScintilla::WsInvisible);
    m_codeEditor->setIndentationGuides(true);
    m_codeEditor->setIndentationGuidesBackgroundColor(QColor("#404040"));
    m_codeEditor->setIndentationGuidesForegroundColor(QColor("#404040"));
    
    // Edge marker
    m_codeEditor->setEdgeMode(QsciScintilla::EdgeLine);
    m_codeEditor->setEdgeColumn(80);
    m_codeEditor->setEdgeColor(QColor("#404040"));
}

void FloatingCodeEditor::connectSignals()
{
    connect(m_languageCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FloatingCodeEditor::onLanguageChanged);
    
    connect(m_compileButton, &QPushButton::clicked,
            this, &FloatingCodeEditor::onCompileClicked);
    
    connect(m_codeEditor, &QsciScintilla::textChanged,
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
        setupOpenCLHighlighting();
    } else if (language == "CUDA") {
        setupCUDAHighlighting();
    } else if (language == "Metal") {
        setupMetalHighlighting();
    }
}

void FloatingCodeEditor::setupOpenCLHighlighting()
{
    // Set lexer properties
    m_lexer->setDefaultFont(QFont("Consolas", 10));
    m_lexer->setDefaultPaper(QColor("#1e1e1e"));
    m_lexer->setDefaultColor(QColor("#d4d4d4"));
    
    // Keywords (blue)
    m_lexer->setColor(QColor("#569cd6"), QsciLexerCPP::Keyword);
    m_lexer->setColor(QColor("#569cd6"), QsciLexerCPP::KeywordSet2);
    
    // Types (cyan)
    m_lexer->setColor(QColor("#4ec9b0"), QsciLexerCPP::GlobalClass);
    
    // Strings (orange)
    m_lexer->setColor(QColor("#ce9178"), QsciLexerCPP::DoubleQuotedString);
    m_lexer->setColor(QColor("#ce9178"), QsciLexerCPP::SingleQuotedString);
    
    // Comments (green)
    m_lexer->setColor(QColor("#608b4e"), QsciLexerCPP::Comment);
    m_lexer->setColor(QColor("#608b4e"), QsciLexerCPP::CommentLine);
    m_lexer->setColor(QColor("#608b4e"), QsciLexerCPP::CommentDoc);
    m_lexer->setColor(QColor("#608b4e"), QsciLexerCPP::CommentLineDoc);
    m_lexer->setColor(QColor("#608b4e"), QsciLexerCPP::CommentDocKeyword);
    
    // Numbers (light green)
    m_lexer->setColor(QColor("#b5cea8"), QsciLexerCPP::Number);
    
    // Preprocessor (purple)
    m_lexer->setColor(QColor("#c586c0"), QsciLexerCPP::PreProcessor);
    
    // Operators (yellow)
    m_lexer->setColor(QColor("#d7ba7d"), QsciLexerCPP::Operator);
    
    // Identifiers
    m_lexer->setColor(QColor("#9cdcfe"), QsciLexerCPP::Identifier);
    
    // OpenCL specific keywords
    QsciAPIs *api = qobject_cast<QsciAPIs *>(m_lexer->apis());
    if (!api) {
        api = new QsciAPIs(m_lexer);
        m_lexer->setAPIs(api);
    }
    api->clear();
    
    // OpenCL keywords
    QStringList openclKeywords = {
        // Qualifiers
        "__kernel", "kernel", "__global", "global", "__local", "local",
        "__constant", "constant", "__private", "private", "__read_only",
        "read_only", "__write_only", "write_only", "__read_write", "read_write",
        
        // Types
        "bool", "char", "uchar", "short", "ushort", "int", "uint",
        "long", "ulong", "float", "double", "half", "size_t", "ptrdiff_t",
        "intptr_t", "uintptr_t", "void",
        
        // Vector types
        "char2", "char3", "char4", "char8", "char16",
        "uchar2", "uchar3", "uchar4", "uchar8", "uchar16",
        "short2", "short3", "short4", "short8", "short16",
        "ushort2", "ushort3", "ushort4", "ushort8", "ushort16",
        "int2", "int3", "int4", "int8", "int16",
        "uint2", "uint3", "uint4", "uint8", "uint16",
        "long2", "long3", "long4", "long8", "long16",
        "ulong2", "ulong3", "ulong4", "ulong8", "ulong16",
        "float2", "float3", "float4", "float8", "float16",
        "double2", "double3", "double4", "double8", "double16",
        "half2", "half3", "half4", "half8", "half16",
        
        // Image types
        "image2d_t", "image3d_t", "image2d_array_t", "image1d_t",
        "image1d_buffer_t", "image1d_array_t", "sampler_t", "event_t",
        
        // Work-item functions
        "get_global_id", "get_global_size", "get_local_id", "get_local_size",
        "get_group_id", "get_num_groups", "get_work_dim", "get_global_offset",
        
        // Synchronization functions
        "barrier", "mem_fence", "read_mem_fence", "write_mem_fence",
        "CLK_LOCAL_MEM_FENCE", "CLK_GLOBAL_MEM_FENCE",
        
        // Math functions
        "acos", "acosh", "acospi", "asin", "asinh", "asinpi", "atan",
        "atan2", "atanh", "atanpi", "atan2pi", "cbrt", "ceil", "copysign",
        "cos", "cosh", "cospi", "exp", "exp2", "exp10", "expm1", "fabs",
        "fdim", "floor", "fma", "fmax", "fmin", "fmod", "hypot", "log",
        "log2", "log10", "log1p", "pow", "pown", "powr", "rint", "round",
        "rsqrt", "sin", "sinh", "sinpi", "sqrt", "tan", "tanh", "tanpi",
        "trunc", "half_cos", "half_exp", "half_log", "half_rsqrt",
        "half_sin", "half_sqrt", "half_tan", "native_cos", "native_exp",
        "native_log", "native_powr", "native_sin", "native_sqrt", "native_tan",
        
        // Atomic functions
        "atomic_add", "atomic_sub", "atomic_xchg", "atomic_inc", "atomic_dec",
        "atomic_cmpxchg", "atomic_min", "atomic_max", "atomic_and", "atomic_or",
        "atomic_xor",
        
        // Other built-in functions
        "convert_char", "convert_uchar", "convert_short", "convert_ushort",
        "convert_int", "convert_uint", "convert_long", "convert_ulong",
        "convert_float", "convert_double", "as_char", "as_uchar", "as_short",
        "as_ushort", "as_int", "as_uint", "as_long", "as_ulong", "as_float",
        "as_double", "vload", "vstore", "shuffle", "shuffle2", "printf"
    };
    
    for (const QString &keyword : openclKeywords) {
        api->add(keyword);
    }
    
    api->prepare();
}

void FloatingCodeEditor::setupCUDAHighlighting()
{
    // Similar setup to OpenCL but with CUDA-specific keywords
    m_lexer->setDefaultFont(QFont("Consolas", 10));
    m_lexer->setDefaultPaper(QColor("#1e1e1e"));
    m_lexer->setDefaultColor(QColor("#d4d4d4"));
    
    // Keywords (blue)
    m_lexer->setColor(QColor("#569cd6"), QsciLexerCPP::Keyword);
    m_lexer->setColor(QColor("#569cd6"), QsciLexerCPP::KeywordSet2);
    
    // Types (cyan)
    m_lexer->setColor(QColor("#4ec9b0"), QsciLexerCPP::GlobalClass);
    
    // Strings (orange)
    m_lexer->setColor(QColor("#ce9178"), QsciLexerCPP::DoubleQuotedString);
    m_lexer->setColor(QColor("#ce9178"), QsciLexerCPP::SingleQuotedString);
    
    // Comments (green)
    m_lexer->setColor(QColor("#608b4e"), QsciLexerCPP::Comment);
    m_lexer->setColor(QColor("#608b4e"), QsciLexerCPP::CommentLine);
    
    // Numbers (light green)
    m_lexer->setColor(QColor("#b5cea8"), QsciLexerCPP::Number);
    
    // Preprocessor (purple)
    m_lexer->setColor(QColor("#c586c0"), QsciLexerCPP::PreProcessor);
    
    // Operators (yellow)
    m_lexer->setColor(QColor("#d7ba7d"), QsciLexerCPP::Operator);
    
    // CUDA specific keywords
    QsciAPIs *api = qobject_cast<QsciAPIs *>(m_lexer->apis());
    if (!api) {
        api = new QsciAPIs(m_lexer);
        m_lexer->setAPIs(api);
    }
    api->clear();
    
    QStringList cudaKeywords = {
        // CUDA qualifiers
        "__global__", "__device__", "__host__", "__constant__", "__shared__",
        "__restrict__", "__noinline__", "__forceinline__",
        
        // CUDA types
        "dim3", "cudaError_t", "cudaEvent_t", "cudaStream_t",
        "float1", "float2", "float3", "float4",
        "double1", "double2", "double3", "double4",
        "int1", "int2", "int3", "int4",
        "uint1", "uint2", "uint3", "uint4",
        "char1", "char2", "char3", "char4",
        "uchar1", "uchar2", "uchar3", "uchar4",
        "short1", "short2", "short3", "short4",
        "ushort1", "ushort2", "ushort3", "ushort4",
        "long1", "long2", "long3", "long4",
        "ulong1", "ulong2", "ulong3", "ulong4",
        "longlong1", "longlong2", "longlong3", "longlong4",
        "ulonglong1", "ulonglong2", "ulonglong3", "ulonglong4",
        
        // Built-in variables
        "threadIdx", "blockIdx", "blockDim", "gridDim", "warpSize",
        
        // Synchronization
        "__syncthreads", "__syncthreads_count", "__syncthreads_and",
        "__syncthreads_or", "__threadfence", "__threadfence_block",
        "__threadfence_system",
        
        // Warp functions
        "__all_sync", "__any_sync", "__ballot_sync", "__shfl_sync",
        "__shfl_up_sync", "__shfl_down_sync", "__shfl_xor_sync",
        
        // Math functions
        "sinf", "cosf", "tanf", "asinf", "acosf", "atanf", "atan2f",
        "sinhf", "coshf", "tanhf", "expf", "exp2f", "exp10f", "logf",
        "log2f", "log10f", "powf", "sqrtf", "rsqrtf", "fabsf", "fminf",
        "fmaxf", "fmaf", "truncf", "roundf", "floorf", "ceilf",
        "__fadd_rn", "__fsub_rn", "__fmul_rn", "__fdiv_rn", "__fsqrt_rn",
        
        // Atomic functions
        "atomicAdd", "atomicSub", "atomicExch", "atomicMin", "atomicMax",
        "atomicInc", "atomicDec", "atomicCAS", "atomicAnd", "atomicOr",
        "atomicXor",
        
        // Texture functions
        "tex1D", "tex2D", "tex3D", "tex1Dfetch", "texCubemap",
        
        // Memory functions
        "cudaMalloc", "cudaFree", "cudaMemcpy", "cudaMemset",
        "cudaMallocManaged", "cudaDeviceSynchronize",
        
        // Launch bounds
        "__launch_bounds__",
        
        // Common CUDA runtime functions
        "cudaGetLastError", "cudaGetErrorString", "cudaSetDevice",
        "cudaGetDevice", "cudaGetDeviceProperties"
    };
    
    for (const QString &keyword : cudaKeywords) {
        api->add(keyword);
    }
    
    api->prepare();
}

void FloatingCodeEditor::setupMetalHighlighting()
{
    m_lexer->setDefaultFont(QFont("Consolas", 10));
    m_lexer->setDefaultPaper(QColor("#1e1e1e"));
    m_lexer->setDefaultColor(QColor("#d4d4d4"));
    
    // Keywords (blue)
    m_lexer->setColor(QColor("#569cd6"), QsciLexerCPP::Keyword);
    m_lexer->setColor(QColor("#569cd6"), QsciLexerCPP::KeywordSet2);
    
    // Types (cyan)
    m_lexer->setColor(QColor("#4ec9b0"), QsciLexerCPP::GlobalClass);
    
    // Strings (orange)
    m_lexer->setColor(QColor("#ce9178"), QsciLexerCPP::DoubleQuotedString);
    m_lexer->setColor(QColor("#ce9178"), QsciLexerCPP::SingleQuotedString);
    
    // Comments (green)
    m_lexer->setColor(QColor("#608b4e"), QsciLexerCPP::Comment);
    m_lexer->setColor(QColor("#608b4e"), QsciLexerCPP::CommentLine);
    
    // Numbers (light green)
    m_lexer->setColor(QColor("#b5cea8"), QsciLexerCPP::Number);
    
    // Preprocessor (purple)
    m_lexer->setColor(QColor("#c586c0"), QsciLexerCPP::PreProcessor);
    
    // Operators (yellow)
    m_lexer->setColor(QColor("#d7ba7d"), QsciLexerCPP::Operator);
    
    // Metal specific keywords
    QsciAPIs *api = qobject_cast<QsciAPIs *>(m_lexer->apis());
    if (!api) {
        api = new QsciAPIs(m_lexer);
        m_lexer->setAPIs(api);
    }
    api->clear();
    
    QStringList metalKeywords = {
        // Metal qualifiers
        "kernel", "vertex", "fragment", "device", "constant", "threadgroup",
        "thread", "threadgroup_imageblock", "ray_data",
        
        // Metal types
        "bool", "char", "uchar", "short", "ushort", "int", "uint",
        "long", "ulong", "half", "float", "double", "size_t", "ptrdiff_t",
        
        // Vector types
        "bool2", "bool3", "bool4",
        "char2", "char3", "char4", "uchar2", "uchar3", "uchar4",
        "short2", "short3", "short4", "ushort2", "ushort3", "ushort4",
        "int2", "int3", "int4", "uint2", "uint3", "uint4",
        "long2", "long3", "long4", "ulong2", "ulong3", "ulong4",
        "half2", "half3", "half4", "float2", "float3", "float4",
        "double2", "double3", "double4",
        
        // Packed types
        "packed_bool2", "packed_bool3", "packed_bool4",
        "packed_char2", "packed_char3", "packed_char4",
        "packed_uchar2", "packed_uchar3", "packed_uchar4",
        "packed_short2", "packed_short3", "packed_short4",
        "packed_ushort2", "packed_ushort3", "packed_ushort4",
        "packed_int2", "packed_int3", "packed_int4",
        "packed_uint2", "packed_uint3", "packed_uint4",
        "packed_half2", "packed_half3", "packed_half4",
        "packed_float2", "packed_float3", "packed_float4",
        
        // Matrix types
        "half2x2", "half3x3", "half4x4", "float2x2", "float3x3", "float4x4",
        "double2x2", "double3x3", "double4x4",
        
        // Texture types
        "texture1d", "texture1d_array", "texture2d", "texture2d_array",
        "texture3d", "texturecube", "texturecube_array", "texture2d_ms",
        "texture2d_ms_array", "depth2d", "depth2d_array", "depthcube",
        "depth2d_ms", "depth2d_ms_array",
        
        // Sampler
        "sampler", "sampler_compare",
        
        // Attributes
        "[[buffer]]", "[[texture]]", "[[sampler]]", "[[thread_position_in_grid]]",
        "[[thread_position_in_threadgroup]]", "[[threadgroup_position_in_grid]]",
        "[[threads_per_threadgroup]]", "[[vertex_id]]", "[[instance_id]]",
        "[[position]]", "[[point_size]]", "[[color]]", "[[stage_in]]",
        
        // Built-in functions
        "abs", "acos", "acosh", "asin", "asinh", "atan", "atan2", "atanh",
        "ceil", "clamp", "copysign", "cos", "cosh", "cross", "degrees",
        "distance", "dot", "exp", "exp2", "exp10", "fabs", "faceforward",
        "fdim", "floor", "fma", "fmax", "fmin", "fmod", "fract", "frexp",
        "isinf", "isnan", "isnormal", "length", "log", "log2", "log10",
        "max", "min", "mix", "modf", "normalize", "pow", "powr", "radians",
        "reflect", "refract", "rint", "round", "rsqrt", "saturate", "sign",
        "sin", "sincos", "sinh", "smoothstep", "sqrt", "step", "tan", "tanh",
        "trunc",
        
        // Synchronization
        "threadgroup_barrier", "simdgroup_barrier", "mem_flags",
        "mem_none", "mem_device", "mem_threadgroup", "mem_texture",
        
        // SIMD-group functions
        "simd_active_threads_mask", "simd_ballot", "simd_broadcast",
        "simd_broadcast_first", "simd_is_first", "simd_max", "simd_min",
        "simd_or", "simd_and", "simd_xor", "simd_prefix_exclusive_product",
        "simd_prefix_exclusive_sum", "simd_prefix_inclusive_product",
        "simd_prefix_inclusive_sum", "simd_product", "simd_sum",
        
        // Atomic functions
        "atomic_exchange_explicit", "atomic_compare_exchange_weak_explicit",
        "atomic_fetch_add_explicit", "atomic_fetch_sub_explicit",
        "atomic_fetch_or_explicit", "atomic_fetch_xor_explicit",
        "atomic_fetch_and_explicit", "atomic_fetch_min_explicit",
        "atomic_fetch_max_explicit", "atomic_load_explicit",
        "atomic_store_explicit",
        
        // Memory order
        "memory_order_relaxed", "memory_order_acquire", "memory_order_release",
        "memory_order_acq_rel", "memory_order_seq_cst",
        
        // Metal standard library
        "metal", "using namespace metal"
    };
    
    for (const QString &keyword : metalKeywords) {
        api->add(keyword);
    }
    
    api->prepare();
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