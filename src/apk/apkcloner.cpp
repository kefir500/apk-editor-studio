#include "apk/apkcloner.h"
#include <QDirIterator>
#include <QtConcurrent/QtConcurrent>

ApkCloner::ApkCloner(const QString &contentsPath, const QString &originalPackageName, const QString &newPackageName, QObject *parent)
    : QObject(parent)
    , contentsPath(contentsPath)
    , originalPackageName(originalPackageName)
    , newPackageName(newPackageName)
{
    newPackagePath = newPackageName;
    newPackagePath.replace('.', '/');

    originalPackagePath = originalPackageName;
    originalPackagePath.replace('.', '/');
}

void ApkCloner::start()
{
    QtConcurrent::run([this]() {
        emit started();

        // Update references in resources:

        const auto resourcesPath = contentsPath + "/res/";
        QDirIterator files(resourcesPath, QDir::Files, QDirIterator::Subdirectories);
        while (files.hasNext()) {
            const QString path(files.next());
            emit progressed(tr("Updating resource references..."), path.mid(contentsPath.size() + 1));

            QFile file(path);
            if (file.open(QFile::ReadWrite)) {
                const QString data(file.readAll());
                QString newData(data);
                newData.replace(originalPackageName, newPackageName);
                if (newData != data) {
                    file.resize(0);
                    file.write(newData.toUtf8());
                }
                file.close();
            }
        }

        const auto smaliDirs = QDir(contentsPath).entryList({"smali*"}, QDir::Dirs);
        for (const auto &smaliDir : smaliDirs) {

            const auto smaliPath = QString("%1/%2/").arg(contentsPath, smaliDir);

            // Update references in smali:

            QDirIterator files(smaliPath, QDir::Files, QDirIterator::Subdirectories);
            while (files.hasNext()) {
                const QString path(files.next());
                //: "Smali" is the name of the tool/format, don't translate it.
                emit progressed(tr("Updating Smali references..."), path.mid(contentsPath.size() + 1));

                QFile file(path);
                if (file.open(QFile::ReadWrite)) {
                    const QString data(file.readAll());
                    QString newData(data);
                    newData.replace('L' + originalPackagePath, 'L' + newPackagePath);
                    newData.replace(originalPackageName, newPackageName);
                    if (newData != data) {
                        file.resize(0);
                        file.write(newData.toUtf8());
                    }
                    file.close();
                }
            }

            // Update directory structure:

            emit progressed(tr("Updating directory structure..."), smaliDir);

            const auto fullPackagePath = smaliPath + newPackagePath;
            const auto fullOriginalPackagePath = smaliPath + originalPackagePath;
            if (!QDir().exists(fullOriginalPackagePath)) {
                continue;
            }
            QDir().mkpath(QFileInfo(fullPackagePath).path());
            if (!QDir().rename(fullOriginalPackagePath, fullPackagePath)) {
                emit finished(false);
                return;
            }
        }
\
        emit finished(true);
    });
}
