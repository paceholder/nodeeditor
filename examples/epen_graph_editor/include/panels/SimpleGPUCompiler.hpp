#ifndef SIMPLEGPUCOMPILER_HPP
#define SIMPLEGPUCOMPILER_HPP

#include <QString>
#include <QVector>
#include <QByteArray>
#include <memory>

#ifdef HAS_OPENCL
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#endif

#ifdef HAS_CUDA
#include <cuda.h>
#include <nvrtc.h>
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
    
    // Compile source code
    virtual CompileResult compile(const QString &source, const QString &kernelName = "kernel") = 0;
    
    // Get available devices
    virtual QStringList getAvailableDevices() const = 0;

protected:
    // Parse compiler output to extract errors/warnings
    QVector<CompileMessage> parseCompilerOutput(const QString &output);
};

#ifdef HAS_OPENCL
class OpenCLCompiler : public SimpleGPUCompiler
{
public:
    OpenCLCompiler();
    ~OpenCLCompiler() override;

    bool isAvailable() const override;
    QString getName() const override { return "OpenCL"; }
    CompileResult compile(const QString &source, const QString &kernelName = "kernel") override;
    QStringList getAvailableDevices() const override;

private:
    bool initializeOpenCL();
    void cleanup();
    
    cl_platform_id m_platform;
    cl_device_id m_device;
    cl_context m_context;
    bool m_initialized;
};
#endif

#ifdef HAS_CUDA
class CUDACompiler : public SimpleGPUCompiler
{
public:
    CUDACompiler();
    ~CUDACompiler() override;

    bool isAvailable() const override;
    QString getName() const override { return "CUDA"; }
    CompileResult compile(const QString &source, const QString &kernelName = "kernel") override;
    QStringList getAvailableDevices() const override;

private:
    bool initializeCUDA();
    void cleanup();
    
    bool m_initialized;
    int m_deviceCount;
};
#endif

#ifdef HAS_METAL
class MetalCompiler : public SimpleGPUCompiler
{
public:
    MetalCompiler();
    ~MetalCompiler() override;

    bool isAvailable() const override;
    QString getName() const override { return "Metal"; }
    CompileResult compile(const QString &source, const QString &kernelName = "kernel") override;
    QStringList getAvailableDevices() const override;

private:
    bool m_initialized;
};
#endif

// GLSL compiler is always available as fallback
class GLSLComputeCompiler : public SimpleGPUCompiler
{
public:
    GLSLComputeCompiler();
    ~GLSLComputeCompiler() override;

    bool isAvailable() const override { return true; }
    QString getName() const override { return "GLSL Compute"; }
    CompileResult compile(const QString &source, const QString &kernelName = "kernel") override;
    QStringList getAvailableDevices() const override;
};

#endif // SIMPLEGPUCOMPILER_HPP