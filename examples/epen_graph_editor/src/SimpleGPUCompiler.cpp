#include "SimpleGPUCompiler.hpp"
#include <QRegularExpression>
#include <QDebug>
#include <QProcess>
#include <QTemporaryFile>
#include <QDir>
#include <QLibrary>
#include <vector>

SimpleGPUCompiler::SimpleGPUCompiler() {}

SimpleGPUCompiler::~SimpleGPUCompiler() {}

QVector<SimpleGPUCompiler::CompileMessage> SimpleGPUCompiler::parseCompilerOutput(const QString &output)
{
    QVector<CompileMessage> messages;
    
    // Parse common error formats
    QStringList lines = output.split('\n');
    
    for (const QString &line : lines) {
        if (line.isEmpty()) continue;
        
        CompileMessage msg;
        
        // Try to parse line:column: error format
        QRegularExpression lineColRegex(R"((?:^|:)(\d+):(\d+):\s*(error|warning|note):\s*(.+))");
        QRegularExpressionMatch match = lineColRegex.match(line);
        
        if (match.hasMatch()) {
            msg.line = match.captured(1).toInt();
            msg.column = match.captured(2).toInt();
            QString type = match.captured(3).toLower();
            msg.type = (type == "error") ? CompileMessage::Error :
                      (type == "warning") ? CompileMessage::Warning :
                      CompileMessage::Info;
            msg.message = match.captured(4);
            messages.append(msg);
            continue;
        }
        
        // Try simpler error format
        QRegularExpression simpleRegex(R"((error|warning):\s*(.+?)(?:\s*at line\s*(\d+))?)");
        match = simpleRegex.match(line);
        
        if (match.hasMatch()) {
            QString type = match.captured(1).toLower();
            msg.type = (type == "error") ? CompileMessage::Error :
                      CompileMessage::Warning;
            msg.message = match.captured(2);
            msg.line = match.captured(3).isEmpty() ? 0 : match.captured(3).toInt();
            msg.column = 0;
            messages.append(msg);
            continue;
        }
        
        // Generic error detection
        if (line.contains("error", Qt::CaseInsensitive)) {
            msg.type = CompileMessage::Error;
            msg.message = line.trimmed();
            msg.line = 0;
            msg.column = 0;
            messages.append(msg);
        }
    }
    
    return messages;
}

// OpenCL Implementation with runtime loading
#ifdef HAS_OPENCL_HEADERS
// If we have headers, we can use the actual types
typedef cl_int (*clGetPlatformIDs_fn)(cl_uint, cl_platform_id*, cl_uint*);
typedef cl_int (*clGetDeviceIDs_fn)(cl_platform_id, cl_device_type, cl_uint, cl_device_id*, cl_uint*);
typedef cl_context (*clCreateContext_fn)(const cl_context_properties*, cl_uint, const cl_device_id*, 
                                         void (*)(const char*, const void*, size_t, void*), void*, cl_int*);
typedef cl_int (*clReleaseContext_fn)(cl_context);
typedef cl_program (*clCreateProgramWithSource_fn)(cl_context, cl_uint, const char**, const size_t*, cl_int*);
typedef cl_int (*clBuildProgram_fn)(cl_program, cl_uint, const cl_device_id*, const char*, 
                                   void (*)(cl_program, void*), void*);
typedef cl_int (*clGetProgramBuildInfo_fn)(cl_program, cl_device_id, cl_program_build_info, size_t, void*, size_t*);
typedef cl_int (*clGetProgramInfo_fn)(cl_program, cl_program_info, size_t, void*, size_t*);
typedef cl_int (*clReleaseProgram_fn)(cl_program);
typedef cl_int (*clGetDeviceInfo_fn)(cl_device_id, cl_device_info, size_t, void*, size_t*);
#else
// Define minimal types if headers not available
typedef int cl_int;
typedef unsigned int cl_uint;
typedef void* cl_platform_id;
typedef void* cl_device_id;
typedef void* cl_context;
typedef void* cl_program;
typedef unsigned long cl_device_type;
typedef unsigned int cl_program_build_info;
typedef unsigned int cl_program_info;
typedef unsigned int cl_device_info;
typedef intptr_t cl_context_properties;
typedef size_t size_t;

