#ifndef FLOATINGCODEEDITOR_HPP
#define FLOATINGCODEEDITOR_HPP

#include "FloatingPanelBase.hpp"
#include <memory>
#include <map>

class QComboBox;
class QCheckBox;
class QPushButton;
class QsciScintilla;
class CompileResultsWidget;
class SimpleGPUCompiler;
class GraphEditorWindow;
class GPULanguageLexer;

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

private:
    void setupUI();
    void connectSignals();
    void updateHighlighter();
    void applyDarkTheme();
    void applyLightTheme();
    void setupCommonEditorFeatures();
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

    // Compilers
    std::map<QString, std::unique_ptr<SimpleGPUCompiler>> m_compilers;
};

#endif // FLOATINGCODEEDITOR_HPP