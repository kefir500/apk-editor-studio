#ifndef FILEFORMATLIST_H
#define FILEFORMATLIST_H

#include <QCoreApplication>
#include "base/fileformat.h"

class FileFormatList
{
    Q_DECLARE_TR_FUNCTIONS(FileFormatList)

public:
    static FileFormatList forApk();
    static FileFormatList forKeystore();
    static FileFormatList forReadableImages();
    static FileFormatList forWritableImages();

    FileFormatList() = default;
    FileFormatList(const FileFormat &format);

    void add(const FileFormat &format);
    bool isEmpty() const;
    QStringList getExtensions() const;
    QString getFilterString() const;

private:
    QList<FileFormat> formats;
};

#endif // FILEFORMATLIST_H