#define CL_SUCCESS 0
#define CL_DEVICE_TYPE_GPU 0x04
#define CL_DEVICE_TYPE_CPU 0x02
#define CL_DEVICE_TYPE_ALL 0xFFFFFFFF
#define CL_PROGRAM_BUILD_LOG 0x1183
#define CL_PROGRAM_BINARY_SIZES 0x1165
#define CL_PROGRAM_BINARIES 0x1166
#define CL_DEVICE_NAME 0x102B

typedef cl_int (*clGetPlatformIDs_fn)(cl_uint, cl_platform_id*, cl_uint*);
typedef cl_int (*clGetDeviceIDs_fn)(cl_platform_id, cl_device_type, cl_uint, cl_device_id*, cl_uint*);
typedef cl_context (*clCreateContext_fn)(const cl_context_properties*, cl_uint, const cl_device_id*, 
                                         void*, void*, cl_int*);
typedef cl_int (*clReleaseContext_fn)(cl_context);
typedef cl_program (*clCreateProgramWithSource_fn)(cl_context, cl_uint, const char**, const size_t*, cl_int*);
typedef cl_int (*clBuildProgram_fn)(cl_program, cl_uint, const cl_device_id*, const char*, void*, void*);
typedef cl_int (*clGetProgramBuildInfo_fn)(cl_program, cl_device_id, cl_program_build_info, size_t, void*, size_t*);
typedef cl_int (*clGetProgramInfo_fn)(cl_program, cl_program_info, size_t, void*, size_t*);
typedef cl_int (*clReleaseProgram_fn)(cl_program);
typedef cl_int (*clGetDeviceInfo_fn)(cl_device_id, cl_device_info, size_t, void*, size_t*);
#endif

struct OpenCLCompiler::OpenCLFunctions {
    clGetPlatformIDs_fn clGetPlatformIDs;
    clGetDeviceIDs_fn clGetDeviceIDs;
    clCreateContext_fn clCreateContext;
    clReleaseContext_fn clReleaseContext;
    clCreateProgramWithSource_fn clCreateProgramWithSource;
    clBuildProgram_fn clBuildProgram;
    clGetProgramBuildInfo_fn clGetProgramBuildInfo;
    clGetProgramInfo_fn clGetProgramInfo;
    clReleaseProgram_fn clReleaseProgram;
    clGetDeviceInfo_fn clGetDeviceInfo;
};

OpenCLCompiler::OpenCLCompiler()
    : m_openclLib(nullptr)
    , m_platform(nullptr)
    , m_device(nullptr)
    , m_context(nullptr)
    , m_initialized(false)
    , m_functions(std::make_unique<OpenCLFunctions>())
{
    m_initialized = initializeOpenCL();
}

OpenCLCompiler::~OpenCLCompiler()
{
    cleanup();
}

