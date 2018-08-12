#include "windows/aboutdialog.h"
#include "tools/adb.h"
#include "tools/apktool.h"
#include "tools/java.h"
#include <QFormLayout>
#include <QTabWidget>
#include <QLabel>
#include <QPlainTextEdit>
#include <QTextBrowser>
#include <QDialogButtonBox>
#include <QtConcurrent/QtConcurrent>
#include "base/application.h"

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("About"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    resize(app->dpiAwareSize(640, 400));

    QTabWidget *tabs = new QTabWidget(this);
    tabs->addTab(createAboutTab(), tr("About"));
    tabs->addTab(createAuthorsTab(), tr("Authors"));
    tabs->addTab(createVersionsTab(), tr("Version History"));
    tabs->addTab(createLibrariesTab(), tr("Technologies"));
    tabs->addTab(createLicenseTab(), tr("License"));

    QLabel *icon = new QLabel(this);
    icon->setContentsMargins(0, 0, 10, 4);
    icon->setPixmap(app->loadIcon("application.png").pixmap(48, 48));
    icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QLabel *title = new QLabel(app->getTitleAndVersion(), this);
    QFont titleFont = title->font();
    titleFont.setPointSize(11);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok);

    QHBoxLayout *layoutTitle = new QHBoxLayout;
    QVBoxLayout *layoutMain = new QVBoxLayout(this);
    layoutTitle->addWidget(icon);
    layoutTitle->addWidget(title);
    layoutMain->addLayout(layoutTitle);
    layoutMain->addWidget(tabs);
    layoutMain->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, &AboutDialog::accept);
}

GradientWidget *AboutDialog::createAboutTab()
{
    GradientWidget *tab = new GradientWidget(this);

    QLabel *icon = new QLabel(this);
    icon->setMargin(16);
    icon->setPixmap(app->loadIcon("application.png").pixmap(128, 128));
    icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QLabel *text = new QLabel(this);
    text->setWordWrap(true);
    text->setOpenExternalLinks(true);
    const QString tableRow("<tr><td>%1</td><td><a href=\"%2\">%2</a></td></tr>");
    text->setText(
        QString("<h4>%1</h4>").arg(app->getTitleAndVersion()) +
        QString("<p>%1</p>").arg(tr("Author: %1").arg("Alexander Gorishnyak")) +
        QString("<p><table>") +
        QString(tableRow).arg(tr("Homepage:"), app->getWebPage()) +
        QString(tableRow).arg(tr("Bug Tracker:"), app->getIssuesPage()) +
        QString(tableRow).arg(tr("Translation:"), app->getTranslatePage()) +
        QString("</table></p>") +
        QString("<p>%1 - %2</p>").arg(QString(__DATE__).toUpper(), __TIME__)
    );

    QHBoxLayout *layout = new QHBoxLayout(tab);
    layout->addWidget(icon);
    layout->addWidget(text);

    return tab;
}

QWidget *AboutDialog::createAuthorsTab()
{
    QString content = "";
    const QString br("<br />");
    QFile file(app->getSharedPath("docs/authors.txt"));
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        while (!stream.atEnd()) {
            QString line = stream.readLine();

            // Parse Markdown headers and lists:
            if (line.startsWith("# ")) {
                line = QString("<h3>%1</h3>").arg(line.mid(2));
                if (content.endsWith(br)) {
                    content.chop(br.length());
                }
            } else if (line.startsWith("- ")) {
                line = line.mid(2) + br;
            }

            // Parse Markdown links:
            QRegularExpression regex("\\[(.+?)\\]\\((.+?)\\)");
            forever {
                QRegularExpressionMatch match = regex.match(line);
                if (match.hasMatch()) {
                    const QString title = match.captured(1);
                    const QString href = match.captured(2);
                    const QString link = QString("<a href=\"%1\">%2</a>").arg(href, title);
                    line.replace(match.capturedStart(0), match.capturedLength(0), link);
                } else {
                    break;
                }
            }
            content.append(line);
        }
    }

    QTextBrowser *tab = new QTextBrowser(this);
    tab->setReadOnly(true);
    tab->setOpenExternalLinks(true);
    tab->setText(content);
    return tab;
}

QWidget *AboutDialog::createVersionsTab()
{
    QPlainTextEdit *tab = new QPlainTextEdit(this);
    tab->setReadOnly(true);

    QFile file(app->getSharedPath("docs/versions.txt"));
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        while (!stream.atEnd()) {
            QString line = stream.readLine();

            // Parse Markdown headers and lists:
            if (line.startsWith("## ")) {
                line = QString("<h3>%1<br></h3>").arg(line.mid(3));
            } else if (line.startsWith("# ")) {
                line = QString("<h3>%1</h3>").arg(line.mid(2));
            } else if (line.startsWith("- ")) {
                line = line.mid(2);
            }

            // Parse Markdown links:
            QRegularExpression regex("\\[(.+?)\\]\\((.+?)\\)");
            forever {
                QRegularExpressionMatch match = regex.match(line);
                if (match.hasMatch()) {
                    const QString title = match.captured(1);
                    const QString href = match.captured(2);
                    const QString link = QString("<a href=\"%1\">%2</a>").arg(href, title);
                    line.replace(match.capturedStart(0), match.capturedLength(0), link);
                } else {
                    break;
                }
            }
            tab->appendHtml(line);
        }
    }

    return tab;
}

QWidget *AboutDialog::createLibrariesTab()
{
    GradientWidget *tab = new GradientWidget(this);

    const QChar mdash(0x2014);
    const QChar ellipsis(0x2026);

    QLabel *labelQt = new QLabel(ellipsis, this);
    QLabel *labelJava = new QLabel(ellipsis, this);
    QLabel *labelApktool = new QLabel(ellipsis, this);
    QLabel *labelAdb = new QLabel(ellipsis, this);

    QFormLayout *layout = new QFormLayout(tab);
    layout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    layout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    layout->setFormAlignment(Qt::AlignCenter);
    layout->setLabelAlignment(Qt::AlignRight);
    layout->addRow(new QLabel(tr("Qt"), this), labelQt);
    layout->addRow(new QLabel(tr("Java"), this), labelJava);
    layout->addRow(new QLabel(tr("Apktool"), this), labelApktool);
    layout->addRow(new QLabel(tr("ADB"), this), labelAdb);

    // Set versions:

    labelQt->setText(QT_VERSION_STR);

    QtConcurrent::run([=] {
        Java java;
        const QString versionJava = java.version();
        labelJava->setText(!versionJava.isEmpty() ? versionJava : mdash);

        Apktool apktool(app->getToolPath("apktool.jar"));
        const QString versionApktool = apktool.version();
        labelApktool->setText(!versionApktool.isEmpty() ? versionApktool : mdash);

        Adb adb(app->getToolPath("adb"));
        const QString versionAdb = adb.version();
        labelAdb->setText(!versionAdb.isEmpty() ? versionAdb : mdash);
    });

    return tab;
}

QWidget *AboutDialog::createLicenseTab()
{
    QTextBrowser *tab = new QTextBrowser(this);
    tab->setReadOnly(true);
    tab->setOpenExternalLinks(true);

    QFile file(app->getSharedPath("docs/license.html"));
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        tab->setText(stream.readAll());
    }

    return tab;
}
