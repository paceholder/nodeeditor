#ifndef METALCOMPILER_HPP
#define METALCOMPILER_HPP

#include "SimpleGPUCompiler.hpp"

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

#endif // METALCOMPILER_HPP