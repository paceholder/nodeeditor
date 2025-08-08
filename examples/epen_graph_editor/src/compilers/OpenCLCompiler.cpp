#include "compilers/OpenCLCompiler.hpp"
#include <vector>
#include <QDebug>

// OpenCL type definitions and function pointers
#ifdef HAS_OPENCL_HEADERS
// If we have headers, we can use the actual types
typedef cl_int (*clGetPlatformIDs_fn)(cl_uint, cl_platform_id *, cl_uint *);
typedef cl_int (*clGetDeviceIDs_fn)(
    cl_platform_id, cl_device_type, cl_uint, cl_device_id *, cl_uint *);
typedef cl_context (*clCreateContext_fn)(const cl_context_properties *,
                                         cl_uint,
                                         const cl_device_id *,
                                         void (*)(const char *, const void *, size_t, void *),
                                         void *,
                                         cl_int *);
typedef cl_int (*clReleaseContext_fn)(cl_context);
typedef cl_program (*clCreateProgramWithSource_fn)(
    cl_context, cl_uint, const char **, const size_t *, cl_int *);
typedef cl_int (*clBuildProgram_fn)(
    cl_program, cl_uint, const cl_device_id *, const char *, void (*)(cl_program, void *), void *);
typedef cl_int (*clGetProgramBuildInfo_fn)(
    cl_program, cl_device_id, cl_program_build_info, size_t, void *, size_t *);
typedef cl_int (*clGetProgramInfo_fn)(cl_program, cl_program_info, size_t, void *, size_t *);
typedef cl_int (*clReleaseProgram_fn)(cl_program);
typedef cl_int (*clGetDeviceInfo_fn)(cl_device_id, cl_device_info, size_t, void *, size_t *);
typedef cl_int (*clGetPlatformInfo_fn)(cl_platform_id, cl_platform_info, size_t, void *, size_t *);
#else
// Define minimal types if headers not available
typedef int cl_int;
typedef unsigned int cl_uint;
typedef void *cl_platform_id;
typedef void *cl_device_id;
typedef void *cl_context;
typedef void *cl_program;
typedef unsigned long cl_device_type;
typedef unsigned int cl_program_build_info;
typedef unsigned int cl_program_info;
typedef unsigned int cl_device_info;
typedef unsigned int cl_platform_info;
typedef intptr_t cl_context_properties;

#define CL_SUCCESS 0
#define CL_DEVICE_TYPE_GPU 0x04
#define CL_DEVICE_TYPE_CPU 0x02
#define CL_DEVICE_TYPE_ALL 0xFFFFFFFF
#define CL_PROGRAM_BUILD_LOG 0x1183
#define CL_PROGRAM_BINARY_SIZES 0x1165
#define CL_PROGRAM_BINARIES 0x1166
#define CL_DEVICE_NAME 0x102B
#define CL_PLATFORM_NAME 0x0902
#define CL_PLATFORM_VENDOR 0x0903
#define CL_DEVICE_VERSION 0x102F
#define CL_DEVICE_VENDOR 0x102C

// Error codes
#define CL_INVALID_PLATFORM -32
#define CL_INVALID_DEVICE -33
#define CL_INVALID_CONTEXT -34
#define CL_INVALID_PROGRAM -44
#define CL_INVALID_BUILD_OPTIONS -43
#define CL_BUILD_PROGRAM_FAILURE -11

typedef cl_int (*clGetPlatformIDs_fn)(cl_uint, cl_platform_id *, cl_uint *);
typedef cl_int (*clGetDeviceIDs_fn)(
    cl_platform_id, cl_device_type, cl_uint, cl_device_id *, cl_uint *);
typedef cl_context (*clCreateContext_fn)(
    const cl_context_properties *, cl_uint, const cl_device_id *, void *, void *, cl_int *);
typedef cl_int (*clReleaseContext_fn)(cl_context);
typedef cl_program (*clCreateProgramWithSource_fn)(
    cl_context, cl_uint, const char **, const size_t *, cl_int *);
typedef cl_int (*clBuildProgram_fn)(
    cl_program, cl_uint, const cl_device_id *, const char *, void *, void *);
typedef cl_int (*clGetProgramBuildInfo_fn)(
    cl_program, cl_device_id, cl_program_build_info, size_t, void *, size_t *);
