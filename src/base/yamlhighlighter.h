#ifndef YAMLHIGHLIGHTER_H
#define YAMLHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class YamlHighlighter : public QSyntaxHighlighter {

public:
    YamlHighlighter(QTextDocument *parent);

protected:
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

private:
    void highlightRegex(const QString &text, const QTextCharFormat &format, const QRegularExpression &regex);
    QTextCharFormat formatDefault;
    QTextCharFormat formatTag;
    QTextCharFormat formatKey;
    QTextCharFormat formatValue;
    QTextCharFormat formatValueNumber;
    QTextCharFormat formatComment;
};

#endif // YAMLHIGHLIGHTER_H
