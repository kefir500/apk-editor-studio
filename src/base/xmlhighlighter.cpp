#include "base/xmlhighlighter.h"
#include "base/application.h"
#include <QRegularExpression>

void XmlHighlighter::highlightBlock(const QString &text)
{
    setFormat(0, text.length(), app->theme()->text(Theme::Text::XmlDefault));
    highlightRegex(text, app->theme()->text(Theme::Text::XmlTagName), QRegularExpression("<\\/?\\?*([\\w\\-.?]+)"));
    highlightRegex(text, app->theme()->text(Theme::Text::XmlTagBracket), QRegularExpression("[<?>\\/]"));
    highlightRegex(text, app->theme()->text(Theme::Text::XmlAttribute), QRegularExpression("[\\w:]+(?==)"));
    highlightRegex(text, app->theme()->text(Theme::Text::XmlValue), QRegularExpression("(?<==)\"[^\"]+\""));
    highlightRegex(text, app->theme()->text(Theme::Text::XmlComment), QRegularExpression("<!--.*-->"));
}

void XmlHighlighter::highlightRegex(const QString &text, const QTextCharFormat &format, const QRegularExpression &regex)
{
    QRegularExpressionMatchIterator it = regex.globalMatch(text);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        setFormat(match.capturedStart(), match.capturedLength(), format);
    }
}
