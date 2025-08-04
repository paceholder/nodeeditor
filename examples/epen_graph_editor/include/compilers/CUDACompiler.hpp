#ifndef CUDACOMPILER_HPP
#define CUDACOMPILER_HPP

#include "SimpleGPUCompiler.hpp"
#include <QLibrary>
#include <memory>

// Check for CUDA headers
#ifdef __has_include
    #if __has_include(<cuda.h>) && __has_include(<nvrtc.h>)
        #define HAS_CUDA_HEADERS
        #include <cuda.h>
        #include <nvrtc.h>
    #endif
#endif

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

#endif // CUDACOMPILER_HPP