#ifndef FLOATINGCODEEDITOR_HPP
#define FLOATINGCODEEDITOR_HPP

#include "FloatingPanelBase.hpp"
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexercustom.h>
#include <memory>

class QComboBox;
class QPushButton;
class QCheckBox;
class GraphEditorWindow;

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

private:
    bool isKeyword(const QString &word) const;
    bool isLanguageKeyword(const QString &word) const;
    bool isNumber(const QString &text) const;
    
    QString m_language;
    bool m_isDarkMode;
    QStringList m_keywords;
    QStringList m_languageKeywords;
    QStringList m_types;
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

private:
    void updateHighlighter();
    void setupCommonEditorFeatures();
    void applyDarkTheme();
    void applyLightTheme();

    // UI elements
    QComboBox *m_languageCombo;
    QsciScintilla *m_codeEditor;
    QPushButton *m_compileButton;
    QCheckBox *m_darkModeCheckBox;
    
    // Lexer
    GPULanguageLexer *m_lexer;
    
    // Languages
    QStringList m_supportedLanguages;
    
    // Theme
    bool m_isDarkMode;
};

#endif // FLOATINGCODEEDITOR_HPP