bool OpenCLCompiler::loadOpenCLLibrary()
{
    // Try to load OpenCL library
    QStringList libraryNames;
    
#ifdef Q_OS_WIN
    libraryNames << "OpenCL.dll" << "OpenCL64.dll";
#elif defined(Q_OS_MAC)
    libraryNames << "/System/Library/Frameworks/OpenCL.framework/OpenCL";
#else
    libraryNames << "libOpenCL.so" << "libOpenCL.so.1";
#endif

    m_openclLib = std::make_unique<QLibrary>();
    for (const QString &libName : libraryNames) {
        m_openclLib->setFileName(libName);
        if (m_openclLib->load()) {
            break;
        }
    }
    
    if (!m_openclLib->isLoaded()) {
        m_availabilityError = "OpenCL library not found. Please install OpenCL drivers for your GPU.";
        return false;
    }
    
    // Load function pointers
    m_functions->clGetPlatformIDs = (clGetPlatformIDs_fn)m_openclLib->resolve("clGetPlatformIDs");
    m_functions->clGetDeviceIDs = (clGetDeviceIDs_fn)m_openclLib->resolve("clGetDeviceIDs");
    m_functions->clCreateContext = (clCreateContext_fn)m_openclLib->resolve("clCreateContext");
    m_functions->clReleaseContext = (clReleaseContext_fn)m_openclLib->resolve("clReleaseContext");
    m_functions->clCreateProgramWithSource = (clCreateProgramWithSource_fn)m_openclLib->resolve("clCreateProgramWithSource");
    m_functions->clBuildProgram = (clBuildProgram_fn)m_openclLib->resolve("clBuildProgram");
    m_functions->clGetProgramBuildInfo = (clGetProgramBuildInfo_fn)m_openclLib->resolve("clGetProgramBuildInfo");
    m_functions->clGetProgramInfo = (clGetProgramInfo_fn)m_openclLib->resolve("clGetProgramInfo");
    m_functions->clReleaseProgram = (clReleaseProgram_fn)m_openclLib->resolve("clReleaseProgram");
    m_functions->clGetDeviceInfo = (clGetDeviceInfo_fn)m_openclLib->resolve("clGetDeviceInfo");
    
    // Check if all functions were loaded
    if (!m_functions->clGetPlatformIDs || !m_functions->clGetDeviceIDs || 
        !m_functions->clCreateContext || !m_functions->clReleaseContext ||
        !m_functions->clCreateProgramWithSource || !m_functions->clBuildProgram ||
        !m_functions->clGetProgramBuildInfo || !m_functions->clGetProgramInfo ||
        !m_functions->clReleaseProgram || !m_functions->clGetDeviceInfo) {
        m_availabilityError = "Failed to load OpenCL functions. OpenCL library may be corrupted.";
        return false;
    }
    
    return true;
}

bool OpenCLCompiler::initializeOpenCL()
{
    if (!loadOpenCLLibrary()) {
        return false;
    }
    
    cl_int err;
    
    // Get platform
    cl_uint numPlatforms;
    err = m_functions->clGetPlatformIDs(0, nullptr, &numPlatforms);
    if (err != CL_SUCCESS || numPlatforms == 0) {
        m_availabilityError = "No OpenCL platforms found. Please install OpenCL drivers.";
        return false;
    }
    
    std::vector<cl_platform_id> platforms(numPlatforms);
    err = m_functions->clGetPlatformIDs(numPlatforms, platforms.data(), nullptr);
    if (err != CL_SUCCESS) {
        m_availabilityError = "Failed to get OpenCL platforms.";
        return false;
    }
    
    m_platform = platforms[0]; // Use first platform
    
    // Get GPU device
    cl_uint numDevices;
    err = m_functions->clGetDeviceIDs((cl_platform_id)m_platform, CL_DEVICE_TYPE_GPU, 0, nullptr, &numDevices);
    if (err != CL_SUCCESS || numDevices == 0) {
        // Try CPU if no GPU found
        err = m_functions->clGetDeviceIDs((cl_platform_id)m_platform, CL_DEVICE_TYPE_CPU, 0, nullptr, &numDevices);
        if (err != CL_SUCCESS || numDevices == 0) {
            m_availabilityError = "No OpenCL devices found. Please check your GPU drivers.";
            return false;
        }
    }
    
    std::vector<cl_device_id> devices(numDevices);
    err = m_functions->clGetDeviceIDs((cl_platform_id)m_platform, CL_DEVICE_TYPE_ALL, numDevices, devices.data(), nullptr);
    if (err != CL_SUCCESS) {
        m_availabilityError = "Failed to get OpenCL devices.";
        return false;
    }
    
    m_device = devices[0]; // Use first device
    
    // Create context
    m_context = m_functions->clCreateContext(nullptr, 1, (cl_device_id*)&m_device, nullptr, nullptr, &err);
    if (err != CL_SUCCESS) {
        m_availabilityError = "Failed to create OpenCL context.";
        return false;
    }
    
    return true;
}

void OpenCLCompiler::cleanup()
{
    if (m_context && m_functions && m_functions->clReleaseContext) {
        m_functions->clReleaseContext((cl_context)m_context);
        m_context = nullptr;
    }
    m_initialized = false;
}

bool OpenCLCompiler::isAvailable() const
{
    return m_initialized;
}

