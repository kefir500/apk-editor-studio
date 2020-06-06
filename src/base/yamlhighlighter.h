#ifndef YAMLHIGHLIGHTER_H
#define YAMLHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class YamlHighlighter : public QSyntaxHighlighter
{
public:
    YamlHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) {}

protected:
    void highlightBlock(const QString &text) override;

private:
    void highlightRegex(const QString &text, const QTextCharFormat &format, const QRegularExpression &regex);
};

#endif // YAMLHIGHLIGHTER_H
