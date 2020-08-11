#include "base/jarprocess.h"
#include "base/application.h"
#include "tools/java.h"

void JarProcess::run(const QString &jar, const QStringList &jarArguments)
{
    QStringList arguments;
    const int minHeapSize = app->settings->getJavaMinHeapSize();
    const int maxHeapSize = app->settings->getJavaMaxHeapSize();
    if (app->settings->getJavaMinHeapSize()) {
        arguments << QString("-Xms%1m").arg(minHeapSize);
    }
    if (app->settings->getJavaMaxHeapSize()) {
        arguments << QString("-Xmx%1m").arg(maxHeapSize);
    }
    arguments << "-jar" << jar << jarArguments;
    Process::run(Java::getBinaryPath("java"), arguments);
}
