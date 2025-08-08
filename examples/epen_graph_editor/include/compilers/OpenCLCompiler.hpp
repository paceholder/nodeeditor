#ifndef OPENCLCOMPILER_HPP
#define OPENCLCOMPILER_HPP

#include "SimpleGPUCompiler.hpp"
#include <memory>
#include <QLibrary>

// Check for OpenCL headers
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
    QString getErrorString(int error) const;

    // Function pointers for OpenCL functions
    struct OpenCLFunctions;
    std::unique_ptr<OpenCLFunctions> m_functions;

    std::unique_ptr<QLibrary> m_openclLib;
    void *m_platform;
    void *m_device;
    void *m_context;
    bool m_initialized;
    QString m_availabilityError;
};

#endif // OPENCLCOMPILER_HPP