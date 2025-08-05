#ifndef CODEEDITOR_HPP
#define CODEEDITOR_HPP

#include <QWidget>
#include <QString>
#include <QStringList>
#include <memory>

class QsciScintilla;
class GPULanguageLexer;
class QsciAPIs;

class CodeEditor : public QWidget
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);
    ~CodeEditor();

    // Code management
    void setCode(const QString &code);
    QString getCode() const;
    
    // Language management
    void setLanguage(const QString &language);
    QString getCurrentLanguage() const;
    QStringList getSupportedLanguages() const { return m_supportedLanguages; }
    
    // Editor settings
    void setReadOnly(bool readOnly);
    bool isReadOnly() const;
    
    // Theme management
    void setDarkMode(bool dark);
    bool isDarkMode() const { return m_isDarkMode; }
    
    // Editor access (for advanced operations)
    QsciScintilla* editor() { return m_editor; }
    const QsciScintilla* editor() const { return m_editor; }
    
    // Cursor and selection
    void setCursorPosition(int line, int column);
    void getCursorPosition(int &line, int &column) const;
    void ensureLineVisible(int line);
    void highlightLine(int line, int duration = 2000);
    
    // Focus
    void setFocus();

signals:
    void codeChanged();
    void languageChanged(const QString &language);

private slots:
    void onTextChanged();

private:
    void setupEditor();
    void setupCommonEditorFeatures();
    void updateHighlighter();
    void updateLexerColors();
    void forceRefreshLexer();
    void applyDarkTheme();
    void applyLightTheme();
    void setupAutoCompletion();
    
    // Default code generators
    QString getDefaultCode(const QString &language) const;

    // UI elements
    QsciScintilla *m_editor;
    GPULanguageLexer *m_lexer;
    
    // State
    QString m_currentLanguage;
    QStringList m_supportedLanguages;
    bool m_isDarkMode;
};

#endif // CODEEDITOR_HPP