SimpleGPUCompiler::CompileResult OpenCLCompiler::compile(const QString &source, const QString &kernelName)
{
    CompileResult result;
    result.success = false;
    
    if (!m_initialized) {
        result.buildLog = "OpenCL not initialized: " + m_availabilityError;
        return result;
    }
    
    cl_int err;
    const char *sourceStr = source.toUtf8().constData();
    size_t sourceSize = source.toUtf8().size();
    
    // Create program
    cl_program program = m_functions->clCreateProgramWithSource((cl_context)m_context, 1, &sourceStr, &sourceSize, &err);
    if (err != CL_SUCCESS) {
        result.buildLog = QString("Failed to create program: %1").arg(err);
        return result;
    }
    
    // Build program
    err = m_functions->clBuildProgram(program, 1, (cl_device_id*)&m_device, "-cl-std=CL1.2", nullptr, nullptr);
    
    // Get build log
    size_t logSize;
    m_functions->clGetProgramBuildInfo(program, (cl_device_id)m_device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);
    
    if (logSize > 0) {
        std::vector<char> log(logSize);
        m_functions->clGetProgramBuildInfo(program, (cl_device_id)m_device, CL_PROGRAM_BUILD_LOG, logSize, log.data(), nullptr);
        result.buildLog = QString::fromUtf8(log.data());
        
        // Parse messages from build log
        result.messages = parseCompilerOutput(result.buildLog);
    }
    
    if (err == CL_SUCCESS) {
        result.success = true;
        
        // Get binary size
        size_t binarySize;
        m_functions->clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, sizeof(size_t), &binarySize, nullptr);
        
        if (binarySize > 0) {
            // Get binary
            result.compiledBinary.resize(binarySize);
            unsigned char *binary = reinterpret_cast<unsigned char*>(result.compiledBinary.data());
            m_functions->clGetProgramInfo(program, CL_PROGRAM_BINARIES, sizeof(unsigned char*), &binary, nullptr);
        }
    }
    
    m_functions->clReleaseProgram(program);
    
    return result;
}

QStringList OpenCLCompiler::getAvailableDevices() const
{
    QStringList devices;
    
    if (!m_initialized) {
        return devices;
    }
    
    cl_uint numDevices;
    cl_int err = m_functions->clGetDeviceIDs((cl_platform_id)m_platform, CL_DEVICE_TYPE_ALL, 0, nullptr, &numDevices);
    if (err != CL_SUCCESS) {
        return devices;
    }
    
    std::vector<cl_device_id> deviceIds(numDevices);
    err = m_functions->clGetDeviceIDs((cl_platform_id)m_platform, CL_DEVICE_TYPE_ALL, numDevices, deviceIds.data(), nullptr);
    if (err != CL_SUCCESS) {
        return devices;
    }
    
    for (cl_device_id device : deviceIds) {
        char name[256];
        m_functions->clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(name), name, nullptr);
        devices.append(QString::fromUtf8(name));
    }
    
    return devices;
}

// CUDA Implementation with runtime loading
#ifdef HAS_CUDA_HEADERS
typedef CUresult (*cuInit_fn)(unsigned int);
typedef CUresult (*cuDeviceGetCount_fn)(int*);
typedef CUresult (*cuDeviceGet_fn)(CUdevice*, int);
typedef CUresult (*cuDeviceGetName_fn)(char*, int, CUdevice);
typedef nvrtcResult (*nvrtcCreateProgram_fn)(nvrtcProgram*, const char*, const char*, int, const char* const*, const char* const*);
typedef nvrtcResult (*nvrtcDestroyProgram_fn)(nvrtcProgram*);
typedef nvrtcResult (*nvrtcCompileProgram_fn)(nvrtcProgram, int, const char* const*);
typedef nvrtcResult (*nvrtcGetProgramLogSize_fn)(nvrtcProgram, size_t*);
typedef nvrtcResult (*nvrtcGetProgramLog_fn)(nvrtcProgram, char*);
typedef nvrtcResult (*nvrtcGetPTXSize_fn)(nvrtcProgram, size_t*);
typedef nvrtcResult (*nvrtcGetPTX_fn)(nvrtcProgram, char*);
typedef const char* (*nvrtcGetErrorString_fn)(nvrtcResult);
#else
// Define minimal types if headers not available
typedef int CUresult;
typedef int CUdevice;
typedef int nvrtcResult;
typedef void* nvrtcProgram;

