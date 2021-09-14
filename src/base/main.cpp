#include "base/application.h"

int main(int argc, char *argv[])
{
    Application application(argc, argv);
    if (application.isSecondary()) {
        application.sendMessage(application.arguments().join('\n').toUtf8());
        return 0;
    }
    return application.exec();
}
