#ifndef FLOATINGCODEEDITOR_HPP
#define FLOATINGCODEEDITOR_HPP

#include "FloatingPanelBase.hpp"
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexercustom.h>
#include <QTimer>
#include <QSet>
#include <memory>
#include <map>

class QComboBox;
class QPushButton;
class QCheckBox;
class GraphEditorWindow;
class CompileResultsWidget;
class SimpleGPUCompiler;

// Custom lexer for GPU languages
class GPULanguageLexer : public QsciLexerCustom
{
    Q_OBJECT

public:
    enum {
        Default = 0,
        Comment,
        CommentLine,
        Number,
        Keyword,
        LanguageKeyword,  // For language-specific keywords
        String,
        Operator,
        Identifier,
        Preprocessor
    };

    explicit GPULanguageLexer(QObject *parent = nullptr);
    
    const char *language() const override;
    QString description(int style) const override;
    void styleText(int start, int end) override;
    QColor defaultColor(int style) const override;
    QColor defaultPaper(int style) const override;
    QFont defaultFont(int style) const override;
    
    void setLanguageMode(const QString &language);
    void setDarkMode(bool dark);
    
    // Get all identifiers from the text
    QSet<QString> getIdentifiers() const { return m_identifiers; }
    void clearIdentifiers() { m_identifiers.clear(); }
    
    // Public helper methods for error checking
    bool isKeyword(const QString &word) const;
    bool isLanguageKeyword(const QString &word) const;
    bool isBuiltinFunction(const QString &word) const;
    bool isBuiltinType(const QString &word) const;

private:
    bool isNumber(const QString &text) const;
    
    QString m_language;
    bool m_isDarkMode;
    QStringList m_keywords;
    QStringList m_languageKeywords;
    QStringList m_types;
    QStringList m_builtinFunctions;
    QSet<QString> m_identifiers;  // Track all identifiers found
};

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
    
    // Compilation results buffer
    QByteArray getCompiledBinary() const { return m_compiledBinary; }

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
    void onThemeToggled(bool checked);
    void onTextChanged();
    void performErrorCheck();
    void onResultsCloseRequested();
    void onMessageClicked(int line, int column);

private:
    void updateHighlighter();
    void setupCommonEditorFeatures();
    void applyDarkTheme();
    void applyLightTheme();
    void setupErrorIndicator();
    void checkForErrors();
    void performCompilation();
    void initializeCompilers();
    void showCompileResults(bool show);
    void forceRefreshLexer();  // Added method for forcing lexer refresh
    void updateLexerColors();  // Added method for updating lexer colors explicitly
    
    struct ErrorInfo {
        int line;
        int indexStart;
        int indexEnd;
        QString message;
    };

    // UI elements
    QComboBox *m_languageCombo;
    QsciScintilla *m_codeEditor;
    QPushButton *m_compileButton;
    QCheckBox *m_darkModeCheckBox;
    CompileResultsWidget *m_resultsWidget;
    
    // Lexer
    GPULanguageLexer *m_lexer;
    
    // Languages
    QStringList m_supportedLanguages;
    
    // Theme
    bool m_isDarkMode;
    
    // Error checking
    QTimer *m_errorCheckTimer;
    QList<ErrorInfo> m_errors;
    int m_errorIndicator;
    
    // Compilers
    std::map<QString, std::unique_ptr<SimpleGPUCompiler>> m_compilers;
    
    // Compilation result
    QByteArray m_compiledBinary;
};

#endif // FLOATINGCODEEDITOR_HPP