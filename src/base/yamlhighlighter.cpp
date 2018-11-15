#include "base/yamlhighlighter.h"
#include <QRegularExpression>

YamlHighlighter::YamlHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    formatDefault.setForeground(Qt::black);
    formatTag.setForeground(Qt::blue);
    formatKey.setForeground(Qt::darkBlue);
    formatKey.setFontWeight(QFont::Bold);
    formatValue.setForeground(Qt::darkGreen);
    formatValue.setFontWeight(QFont::Bold);
    formatValueNumber.setForeground(QColor(210, 120, 20));
    formatValueNumber.setFontWeight(QFont::Bold);
    formatComment.setForeground(Qt::gray);
}

void YamlHighlighter::highlightBlock(const QString &text)
{
    setFormat(0, text.length(), formatDefault);
    highlightRegex(text, formatTag, QRegularExpression("!.+$"));
    highlightRegex(text, formatValue, QRegularExpression("(?<=:|-) *(.+)"));
    highlightRegex(text, formatValueNumber, QRegularExpression("(?<=:|-) *([\\d.]+) *$"));
    highlightRegex(text, formatKey, QRegularExpression(" *(.*):"));
}

void YamlHighlighter::highlightRegex(const QString &text, const QTextCharFormat &format, const QRegularExpression &regex)
{
    QRegularExpressionMatchIterator it = regex.globalMatch(text);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        setFormat(match.capturedStart(), match.capturedLength(), format);
    }
}
