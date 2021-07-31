#ifndef XMLHIGHLIGHTER_H
#define XMLHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class XmlHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    XmlHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) {}

protected:
    void highlightBlock(const QString &text) override;

private:
    void highlightRegex(const QString &text, const QTextCharFormat &format, const QRegularExpression &regex);
};

#endif // XMLHIGHLIGHTER_H
