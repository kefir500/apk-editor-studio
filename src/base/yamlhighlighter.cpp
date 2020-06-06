#include "base/yamlhighlighter.h"
#include "base/application.h"
#include <QRegularExpression>


void YamlHighlighter::highlightBlock(const QString &text)
{
    setFormat(0, text.length(), app->theme()->text(Theme::Text::YamlDefault));
    highlightRegex(text, app->theme()->text(Theme::Text::YamlValueDefault), QRegularExpression("(?<=:|-) *(.+)"));
    highlightRegex(text, app->theme()->text(Theme::Text::YamlValueNumber), QRegularExpression("(?<=:|-) *([\\d.]+) *$"));
    highlightRegex(text, app->theme()->text(Theme::Text::YamlKey), QRegularExpression(" *(.*):"));
}

void YamlHighlighter::highlightRegex(const QString &text, const QTextCharFormat &format, const QRegularExpression &regex)
{
    QRegularExpressionMatchIterator it = regex.globalMatch(text);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        setFormat(match.capturedStart(), match.capturedLength(), format);
    }
}
