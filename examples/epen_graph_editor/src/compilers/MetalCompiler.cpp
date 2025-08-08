#include "compilers/MetalCompiler.hpp"
#include <QProcess>
#include <QTemporaryFile>
#include <QDir>

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