#define CUDA_SUCCESS 0
#define NVRTC_SUCCESS 0

typedef CUresult (*cuInit_fn)(unsigned int);
typedef CUresult (*cuDeviceGetCount_fn)(int*);
typedef CUresult (*cuDeviceGet_fn)(CUdevice*, int);
typedef CUresult (*cuDeviceGetName_fn)(char*, int, CUdevice);
typedef nvrtcResult (*nvrtcCreateProgram_fn)(nvrtcProgram*, const char*, const char*, int, const char* const*, const char* const*);
typedef nvrtcResult (*nvrtcDestroyProgram_fn)(nvrtcProgram*);
typedef nvrtcResult (*nvrtcCompileProgram_fn)(nvrtcProgram, int, const char* const*);
typedef nvrtcResult (*nvrtcGetProgramLogSize_fn)(nvrtcProgram, size_t*);
typedef nvrtcResult (*nvrtcGetProgramLog_fn)(nvrtcProgram, char*);
typedef nvrtcResult (*nvrtcGetPTXSize_fn)(nvrtcProgram, size_t*);
typedef nvrtcResult (*nvrtcGetPTX_fn)(nvrtcProgram, char*);
typedef const char* (*nvrtcGetErrorString_fn)(nvrtcResult);
#endif

struct CUDACompiler::CUDAFunctions {
    cuInit_fn cuInit;
    cuDeviceGetCount_fn cuDeviceGetCount;
    cuDeviceGet_fn cuDeviceGet;
    cuDeviceGetName_fn cuDeviceGetName;
    nvrtcCreateProgram_fn nvrtcCreateProgram;
    nvrtcDestroyProgram_fn nvrtcDestroyProgram;
    nvrtcCompileProgram_fn nvrtcCompileProgram;
    nvrtcGetProgramLogSize_fn nvrtcGetProgramLogSize;
    nvrtcGetProgramLog_fn nvrtcGetProgramLog;
    nvrtcGetPTXSize_fn nvrtcGetPTXSize;
    nvrtcGetPTX_fn nvrtcGetPTX;
    nvrtcGetErrorString_fn nvrtcGetErrorString;
};

CUDACompiler::CUDACompiler()
    : m_cudaLib(nullptr)
    , m_nvrtcLib(nullptr)
    , m_initialized(false)
    , m_deviceCount(0)
    , m_functions(std::make_unique<CUDAFunctions>())
{
    m_initialized = initializeCUDA();
}

CUDACompiler::~CUDACompiler()
{
    cleanup();
}

