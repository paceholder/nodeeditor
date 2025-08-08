#ifndef SIMPLEGPUCOMPILER_HPP
#define SIMPLEGPUCOMPILER_HPP

#include <QString>
#include <QVector>
#include <QByteArray>

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

#endif // SIMPLEGPUCOMPILER_HPP