#ifndef SIGNATUREVIEWER_H
#define SIGNATUREVIEWER_H

#include <QDialog>

class SignatureViewer : public QDialog
{
    Q_OBJECT

public:
    SignatureViewer(const QString &apkPath, QWidget *parent = nullptr);
};

#endif // SIGNATUREVIEWER_H