bool CUDACompiler::loadCUDALibraries()
{
    // Try to load CUDA driver library
    QStringList cudaLibNames;
    QStringList nvrtcLibNames;
    
#ifdef Q_OS_WIN
    cudaLibNames << "nvcuda.dll" << "cuda.dll";
    nvrtcLibNames << "nvrtc64_120_0.dll" << "nvrtc64_110_0.dll" << "nvrtc64_101_0.dll" << "nvrtc.dll";
#elif defined(Q_OS_MAC)
    cudaLibNames << "libcuda.dylib";
    nvrtcLibNames << "libnvrtc.dylib";
#else
    cudaLibNames << "libcuda.so" << "libcuda.so.1";
    nvrtcLibNames << "libnvrtc.so" << "libnvrtc.so.12" << "libnvrtc.so.11.0" << "libnvrtc.so.10.1";
#endif

    m_cudaLib = std::make_unique<QLibrary>();
    for (const QString &libName : cudaLibNames) {
        m_cudaLib->setFileName(libName);
        if (m_cudaLib->load()) {
            break;
        }
    }
    
    if (!m_cudaLib->isLoaded()) {
        m_availabilityError = "CUDA driver library not found. Please install NVIDIA GPU drivers.";
        return false;
    }
    
    m_nvrtcLib = std::make_unique<QLibrary>();
    for (const QString &libName : nvrtcLibNames) {
        m_nvrtcLib->setFileName(libName);
        if (m_nvrtcLib->load()) {
            break;
        }
    }
    
    if (!m_nvrtcLib->isLoaded()) {
        m_availabilityError = "NVRTC library not found. Please install CUDA Toolkit.";
        return false;
    }
    
    // Load function pointers
    m_functions->cuInit = (cuInit_fn)m_cudaLib->resolve("cuInit");
    m_functions->cuDeviceGetCount = (cuDeviceGetCount_fn)m_cudaLib->resolve("cuDeviceGetCount");
    m_functions->cuDeviceGet = (cuDeviceGet_fn)m_cudaLib->resolve("cuDeviceGet");
    m_functions->cuDeviceGetName = (cuDeviceGetName_fn)m_cudaLib->resolve("cuDeviceGetName");
    
    m_functions->nvrtcCreateProgram = (nvrtcCreateProgram_fn)m_nvrtcLib->resolve("nvrtcCreateProgram");
    m_functions->nvrtcDestroyProgram = (nvrtcDestroyProgram_fn)m_nvrtcLib->resolve("nvrtcDestroyProgram");
    m_functions->nvrtcCompileProgram = (nvrtcCompileProgram_fn)m_nvrtcLib->resolve("nvrtcCompileProgram");
    m_functions->nvrtcGetProgramLogSize = (nvrtcGetProgramLogSize_fn)m_nvrtcLib->resolve("nvrtcGetProgramLogSize");
    m_functions->nvrtcGetProgramLog = (nvrtcGetProgramLog_fn)m_nvrtcLib->resolve("nvrtcGetProgramLog");
    m_functions->nvrtcGetPTXSize = (nvrtcGetPTXSize_fn)m_nvrtcLib->resolve("nvrtcGetPTXSize");
    m_functions->nvrtcGetPTX = (nvrtcGetPTX_fn)m_nvrtcLib->resolve("nvrtcGetPTX");
    m_functions->nvrtcGetErrorString = (nvrtcGetErrorString_fn)m_nvrtcLib->resolve("nvrtcGetErrorString");
    
    // Check if all functions were loaded
    if (!m_functions->cuInit || !m_functions->cuDeviceGetCount || 
        !m_functions->cuDeviceGet || !m_functions->cuDeviceGetName ||
        !m_functions->nvrtcCreateProgram || !m_functions->nvrtcDestroyProgram ||
        !m_functions->nvrtcCompileProgram || !m_functions->nvrtcGetProgramLogSize ||
        !m_functions->nvrtcGetProgramLog || !m_functions->nvrtcGetPTXSize ||
        !m_functions->nvrtcGetPTX) {
        m_availabilityError = "Failed to load CUDA functions. CUDA installation may be incomplete.";
        return false;
    }
    
    return true;
}

bool CUDACompiler::initializeCUDA()
{
    if (!loadCUDALibraries()) {
        return false;
    }
    
    CUresult err = m_functions->cuInit(0);
    if (err != CUDA_SUCCESS) {
        m_availabilityError = "Failed to initialize CUDA. No NVIDIA GPU detected.";
        return false;
    }
    
    err = m_functions->cuDeviceGetCount(&m_deviceCount);
    if (err != CUDA_SUCCESS || m_deviceCount == 0) {
        m_availabilityError = "No CUDA devices found. Please check NVIDIA GPU drivers.";
        return false;
    }
    
    return true;
}

void CUDACompiler::cleanup()
{
    m_initialized = false;
}

bool CUDACompiler::isAvailable() const
{
    return m_initialized;
}

