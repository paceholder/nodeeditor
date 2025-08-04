#include "compilers/SimpleGPUCompiler.hpp"
#include <QRegularExpression>

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