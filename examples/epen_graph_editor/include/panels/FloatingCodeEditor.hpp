#ifndef FLOATINGCODEEDITOR_HPP
#define FLOATINGCODEEDITOR_HPP

#include "FloatingPanelBase.hpp"
#include <memory>
#include <map>

class QComboBox;
class QCheckBox;
class QPushButton;
class CodeEditor;
class CompileResultsWidget;
class SimpleGPUCompiler;
class GraphEditorWindow;

class FloatingCodeEditor : public FloatingPanelBase
{
    Q_OBJECT

public:
    explicit FloatingCodeEditor(GraphEditorWindow *parent = nullptr);
    ~FloatingCodeEditor() override;

    // Code editor API (delegates to CodeEditor)
    void setCode(const QString &code);
    QString getCode() const;
    
    void setLanguage(const QString &language);
    QString getCurrentLanguage() const;
    
    void setReadOnly(bool readOnly);
    
    // Get compiled binary if available
    QByteArray getCompiledBinary() const { return m_compiledBinary; }
    
    // Maximize/restore functionality
    bool isMaximized() const { return m_isMaximized; }
    void setMaximized(bool maximized);

signals:
    void codeChanged();
    void languageChanged(const QString &language);
    void compileRequested(const QString &code, const QString &language);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void moveEvent(QMoveEvent *event) override;
    void dockChanged(bool isFloat) override;

private slots:
    void onLanguageChanged(int index);
    void onCompileClicked();
    void onCodeChanged();
    void onThemeToggled(bool checked);
    void onResultsCloseRequested();
    void onMessageClicked(int line, int column);
    void onMaximizeClicked();
    void onEditorLanguageChanged(const QString &language);

private:
    void setupUI() override;
    void connectSignals() override;
    
    // Compiler management
    void initializeCompilers();
    void performCompilation();
    void showCompileResults(bool show);
    void updateCompilerAvailability();
    
    // Maximize/restore functionality
    void toggleMaximize();
    void restoreFromMaximized();
    void maximizePanel();

    // UI elements
    QComboBox *m_languageCombo;
    CodeEditor *m_codeEditor;  // Changed from QsciScintilla
    QPushButton *m_compileButton;
    QCheckBox *m_darkModeCheckBox;
    CompileResultsWidget *m_resultsWidget;
    QPushButton *m_maximizeButton;

    // State
    QByteArray m_compiledBinary;
    
    // Maximize state
    bool m_isMaximized;
    QRect m_preMaximizeGeometry;
    DockPosition m_preMaximizeDockPosition;
    int m_preMaximizeDockedHeight;
    int m_preMaximizeDockedWidth;
    int m_preMaximizeFloatHeight;

    // Compilers
    std::map<QString, std::unique_ptr<SimpleGPUCompiler>> m_compilers;
};

#endif // FLOATINGCODEEDITOR_HPP