#ifndef COMPILERESULTSWIDGET_HPP
#define COMPILERESULTSWIDGET_HPP

#include <QWidget>
#include <QVector>
#include <functional>
#include "compilers/SimpleGPUCompiler.hpp"

QT_BEGIN_NAMESPACE
class QTextEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QTabWidget;
QT_END_NAMESPACE

class CompileResultsWidget : public QWidget
{
    Q_OBJECT

public:
    using CompileMessage = SimpleGPUCompiler::CompileMessage;

    explicit CompileResultsWidget(QWidget *parent = nullptr);
    ~CompileResultsWidget();

    // Clear all results
    void clear();
    
    // Add a compilation message
    void addMessage(const CompileMessage &message);
    void addMessages(const QVector<CompileMessage> &messages);
    
    // Set raw output
    void setRawOutput(const QString &output);
    
    // Set success/failure status
    void setCompilationStatus(bool success, const QString &summary = QString());

signals:
    // Emitted when user clicks on an error/warning
    void messageClicked(int line, int column);
    void closeRequested();

private slots:
    void onItemClicked(QListWidgetItem *item);

private:
    void setupUI();
    QString getIconForType(CompileMessage::Type type) const;
    QColor getColorForType(CompileMessage::Type type) const;

    QTabWidget *m_tabWidget;
    QTextEdit *m_rawOutputEdit;
    QListWidget *m_messageList;
    QPushButton *m_clearButton;
    QPushButton *m_closeButton;
    
    QVector<CompileMessage> m_messages;
};

#endif // COMPILERESULTSWIDGET_HPP