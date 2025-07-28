// FloatingCodeEditor.hpp
#ifndef FLOATINGCODEEDITOR_HPP
#define FLOATINGCODEEDITOR_HPP

#include "FloatingPanelBase.hpp"
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexer.h>
#include <memory>

class QComboBox;
class QPushButton;
class GraphEditorWindow;
class QsciLexerCPP;

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
    void setupOpenCLHighlighting();
    void setupCUDAHighlighting();
    void setupMetalHighlighting();
    void setupCommonEditorFeatures();

    // UI elements
    QComboBox *m_languageCombo;
    QsciScintilla *m_codeEditor;
    QPushButton *m_compileButton;
    
    // Lexer
    QsciLexerCPP *m_lexer;
    
    // Languages
    QStringList m_supportedLanguages;
};

#endif // FLOATINGCODEEDITOR_HPP
