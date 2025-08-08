#include "compilers/CUDACompiler.hpp"
#include <QDebug>
#include <vector>

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