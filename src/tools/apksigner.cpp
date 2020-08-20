#include "tools/apksigner.h"
#include "base/jarprocess.h"
#include "base/application.h"
#include "base/utils.h"
#include <QRegularExpression>

void Apksigner::Sign::run()
{
    emit started();

    QStringList arguments;
    arguments << "sign";
    arguments << "--ks" << keystorePath;
    arguments << "--ks-key-alias" << keyAlias;
    arguments << "--ks-pass" << QString("pass:%1").arg(keystorePassword);
    arguments << "--key-pass" << QString("pass:%1").arg(keyPassword);
    arguments << target;

    auto process = new JarProcess(this);
    connect(process, &JarProcess::finished, this, [=](bool success, const QString &output) {
        resultOutput = output;
        emit finished(success);
        process->deleteLater();
    });
    process->run(getPath(), arguments);
}

const QString &Apksigner::Sign::output() const
{
    return resultOutput;
}

void Apksigner::Verify::run()
{
    emit started();

    QStringList arguments;
    arguments << "verify";
    arguments << "--print-certs";
    arguments << "--verbose";
    arguments << apkPath;

    auto process = new JarProcess(this);
    connect(process, &JarProcess::finished, this, [=](bool success, const QString &output) {
        resultSignersInfo.clear();
        QRegularExpression signerCountRegex("^Number of signers: (\\d+)$");
        signerCountRegex.setPatternOptions(QRegularExpression::MultilineOption);
        const int signerCount = signerCountRegex.match(output).captured(1).toInt();
        for (int i = 1; i <= signerCount; ++i) {
            QString signerInfo;
            QRegularExpression signerInfoRegex(QString("^Signer #%1 .+$").arg(i));
            signerInfoRegex.setPatternOptions(QRegularExpression::MultilineOption);
            auto it = signerInfoRegex.globalMatch(output);
            while (it.hasNext()) {
                auto match = it.next();
                signerInfo.append(match.captured(0) + '\n');
            }
            resultSignersInfo.append(signerInfo);
        }
        QRegularExpression v1SchemeRegex("^Verified using v1 scheme \\(JAR signing\\): true$");
        v1SchemeRegex.setPatternOptions(QRegularExpression::MultilineOption);
        QRegularExpression v2SchemeRegex("^Verified using v2 scheme \\(APK Signature Scheme v2\\): true$");
        v2SchemeRegex.setPatternOptions(QRegularExpression::MultilineOption);
        QRegularExpression v3SchemeRegex("^Verified using v3 scheme \\(APK Signature Scheme v3\\): true$");
        v3SchemeRegex.setPatternOptions(QRegularExpression::MultilineOption);
        resultV1Scheme = output.contains(v1SchemeRegex);
        resultV2Scheme = output.contains(v2SchemeRegex);
        resultV3Scheme = output.contains(v3SchemeRegex);
        emit finished(success);
        process->deleteLater();
    });
    process->run(getPath(), arguments);
}

bool Apksigner::Verify::hasV1Scheme() const
{
    return resultV1Scheme;
}

bool Apksigner::Verify::hasV2Scheme() const
{
    return resultV2Scheme;
}

bool Apksigner::Verify::hasV3Scheme() const
{
    return resultV3Scheme;
}

const QStringList &Apksigner::Verify::signersInfo() const
{
    return resultSignersInfo;
}

void Apksigner::Version::run()
{
    emit started();
    auto process = new JarProcess(this);
    connect(process, &JarProcess::finished, this, [=](bool success, const QString &output) {
        if (success) {
            resultVersion = output;
        }
        emit finished(success);
        process->deleteLater();
    });
    process->run(getPath(), {"--version"});
}

const QString &Apksigner::Version::version() const
{
    return resultVersion;
}

QString Apksigner::getPath()
{
    const QString path = Utils::toAbsolutePath(app->settings->getApksignerPath());
    return !path.isEmpty() ? path : getDefaultPath();
}

QString Apksigner::getDefaultPath()
{
    return Utils::getSharedPath("tools/apksigner.jar");
}
