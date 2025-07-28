#ifndef FLOATINGCODEEDITOR_HPP
#define FLOATINGCODEEDITOR_HPP

#include "FloatingPanelBase.hpp"
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <memory>

class QComboBox;
class QPushButton;
class QPlainTextEdit;
class GraphEditorWindow;
class CodeHighlighter;

class FloatingCodeEditor : public FloatingPanelBase
{
    Q_OBJECT

public:
    explicit FloatingCodeEditor(GraphEditorWindow *parent = nullptr);
    ~FloatingCodeEditor();

    // Set/get code content
    void setCode(const QString &code);
    QString getCode() const;

    // Set current language
    void setLanguage(const QString &language);
    QString getCurrentLanguage() const;

    // Enable/disable editing
    void setReadOnly(bool readOnly);

signals:
    void compileRequested(const QString &code, const QString &language);
    void languageChanged(const QString &language);
    void codeChanged();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void setupUI() override;
    void connectSignals() override;

private slots:
    void onLanguageChanged(int index);
    void onCompileClicked();
    void onCodeChanged();

private:
    void updateHighlighter();

    // UI elements
    QComboBox *m_languageCombo;
    QPlainTextEdit *m_codeEditor;
    QPushButton *m_compileButton;
    
    // Syntax highlighter
    std::unique_ptr<CodeHighlighter> m_highlighter;
    
    // Languages
    QStringList m_supportedLanguages;
};

// Simple syntax highlighter base class
class CodeHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit CodeHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override = 0;

    // Common highlighting helpers
    void applyFormat(int start, int length, const QTextCharFormat &format);
    
    // Common formats
    QTextCharFormat m_keywordFormat;
    QTextCharFormat m_typeFormat;
    QTextCharFormat m_commentFormat;
    QTextCharFormat m_stringFormat;
    QTextCharFormat m_numberFormat;
    QTextCharFormat m_functionFormat;
};

// OpenCL highlighter
class OpenCLHighlighter : public CodeHighlighter
{
public:
    explicit OpenCLHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    QStringList m_keywords;
    QStringList m_types;
};

// CUDA highlighter
class CUDAHighlighter : public CodeHighlighter
{
public:
    explicit CUDAHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    QStringList m_keywords;
    QStringList m_types;
    QStringList m_cudaSpecific;
};

// Metal highlighter
class MetalHighlighter : public CodeHighlighter
{
public:
    explicit MetalHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    QStringList m_keywords;
    QStringList m_types;
    QStringList m_metalSpecific;
};

#endif // FLOATINGCODEEDITOR_HPP