typedef cl_int (*clGetProgramInfo_fn)(cl_program, cl_program_info, size_t, void *, size_t *);
typedef cl_int (*clReleaseProgram_fn)(cl_program);
typedef cl_int (*clGetDeviceInfo_fn)(cl_device_id, cl_device_info, size_t, void *, size_t *);
typedef cl_int (*clGetPlatformInfo_fn)(cl_platform_id, cl_platform_info, size_t, void *, size_t *);
#endif

struct OpenCLCompiler::OpenCLFunctions
{
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
    clGetPlatformInfo_fn clGetPlatformInfo;
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

QString OpenCLCompiler::getErrorString(int error) const
{
    switch (error) {
    case CL_SUCCESS:
        return "Success";
    case CL_INVALID_PLATFORM:
        return "Invalid platform";
    case CL_INVALID_DEVICE:
        return "Invalid device";
    case CL_INVALID_CONTEXT:
        return "Invalid context";
    case CL_INVALID_PROGRAM:
        return "Invalid program";
    case CL_INVALID_BUILD_OPTIONS:
        return "Invalid build options";
    case CL_BUILD_PROGRAM_FAILURE:
        return "Build program failure";
    default:
        return QString("Unknown error (%1)").arg(error);
    }
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

    bool loaded = false;
    QString lastError;

    for (const QString &libName : libraryNames) {
        m_openclLib->setFileName(libName);
        if (m_openclLib->load()) {
            loaded = true;
            qDebug() << "Loaded OpenCL library:" << libName;
            break;
        }
        lastError = m_openclLib->errorString();
        qDebug() << "Failed to load" << libName << ":" << lastError;
    }

    if (!loaded) {
        m_availabilityError
            = "OpenCL library not found. Please install OpenCL drivers for your GPU. Last error: "
              + lastError;
        return false;
    }

    // Load function pointers
    m_functions->clGetPlatformIDs = (clGetPlatformIDs_fn) m_openclLib->resolve("clGetPlatformIDs");
    m_functions->clGetDeviceIDs = (clGetDeviceIDs_fn) m_openclLib->resolve("clGetDeviceIDs");
    m_functions->clCreateContext = (clCreateContext_fn) m_openclLib->resolve("clCreateContext");
    m_functions->clReleaseContext = (clReleaseContext_fn) m_openclLib->resolve("clReleaseContext");
    m_functions->clCreateProgramWithSource = (clCreateProgramWithSource_fn) m_openclLib->resolve(
        "clCreateProgramWithSource");
    m_functions->clBuildProgram = (clBuildProgram_fn) m_openclLib->resolve("clBuildProgram");
    m_functions->clGetProgramBuildInfo = (clGetProgramBuildInfo_fn) m_openclLib->resolve(
        "clGetProgramBuildInfo");
    m_functions->clGetProgramInfo = (clGetProgramInfo_fn) m_openclLib->resolve("clGetProgramInfo");
    m_functions->clReleaseProgram = (clReleaseProgram_fn) m_openclLib->resolve("clReleaseProgram");
    m_functions->clGetDeviceInfo = (clGetDeviceInfo_fn) m_openclLib->resolve("clGetDeviceInfo");
    m_functions->clGetPlatformInfo = (clGetPlatformInfo_fn) m_openclLib->resolve(
        "clGetPlatformInfo");

    // Check if all functions were loaded
    if (!m_functions->clGetPlatformIDs || !m_functions->clGetDeviceIDs
        || !m_functions->clCreateContext || !m_functions->clReleaseContext
        || !m_functions->clCreateProgramWithSource || !m_functions->clBuildProgram
        || !m_functions->clGetProgramBuildInfo || !m_functions->clGetProgramInfo
        || !m_functions->clReleaseProgram || !m_functions->clGetDeviceInfo) {
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

    // Get platform count first
    cl_uint numPlatforms = 0;
    err = m_functions->clGetPlatformIDs(0, nullptr, &numPlatforms);
    if (err != CL_SUCCESS) {
        m_availabilityError = QString("Failed to query OpenCL platforms: %1")
                                  .arg(getErrorString(err));
        return false;
    }

    if (numPlatforms == 0) {
        m_availabilityError = "No OpenCL platforms found. Please install OpenCL drivers.";
        return false;
    }

    qDebug() << "Found" << numPlatforms << "OpenCL platform(s)";

    // Get all platforms
    std::vector<cl_platform_id> platforms(numPlatforms);
    err = m_functions->clGetPlatformIDs(numPlatforms, platforms.data(), nullptr);
    if (err != CL_SUCCESS) {
        m_availabilityError = QString("Failed to get OpenCL platforms: %1").arg(getErrorString(err));
        return false;
    }

    // Try each platform to find one with GPU support
    bool foundDevice = false;
    for (cl_uint i = 0; i < numPlatforms; i++) {
        // Get platform info
        if (m_functions->clGetPlatformInfo) {
            char platformName[256];
            m_functions->clGetPlatformInfo(platforms[i],
                                           CL_PLATFORM_NAME,
                                           sizeof(platformName),
                                           platformName,
                                           nullptr);
            qDebug() << "Checking platform" << i << ":" << platformName;
        }

        // Try to get GPU devices
        cl_uint numDevices = 0;
        err = m_functions->clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, 0, nullptr, &numDevices);

        if (err == CL_SUCCESS && numDevices > 0) {
            m_platform = platforms[i];

            // Get devices
            std::vector<cl_device_id> devices(numDevices);
            err = m_functions->clGetDeviceIDs((cl_platform_id) m_platform,
                                              CL_DEVICE_TYPE_GPU,
                                              numDevices,
                                              devices.data(),
                                              nullptr);
            if (err == CL_SUCCESS) {
                m_device = devices[0];
                foundDevice = true;

                // Get device name for debugging
                char deviceName[256];
                m_functions->clGetDeviceInfo((cl_device_id) m_device,
                                             CL_DEVICE_NAME,
                                             sizeof(deviceName),
                                             deviceName,
                                             nullptr);
                qDebug() << "Using GPU device:" << deviceName;
                break;
            }
        }
    }

    // If no GPU found, try CPU
    if (!foundDevice) {
        qDebug() << "No GPU found, trying CPU...";

        for (cl_uint i = 0; i < numPlatforms; i++) {
            cl_uint numDevices = 0;
            err = m_functions->clGetDeviceIDs(platforms[i],
                                              CL_DEVICE_TYPE_CPU,
                                              0,
                                              nullptr,
                                              &numDevices);

            if (err == CL_SUCCESS && numDevices > 0) {
                m_platform = platforms[i];

                std::vector<cl_device_id> devices(numDevices);
                err = m_functions->clGetDeviceIDs((cl_platform_id) m_platform,
                                                  CL_DEVICE_TYPE_CPU,
                                                  numDevices,
                                                  devices.data(),
                                                  nullptr);
                if (err == CL_SUCCESS) {
                    m_device = devices[0];
                    foundDevice = true;

                    char deviceName[256];
                    m_functions->clGetDeviceInfo((cl_device_id) m_device,
                                                 CL_DEVICE_NAME,
                                                 sizeof(deviceName),
                                                 deviceName,
                                                 nullptr);
                    qDebug() << "Using CPU device:" << deviceName;
                    break;
                }
            }
        }
    }

    if (!foundDevice) {
        m_availabilityError = "No OpenCL devices found. Please check your GPU drivers.";
        return false;
    }

    // Create context
    m_context = m_functions->clCreateContext(nullptr,
                                             1,
                                             (cl_device_id *) &m_device,
                                             nullptr,
                                             nullptr,
                                             &err);
    if (err != CL_SUCCESS) {
        m_availabilityError = QString("Failed to create OpenCL context: %1").arg(getErrorString(err));
        return false;
    }

    qDebug() << "OpenCL initialized successfully";
    return true;
}

void OpenCLCompiler::cleanup()
{
    if (m_context && m_functions && m_functions->clReleaseContext) {
        m_functions->clReleaseContext((cl_context) m_context);
        m_context = nullptr;
    }

    if (m_openclLib && m_openclLib->isLoaded()) {
        m_openclLib->unload();
    }

    m_initialized = false;
}

bool OpenCLCompiler::isAvailable() const
{
    return m_initialized;
}

SimpleGPUCompiler::CompileResult OpenCLCompiler::compile(const QString &source,
                                                         const QString &kernelName)
{
    CompileResult result;
    result.success = false;

    if (!m_initialized) {
        result.buildLog = "OpenCL not initialized: " + m_availabilityError;
        return result;
    }

    qDebug() << "Compiling OpenCL kernel...";
    qDebug() << "Source code:" << source;

    cl_int err;

    // Store source in a stable variable
    QByteArray sourceData = source.toUtf8();
    const char *sourceStr = sourceData.constData();
    size_t sourceSize = sourceData.size();

    qDebug() << "Source size:" << sourceSize << "bytes";

    // Create program
    cl_program program = m_functions->clCreateProgramWithSource((cl_context) m_context,
                                                                1,
                                                                &sourceStr,
                                                                &sourceSize,
                                                                &err);
    if (err != CL_SUCCESS) {
        result.buildLog = QString("Failed to create program: %1").arg(getErrorString(err));
        qDebug() << result.buildLog;
        return result;
    }

    qDebug() << "Program created, building...";

    // Build program with no special flags (let OpenCL use defaults)
    err = m_functions->clBuildProgram(program, 1, (cl_device_id *) &m_device, "", nullptr, nullptr);

    // Always get build log (even on success, it might contain warnings)
    size_t logSize = 0;
    m_functions->clGetProgramBuildInfo(program,
                                       (cl_device_id) m_device,
                                       CL_PROGRAM_BUILD_LOG,
                                       0,
                                       nullptr,
                                       &logSize);

    if (logSize > 1) { // 1 because it includes null terminator
        std::vector<char> log(logSize);
        m_functions->clGetProgramBuildInfo(program,
                                           (cl_device_id) m_device,
                                           CL_PROGRAM_BUILD_LOG,
                                           logSize,
                                           log.data(),
                                           nullptr);
        result.buildLog = QString::fromUtf8(log.data()).trimmed();

        if (!result.buildLog.isEmpty()) {
            qDebug() << "Build log:" << result.buildLog;

            // Parse messages from build log
            result.messages = parseCompilerOutput(result.buildLog);
        }
    }

    if (err != CL_SUCCESS) {
        result.success = false;
        if (result.buildLog.isEmpty()) {
            result.buildLog = QString("Build failed with error: %1").arg(getErrorString(err));
        }
        qDebug() << "Build failed:" << err;
    } else {
        result.success = true;
        qDebug() << "Build succeeded";

        // Get binary size
        size_t binarySize = 0;
        err = m_functions->clGetProgramInfo(program,
                                            CL_PROGRAM_BINARY_SIZES,
                                            sizeof(size_t),
                                            &binarySize,
                                            nullptr);

        if (err == CL_SUCCESS && binarySize > 0) {
            qDebug() << "Binary size:" << binarySize << "bytes";

            // Get binary
            result.compiledBinary.resize(binarySize);
            unsigned char *binary = reinterpret_cast<unsigned char *>(result.compiledBinary.data());
            err = m_functions->clGetProgramInfo(program,
                                                CL_PROGRAM_BINARIES,
                                                sizeof(unsigned char *),
                                                &binary,
                                                nullptr);

            if (err != CL_SUCCESS) {
                qDebug() << "Failed to get binary:" << getErrorString(err);
                result.compiledBinary.clear();
            }
        }

        if (result.buildLog.isEmpty()) {
            result.buildLog = "Compilation successful";
        }
    }

    // Clean up
    m_functions->clReleaseProgram(program);

    return result;
}

QStringList OpenCLCompiler::getAvailableDevices() const
{
    QStringList devices;

    if (!m_initialized) {
        return devices;
    }

    cl_uint numDevices = 0;
    cl_int err = m_functions->clGetDeviceIDs((cl_platform_id) m_platform,
                                             CL_DEVICE_TYPE_ALL,
                                             0,
                                             nullptr,
                                             &numDevices);
    if (err != CL_SUCCESS || numDevices == 0) {
        return devices;
    }

    std::vector<cl_device_id> deviceIds(numDevices);
    err = m_functions->clGetDeviceIDs((cl_platform_id) m_platform,
                                      CL_DEVICE_TYPE_ALL,
                                      numDevices,
                                      deviceIds.data(),
                                      nullptr);
    if (err != CL_SUCCESS) {
        return devices;
    }

    for (cl_device_id device : deviceIds) {
        char name[256] = {0};
        err = m_functions->clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(name), name, nullptr);
        if (err == CL_SUCCESS) {
            // Get device type
            cl_device_type deviceType;
            m_functions->clGetDeviceInfo(device,
                                         CL_DEVICE_TYPE,
                                         sizeof(deviceType),
                                         &deviceType,
                                         nullptr);

            QString deviceTypeStr;
            if (deviceType & CL_DEVICE_TYPE_GPU)
                deviceTypeStr = "GPU";
            else if (deviceType & CL_DEVICE_TYPE_CPU)
                deviceTypeStr = "CPU";
            else
                deviceTypeStr = "Unknown";

            devices.append(QString("%1 (%2)").arg(name).arg(deviceTypeStr));
        }
    }

    return devices;
}