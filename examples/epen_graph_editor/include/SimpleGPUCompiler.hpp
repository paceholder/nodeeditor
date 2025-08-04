#ifndef SIMPLEGPUCOMPILER_HPP
#define SIMPLEGPUCOMPILER_HPP

#include <QString>
#include <QVector>
#include <QByteArray>
#include <QLibrary>
#include <memory>

// Always include headers, but make them optional
#ifdef __APPLE__
    #ifdef __has_include
        #if __has_include(<OpenCL/opencl.h>)
            #define HAS_OPENCL_HEADERS
            #include <OpenCL/opencl.h>
        #endif
    #endif
#else
    #ifdef __has_include
        #if __has_include(<CL/cl.h>)
            #define HAS_OPENCL_HEADERS
            #include <CL/cl.h>
        #endif
    #endif
#endif

#ifdef __has_include
    #if __has_include(<cuda.h>) && __has_include(<nvrtc.h>)
        #define HAS_CUDA_HEADERS
        #include <cuda.h>
        #include <nvrtc.h>
    #endif
#endif

class SimpleGPUCompiler
{
public:
    // Compilation message structure
    struct CompileMessage {
        enum Type {
            Error,
            Warning,
            Info
        };
        
        Type type;
        QString message;
        int line;
        int column;
        QString file;
    };
    
    struct CompileResult {
        bool success;
        QString buildLog;
        QVector<CompileMessage> messages;
        QByteArray compiledBinary;
    };

    SimpleGPUCompiler();
    virtual ~SimpleGPUCompiler();

    // Check if the compiler is available
    virtual bool isAvailable() const = 0;
    
    // Get compiler name
    virtual QString getName() const = 0;
    
    // Get availability error message if not available
    virtual QString getAvailabilityError() const = 0;
    
    // Compile source code
    virtual CompileResult compile(const QString &source, const QString &kernelName = "kernel") = 0;
    
    // Get available devices
    virtual QStringList getAvailableDevices() const = 0;

protected:
    // Parse compiler output to extract errors/warnings
    QVector<CompileMessage> parseCompilerOutput(const QString &output);
};

// Always define all compiler classes, but implementation varies based on runtime availability
class OpenCLCompiler : public SimpleGPUCompiler
{
public:
    OpenCLCompiler();
    ~OpenCLCompiler() override;

    bool isAvailable() const override;
    QString getName() const override { return "OpenCL"; }
    QString getAvailabilityError() const override { return m_availabilityError; }
    CompileResult compile(const QString &source, const QString &kernelName = "kernel") override;
    QStringList getAvailableDevices() const override;

private:
    bool initializeOpenCL();
    void cleanup();
    bool loadOpenCLLibrary();
    
    // Function pointers for OpenCL functions
    struct OpenCLFunctions;
    std::unique_ptr<OpenCLFunctions> m_functions;
    
    std::unique_ptr<QLibrary> m_openclLib;
    void* m_platform;
    void* m_device;
    void* m_context;
    bool m_initialized;
    QString m_availabilityError;
};

class CUDACompiler : public SimpleGPUCompiler
{
public:
    CUDACompiler();
    ~CUDACompiler() override;

    bool isAvailable() const override;
    QString getName() const override { return "CUDA"; }
    QString getAvailabilityError() const override { return m_availabilityError; }
    CompileResult compile(const QString &source, const QString &kernelName = "kernel") override;
    QStringList getAvailableDevices() const override;

private:
    bool initializeCUDA();
    void cleanup();
    bool loadCUDALibraries();
    
    // Function pointers for CUDA functions
    struct CUDAFunctions;
    std::unique_ptr<CUDAFunctions> m_functions;
    
    std::unique_ptr<QLibrary> m_cudaLib;
    std::unique_ptr<QLibrary> m_nvrtcLib;
    bool m_initialized;
    int m_deviceCount;
    QString m_availabilityError;
};

class MetalCompiler : public SimpleGPUCompiler
{
public:
    MetalCompiler();
    ~MetalCompiler() override;

    bool isAvailable() const override;
    QString getName() const override { return "Metal"; }
    QString getAvailabilityError() const override { return m_availabilityError; }
    CompileResult compile(const QString &source, const QString &kernelName = "kernel") override;
    QStringList getAvailableDevices() const override;

private:
    bool m_initialized;
    QString m_availabilityError;
};

#endif // SIMPLEGPUCOMPILER_HPP