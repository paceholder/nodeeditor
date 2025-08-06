#ifndef CODEEDITOR_HPP
#define CODEEDITOR_HPP

#include "data_models/Process.hpp"
#include <memory>
#include <QString>
#include <QStringList>
#include <QWidget>
#include <Qsci/qsciscintilla.h>
#include <QKeyEvent>

class QsciScintilla;
class GPULanguageLexer;
class QsciAPIs;

class ReadOnlyLinesEditor : public QsciScintilla {
public:
    ReadOnlyLinesEditor(QWidget *parent = nullptr) : QsciScintilla(parent) {
        // Mark lines 0 and 3 as read-only
        readOnlyLines = {0, 3};
        
        // Define a marker for background color
        markerDefine(QsciScintilla::Background, READONLY_MARKER);
        
        // Set grey background color for read-only lines
        setMarkerBackgroundColor(QColor(220, 220, 220), READONLY_MARKER);  // Light grey
        
        // Apply the grey background to read-only lines
        for (int line : readOnlyLines) {
            markerAdd(line, READONLY_MARKER);
        }
    }
    
protected:
    void keyPressEvent(QKeyEvent *event) override {
        // Get current line
        int line, col;
        getCursorPosition(&line, &col);
        
        // If on read-only line and not navigation key
        if (readOnlyLines.contains(line) && 
            event->key() != Qt::Key_Up && 
            event->key() != Qt::Key_Down &&
            event->key() != Qt::Key_Left && 
            event->key() != Qt::Key_Right &&
            event->key() != Qt::Key_Home &&
            event->key() != Qt::Key_End &&
            event->key() != Qt::Key_PageUp &&
            event->key() != Qt::Key_PageDown) {
            // Block the key press
            return;
        }
        
        // Otherwise, normal handling
        QsciScintilla::keyPressEvent(event);
    }
    
private:
    QSet<int> readOnlyLines;
    static const int READONLY_MARKER = 1;
};

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
    QsciScintilla *editor() { return m_editor; }
    const QsciScintilla *editor() const { return m_editor; }

    // Cursor and selection
    void setCursorPosition(int line, int column);
    void getCursorPosition(int &line, int &column) const;
    void ensureLineVisible(int line);
    void highlightLine(int line, int duration = 2000);
    void setProcessNode(Process *processNode);
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
    ReadOnlyLinesEditor *m_editor;
    GPULanguageLexer *m_lexer;

    // State
    QString m_currentLanguage;
    QStringList m_supportedLanguages;
    bool m_isDarkMode;
    Process *_processNode;
};

#endif // CODEEDITOR_HPP