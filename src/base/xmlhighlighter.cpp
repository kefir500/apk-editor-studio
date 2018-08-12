#include "base/xmlhighlighter.h"
#include "base/debug.h"
#include <QRegularExpression>

XmlHighlighter::XmlHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    formatDefault.setForeground(Qt::black);
    formatDefault.setFontWeight(QFont::Bold);
    formatTag.setForeground(Qt::blue);
    formatElement.setForeground(Qt::blue);
    formatAttribute.setForeground(Qt::red);
    formatValue.setForeground(Qt::darkMagenta);
    formatComment.setForeground(Qt::gray);
}

void XmlHighlighter::highlightBlock(const QString &text)
{
    setFormat(0, text.length(), formatDefault);
    highlightRegex(text, formatElement, QRegularExpression("<\\/?\\?*([\\w\\-.?]+)"));
    highlightRegex(text, formatTag, QRegularExpression("[<?>\\/]"));
    highlightRegex(text, formatAttribute, QRegularExpression("[\\w:]+(?==)"));
    highlightRegex(text, formatValue, QRegularExpression("(?<==)\"[^\"]+\""));
    highlightRegex(text, formatComment, QRegularExpression("<!--.*-->"));
}

void XmlHighlighter::highlightRegex(const QString &text, const QTextCharFormat &format, const QRegularExpression &regex)
{
    QRegularExpressionMatchIterator it = regex.globalMatch(text);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        setFormat(match.capturedStart(), match.capturedLength(), format);
    }
}
