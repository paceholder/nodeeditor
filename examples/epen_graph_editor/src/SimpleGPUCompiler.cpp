#include "SimpleGPUCompiler.hpp"
#include <QRegularExpression>
#include <QDebug>
#include <QProcess>
#include <QTemporaryFile>
#include <QDir>
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

#ifdef HAS_OPENCL
OpenCLCompiler::OpenCLCompiler()
    : m_platform(nullptr)
    , m_device(nullptr)
    , m_context(nullptr)
    , m_initialized(false)
{
    m_initialized = initializeOpenCL();
}

OpenCLCompiler::~OpenCLCompiler()
{
    cleanup();
}

bool OpenCLCompiler::initializeOpenCL()
{
    cl_int err;
    
    // Get platform
    cl_uint numPlatforms;
    err = clGetPlatformIDs(0, nullptr, &numPlatforms);
    if (err != CL_SUCCESS || numPlatforms == 0) {
        qDebug() << "No OpenCL platforms found";
        return false;
    }
    
    std::vector<cl_platform_id> platforms(numPlatforms);
    err = clGetPlatformIDs(numPlatforms, platforms.data(), nullptr);
    if (err != CL_SUCCESS) {
        return false;
    }
    
    m_platform = platforms[0]; // Use first platform
    
    // Get GPU device
    cl_uint numDevices;
    err = clGetDeviceIDs(m_platform, CL_DEVICE_TYPE_GPU, 0, nullptr, &numDevices);
    if (err != CL_SUCCESS || numDevices == 0) {
        // Try CPU if no GPU found
        err = clGetDeviceIDs(m_platform, CL_DEVICE_TYPE_CPU, 0, nullptr, &numDevices);
        if (err != CL_SUCCESS || numDevices == 0) {
            qDebug() << "No OpenCL devices found";
            return false;
        }
    }
    
    std::vector<cl_device_id> devices(numDevices);
    err = clGetDeviceIDs(m_platform, CL_DEVICE_TYPE_ALL, numDevices, devices.data(), nullptr);
    if (err != CL_SUCCESS) {
        return false;
    }
    
    m_device = devices[0]; // Use first device
    
    // Create context
    m_context = clCreateContext(nullptr, 1, &m_device, nullptr, nullptr, &err);
    if (err != CL_SUCCESS) {
        qDebug() << "Failed to create OpenCL context";
        return false;
    }
    
    return true;
}

void OpenCLCompiler::cleanup()
{
    if (m_context) {
        clReleaseContext(m_context);
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
        result.buildLog = "OpenCL not initialized";
        return result;
    }
    
    cl_int err;
    const char *sourceStr = source.toUtf8().constData();
    size_t sourceSize = source.toUtf8().size();
    
    // Create program
    cl_program program = clCreateProgramWithSource(m_context, 1, &sourceStr, &sourceSize, &err);
    if (err != CL_SUCCESS) {
        result.buildLog = QString("Failed to create program: %1").arg(err);
        return result;
    }
    
    // Build program
    err = clBuildProgram(program, 1, &m_device, "-cl-std=CL1.2", nullptr, nullptr);
    
    // Get build log
    size_t logSize;
    clGetProgramBuildInfo(program, m_device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);
    
    if (logSize > 0) {
        std::vector<char> log(logSize);
        clGetProgramBuildInfo(program, m_device, CL_PROGRAM_BUILD_LOG, logSize, log.data(), nullptr);
        result.buildLog = QString::fromUtf8(log.data());
        
        // Parse messages from build log
        result.messages = parseCompilerOutput(result.buildLog);
    }
    
    if (err == CL_SUCCESS) {
        result.success = true;
        
        // Get binary size
        size_t binarySize;
        clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, sizeof(size_t), &binarySize, nullptr);
        
        if (binarySize > 0) {
            // Get binary
            result.compiledBinary.resize(binarySize);
            unsigned char *binary = reinterpret_cast<unsigned char*>(result.compiledBinary.data());
            clGetProgramInfo(program, CL_PROGRAM_BINARIES, sizeof(unsigned char*), &binary, nullptr);
        }
    }
    
    clReleaseProgram(program);
    
    return result;
}

