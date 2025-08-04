#include "compilers/OpenCLCompiler.hpp"
#include <QDebug>
#include <vector>

// OpenCL type definitions and function pointers
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