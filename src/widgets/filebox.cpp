#include "widgets/filebox.h"
#include "windows/dialogs.h"
#include "base/application.h"
#include <QBoxLayout>
#include <QDir>
#include <QMimeData>

FileBox::FileBox(const QString &currentPath, const QString &defaultPath, bool isDirectory, QWidget *parent) : QWidget(parent)
{
    setAcceptDrops(true);

    input = new LineEditWithoutDrops(this);
    btnReset = new QToolButton(this);
    btnOpen = new QToolButton(this);
    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);

    this->isDirectory = isDirectory;
    setCurrentPath(currentPath);
    setDefaultPath(defaultPath);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(input);
    layout->addWidget(btnReset);
    layout->addWidget(btnOpen);
    layout->setContentsMargins(0, 0, 0, 0);

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
    input->setToolTip(path);
}

void FileBox::setDefaultPath(const QString &path)
{
    defaultPath = path;
    btnReset->setVisible(!path.isNull());
    btnReset->setToolTip(path);
}

void FileBox::setPlaceholderText(const QString &text)
{
    input->setPlaceholderText(text);
}

void FileBox::openPath()
{
    const QString oldPath = input->text();
    const QString newPath = isDirectory ? Dialogs::getOpenDirectory(oldPath, this) : Dialogs::getOpenFilename(oldPath, this);
    if (!newPath.isEmpty()) {
        setCurrentPath(newPath);
    }
}

void FileBox::checkPath()
{
    const QString path = input->text();
    const bool exists = isDirectory ? QDir(path).exists() : QFile::exists(path);
    if (exists || path.isEmpty()) {
        input->setPalette(QPalette());
    } else {
        QPalette palette = input->palette();
        palette.setColor(QPalette::Text, QColor(Qt::red));
        input->setPalette(palette);
    }
}

void FileBox::resetPath()
{
    setCurrentPath(defaultPath);
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
    }
    QWidget::changeEvent(event);
}

void FileBox::dragEnterEvent(QDragEnterEvent *event)
{
    const bool hasText = event->mimeData()->hasText();
    event->setAccepted(hasText);
    rubberBand->setVisible(hasText);
}

void FileBox::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void FileBox::dragLeaveEvent(QDragLeaveEvent *event)
{
    Q_UNUSED(event)
    rubberBand->hide();
}

void FileBox::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        event->acceptProposedAction();
        const QUrl url = mimeData->text();
        const QString path = url.toLocalFile();
        setCurrentPath(path);
    } else if (mimeData->hasText()) {
        setCurrentPath(mimeData->text());
    }
    rubberBand->hide();
}

void FileBox::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    rubberBand->setGeometry(input->geometry());
}
