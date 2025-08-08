#ifndef GPULANGUAGELEXER_HPP
#define GPULANGUAGELEXER_HPP

#include <QSet>
#include <QString>
#include <Qsci/qscilexercustom.h>

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

#endif // GPULANGUAGELEXER_HPP