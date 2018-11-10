#include "base/application.h"

int main(int argc, char *argv[])
{
    Application application(argc, argv);
    if (application.isRunning()) {
        QStringList args = application.arguments();
        args.removeFirst();
        if (application.sendMessage(args.join('\n'))) {
            return 0;
        }
    }
    return application.exec();
}
