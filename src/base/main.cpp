#include "base/application.h"

int main(int argc, char *argv[])
{
    Application application(argc, argv);
    return application.exec();
}
