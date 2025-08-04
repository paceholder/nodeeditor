#ifndef FLOATINGCODEEDITOR_HPP
#define FLOATINGCODEEDITOR_HPP

#include "FloatingPanelBase.hpp"
#include <QTimer>
#include <QSet>
#include <memory>
#include <map>
#include <Qsci/qscilexercustom.h>

class QComboBox;
class QCheckBox;
class QPushButton;
class QsciScintilla;
class CompileResultsWidget;
class SimpleGPUCompiler;
class GraphEditorWindow;

// Custom lexer for GPU languages
class GPULanguageLexer : public QsciLexerCustom
{
    Q_OBJECT

public:
    enum {
        Default = 0,
        Comment = 1,
        CommentLine = 2,
        Number = 3,
        Keyword = 4,
        LanguageKeyword = 5,
        String = 6,
        Operator = 7,
        Identifier = 8,
        Preprocessor = 9
    };

    explicit GPULanguageLexer(QObject *parent = nullptr);

    const char *language() const override;
    QString description(int style) const override;
    QColor defaultColor(int style) const override;
    QColor defaultPaper(int style) const override;
    QFont defaultFont(int style) const override;
    void styleText(int start, int end) override;

    void setLanguageMode(const QString &language);
    void setDarkMode(bool dark);
    
    // Get collected identifiers
    QSet<QString> getIdentifiers() const { return m_identifiers; }
    void clearIdentifiers() { m_identifiers.clear(); }
    
    // Public helper methods for syntax checking
    bool isKeyword(const QString &word) const;
    bool isLanguageKeyword(const QString &word) const;
    bool isBuiltinFunction(const QString &word) const;
    bool isBuiltinType(const QString &word) const;

private:
    bool isNumber(const QString &text) const;

    QString m_language;
    bool m_isDarkMode;
    QSet<QString> m_keywords;
    QSet<QString> m_languageKeywords;
    QSet<QString> m_builtinFunctions;
    QSet<QString> m_types;
    QSet<QString> m_identifiers; // Track identifiers found during lexing
};

class FloatingCodeEditor : public FloatingPanelBase
{
    Q_OBJECT

public:
    explicit FloatingCodeEditor(GraphEditorWindow *parent = nullptr);
    ~FloatingCodeEditor() override;

    // Code editor API
    void setCode(const QString &code);
    QString getCode() const;
    
    void setLanguage(const QString &language);
    QString getCurrentLanguage() const;
    
    void setReadOnly(bool readOnly);
    
    // Get compiled binary if available
    QByteArray getCompiledBinary() const { return m_compiledBinary; }

signals:
    void codeChanged();
    void languageChanged(const QString &language);
    void compileRequested(const QString &code, const QString &language);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onLanguageChanged(int index);
    void onCompileClicked();
    void onCodeChanged();
    void onThemeToggled(bool checked);
    void onResultsCloseRequested();
    void onMessageClicked(int line, int column);
    void onTextChanged();
    void performErrorCheck();

private:
    void setupUI();
    void connectSignals();
    void updateHighlighter();
    void applyDarkTheme();
    void applyLightTheme();
    void setupCommonEditorFeatures();
    void setupErrorIndicator();
    void checkForErrors();
    void updateLexerColors();
    void forceRefreshLexer();
    
    // Compiler management
    void initializeCompilers();
    void performCompilation();
    void showCompileResults(bool show);
    void updateCompilerAvailability();

    // UI elements
    QComboBox *m_languageCombo;
    QsciScintilla *m_codeEditor;
    QPushButton *m_compileButton;
    QCheckBox *m_darkModeCheckBox;
    CompileResultsWidget *m_resultsWidget;
    GPULanguageLexer *m_lexer;

    // State
    QStringList m_supportedLanguages;
    bool m_isDarkMode;
    QByteArray m_compiledBinary;
    
    // Error checking
    QTimer *m_errorCheckTimer;
    int m_errorIndicator;
    struct ErrorInfo {
        int line;
        int indexStart;
        int indexEnd;
        QString message;
    };
    QList<ErrorInfo> m_errors;

    // Compilers
    std::map<QString, std::unique_ptr<SimpleGPUCompiler>> m_compilers;
};

#endif // FLOATINGCODEEDITOR_HPP