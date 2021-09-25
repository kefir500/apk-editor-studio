#include "widgets/packagelist.h"
#include "widgets/packagelistitemdelegate.h"
#include "apk/package.h"
#include "base/utils.h"

PackageList::PackageList(QWidget *parent) : QComboBox(parent)
{
#if defined(Q_OS_WIN)
    setMinimumHeight(Utils::scale(40));
    setIconSize(Utils::scale(32, 32));
#elif defined(Q_OS_MACOS)
    setIconSize(Utils::scale(16, 16));
#else
    setMinimumHeight(Utils::scale(46));
    setIconSize(Utils::scale(32, 32));
#endif

    setItemDelegate(new PackageListItemDelegate(this));

    connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        auto package = static_cast<Package *>(model()->index(index, 0).internalPointer());
        emit currentPackageChanged(package);
    });
}

bool PackageList::setCurrentPackage(Package *package)
{
    const int rowCount = model()->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        if (package == model()->index(i, 0).internalPointer()) {
            setCurrentIndex(i);
            return true;
        }
    }
    return false;
}
