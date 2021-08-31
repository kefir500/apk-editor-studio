#include "base/application.h"

int main(int argc, char *argv[])
{
    Application application(argc, argv);
    if (application.isSecondary()) {
        QStringList args = application.arguments();
        args.removeFirst();
        application.sendMessage(args.join('\n').toUtf8());
        return 0;
    }
    return application.exec();
}
