#include "widgets/filebox.h"
#include "windows/dialogs.h"
#include "base/application.h"
#include <QBoxLayout>
#include <QDir>
#include <QMimeData>

FileBox::FileBox(const QString &currentPath, const QString &defaultPath, bool isDirectory, QWidget *parent) : QWidget(parent)
{
    setAcceptDrops(true);
    setAutoFillBackground(true);

    input = new QLineEdit(this);
    btnReset = new QToolButton(this);
    btnOpen = new QToolButton(this);

    this->isDirectory = isDirectory;
    setDefaultPath(defaultPath);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(input);
    layout->addWidget(btnReset);
    layout->addWidget(btnOpen);
    layout->setContentsMargins(0, 0, 0, 0);

    input->setText(currentPath);

    connect(btnOpen, &QToolButton::clicked, this, &FileBox::openPath);
    connect(btnReset, &QToolButton::clicked, this, &FileBox::resetPath);
    connect(input, &QLineEdit::textChanged, this, &FileBox::checkPath);
    connect(input, &QLineEdit::textChanged, this, &FileBox::currentPathChanged);

    retranslate();
}

QString FileBox::getCurrentPath() const
{
    return input->text();
}

void FileBox::setCurrentPath(const QString &path)
{
    input->setText(path);
}

void FileBox::setDefaultPath(const QString &path)
{
    defaultPath = path;
    btnReset->setVisible(!path.isEmpty());
}

void FileBox::openPath()
{
    const QString oldPath = input->text();
    const QString newPath = isDirectory ? Dialogs::getOpenDirectory(this, oldPath) : Dialogs::getOpenFilename(this, oldPath);
    if (!newPath.isEmpty()) {
        input->setText(newPath);
    }
}

void FileBox::checkPath()
{
    const QString path = input->text();
    const bool exists = isDirectory ? QDir(path).exists() : QFile::exists(path);
    if (exists) {
        input->setPalette(QPalette());
    } else {
        QPalette palette = input->palette();
        palette.setColor(QPalette::Text, QColor(Qt::red));
        input->setPalette(palette);
    }
}

void FileBox::resetPath()
{
    input->setText(defaultPath);
}

void FileBox::retranslate()
{
    btnOpen->setText("...");
    btnReset->setText(tr("Reset"));
}

void FileBox::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    } else {
        QWidget::changeEvent(event);
    }
}

void FileBox::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
        setBackgroundRole(QPalette::Light);
    }
}

void FileBox::dragLeaveEvent(QDragLeaveEvent *event)
{
    Q_UNUSED(event);
    setBackgroundRole(QPalette::Window);
}

void FileBox::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasText()) {
        const QUrl url = mimeData->text();
        const QString path = url.toLocalFile();
        setCurrentPath(path);
        event->acceptProposedAction();
    }
    setBackgroundRole(QPalette::Window);
}