SimpleGPUCompiler::CompileResult CUDACompiler::compile(const QString &source, const QString &kernelName)
{
    CompileResult result;
    result.success = false;
    
    if (!m_initialized) {
        result.buildLog = "CUDA not initialized: " + m_availabilityError;
        return result;
    }
    
    nvrtcProgram prog;
    nvrtcResult res = m_functions->nvrtcCreateProgram(&prog,
                                        source.toUtf8().constData(),
                                        "kernel.cu",
                                        0,
                                        nullptr,
                                        nullptr);
    
    if (res != NVRTC_SUCCESS) {
        result.buildLog = QString("Failed to create program: %1").arg(
            m_functions->nvrtcGetErrorString ? m_functions->nvrtcGetErrorString(res) : "Unknown error");
        return result;
    }
    
    // Compile with common options
    const char *opts[] = {
        "--gpu-architecture=compute_50",
        "--fmad=true"
    };
    
    res = m_functions->nvrtcCompileProgram(prog, 2, opts);
    
    // Get compilation log
    size_t logSize;
    m_functions->nvrtcGetProgramLogSize(prog, &logSize);
    
    if (logSize > 0) {
        std::vector<char> log(logSize);
        m_functions->nvrtcGetProgramLog(prog, log.data());
        result.buildLog = QString::fromUtf8(log.data());
        
        // Parse messages
        result.messages = parseCompilerOutput(result.buildLog);
    }
    
    if (res == NVRTC_SUCCESS) {
        result.success = true;
        
        // Get PTX
        size_t ptxSize;
        m_functions->nvrtcGetPTXSize(prog, &ptxSize);
        
        if (ptxSize > 0) {
            result.compiledBinary.resize(ptxSize);
            m_functions->nvrtcGetPTX(prog, result.compiledBinary.data());
        }
    }
    
    m_functions->nvrtcDestroyProgram(&prog);
    
    return result;
}

QStringList CUDACompiler::getAvailableDevices() const
{
    QStringList devices;
    
    if (!m_initialized) {
        return devices;
    }
    
    for (int i = 0; i < m_deviceCount; ++i) {
        CUdevice device;
        if (m_functions->cuDeviceGet(&device, i) == CUDA_SUCCESS) {
            char name[256];
            m_functions->cuDeviceGetName(name, sizeof(name), device);
            devices.append(QString::fromUtf8(name));
        }
    }
    
    return devices;
}

// Metal Implementation
MetalCompiler::MetalCompiler()
    : m_initialized(false)
{
#ifdef Q_OS_MAC
    // Check if xcrun is available
    QProcess checkProcess;
    checkProcess.start("which", QStringList() << "xcrun");
    checkProcess.waitForFinished();
    
    if (checkProcess.exitCode() == 0) {
        // Check if metal compiler is available
        QProcess metalCheck;
        metalCheck.start("xcrun", QStringList() << "-find" << "metal");
        metalCheck.waitForFinished();
        
        if (metalCheck.exitCode() == 0) {
            m_initialized = true;
        } else {
            m_availabilityError = "Metal compiler not found. Please install Xcode Command Line Tools.";
        }
    } else {
        m_availabilityError = "xcrun not found. Please install Xcode.";
    }
#else
    m_availabilityError = "Metal is only available on macOS.";
#endif
}

MetalCompiler::~MetalCompiler()
{
}

bool MetalCompiler::isAvailable() const
{
    return m_initialized;
}

SimpleGPUCompiler::CompileResult MetalCompiler::compile(const QString &source, const QString &kernelName)
{
    CompileResult result;
    result.success = false;
    
    if (!m_initialized) {
        result.buildLog = "Metal not available: " + m_availabilityError;
        return result;
    }
    
#ifdef Q_OS_MAC
    // Write source to temporary file
    QTemporaryFile sourceFile(QDir::tempPath() + "/metal_XXXXXX.metal");
    if (!sourceFile.open()) {
        result.buildLog = "Failed to create temporary file";
        return result;
    }
    
    sourceFile.write(source.toUtf8());
    sourceFile.flush();
    
    // Use xcrun to compile Metal
    QProcess process;
    QStringList args;
    args << "metal" << "-std=macos-metal2.0" << sourceFile.fileName() << "-o" << "-";
    
    process.start("xcrun", args);
    if (!process.waitForFinished(30000)) { // 30 second timeout
        result.buildLog = "Metal compilation timed out";
        return result;
    }
    
    if (process.exitCode() != 0) {
        result.buildLog = QString::fromUtf8(process.readAllStandardError());
        result.messages = parseCompilerOutput(result.buildLog);
    } else {
        result.success = true;
        result.compiledBinary = process.readAllStandardOutput();
        result.buildLog = "Compilation successful";
    }
#else
    result.buildLog = "Metal is only available on macOS";
#endif
    
    return result;
}

QStringList MetalCompiler::getAvailableDevices() const
{
#ifdef Q_OS_MAC
    return QStringList() << "Default Metal Device";
#else
    return QStringList();
#endif
}