QStringList OpenCLCompiler::getAvailableDevices() const
{
    QStringList devices;
    
    if (!m_initialized) {
        return devices;
    }
    
    cl_uint numDevices;
    cl_int err = clGetDeviceIDs(m_platform, CL_DEVICE_TYPE_ALL, 0, nullptr, &numDevices);
    if (err != CL_SUCCESS) {
        return devices;
    }
    
    std::vector<cl_device_id> deviceIds(numDevices);
    err = clGetDeviceIDs(m_platform, CL_DEVICE_TYPE_ALL, numDevices, deviceIds.data(), nullptr);
    if (err != CL_SUCCESS) {
        return devices;
    }
    
    for (cl_device_id device : deviceIds) {
        char name[256];
        clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(name), name, nullptr);
        devices.append(QString::fromUtf8(name));
    }
    
    return devices;
}
#endif

#ifdef HAS_CUDA
CUDACompiler::CUDACompiler()
    : m_initialized(false)
    , m_deviceCount(0)
{
    m_initialized = initializeCUDA();
}

CUDACompiler::~CUDACompiler()
{
    cleanup();
}

bool CUDACompiler::initializeCUDA()
{
    CUresult err = cuInit(0);
    if (err != CUDA_SUCCESS) {
        qDebug() << "Failed to initialize CUDA";
        return false;
    }
    
    err = cuDeviceGetCount(&m_deviceCount);
    if (err != CUDA_SUCCESS || m_deviceCount == 0) {
        qDebug() << "No CUDA devices found";
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
        result.buildLog = "CUDA not initialized";
        return result;
    }
    
    nvrtcProgram prog;
    nvrtcResult res = nvrtcCreateProgram(&prog,
                                        source.toUtf8().constData(),
                                        "kernel.cu",
                                        0,
                                        nullptr,
                                        nullptr);
    
    if (res != NVRTC_SUCCESS) {
        result.buildLog = QString("Failed to create program: %1").arg(nvrtcGetErrorString(res));
        return result;
    }
    
    // Compile with common options
    const char *opts[] = {
        "--gpu-architecture=compute_50",
        "--fmad=true"
    };
    
    res = nvrtcCompileProgram(prog, 2, opts);
    
    // Get compilation log
    size_t logSize;
    nvrtcGetProgramLogSize(prog, &logSize);
    
    if (logSize > 0) {
        std::vector<char> log(logSize);
        nvrtcGetProgramLog(prog, log.data());
        result.buildLog = QString::fromUtf8(log.data());
        
        // Parse messages
        result.messages = parseCompilerOutput(result.buildLog);
    }
    
    if (res == NVRTC_SUCCESS) {
        result.success = true;
        
        // Get PTX
        size_t ptxSize;
        nvrtcGetPTXSize(prog, &ptxSize);
        
        if (ptxSize > 0) {
            result.compiledBinary.resize(ptxSize);
            nvrtcGetPTX(prog, result.compiledBinary.data());
        }
    }
    
    nvrtcDestroyProgram(&prog);
    
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
        if (cuDeviceGet(&device, i) == CUDA_SUCCESS) {
            char name[256];
            cuDeviceGetName(name, sizeof(name), device);
            devices.append(QString::fromUtf8(name));
        }
    }
    
    return devices;
}
#endif

#ifdef HAS_METAL
MetalCompiler::MetalCompiler()
    : m_initialized(true)
{
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
    
    return result;
}

QStringList MetalCompiler::getAvailableDevices() const
{
    // TODO: Query actual Metal devices
    return QStringList() << "Default Metal Device";
}
#endif