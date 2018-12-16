#ifndef XMLHIGHLIGHTER_H
#define XMLHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class XmlHighlighter : public QSyntaxHighlighter
{
public:
    XmlHighlighter(QTextDocument *parent);

protected:
    void highlightBlock(const QString &text) override;

private:
    void highlightRegex(const QString &text, const QTextCharFormat &format, const QRegularExpression &regex);
    QString selectionText;
    QTextCharFormat formatDefault;
    QTextCharFormat formatTag;
    QTextCharFormat formatElement;
    QTextCharFormat formatAttribute;
    QTextCharFormat formatValue;
    QTextCharFormat formatComment;
};

#endif // XMLHIGHLIGHTER_H
