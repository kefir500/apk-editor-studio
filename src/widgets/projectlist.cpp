#include "widgets/projectlist.h"
#include "widgets/projectlistitemdelegate.h"
#include "base/utils.h"

ProjectList::ProjectList(QWidget *parent) : QComboBox(parent)
{
#if defined(Q_OS_WIN)
    setMinimumHeight(Utils::scale(40));
    setIconSize(Utils::scale(32, 32));
#elif defined(Q_OS_OSX)
    setIconSize(Utils::scale(16, 16));
#else
    setMinimumHeight(Utils::scale(46));
    setIconSize(Utils::scale(32, 32));
#endif

    setItemDelegate(new ProjectListItemDelegate(this));
}

bool ProjectList::setCurrentProject(Project *project)
{
    const int rowCount = model()->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        if (project == model()->index(i, 0).internalPointer()) {
            setCurrentIndex(i);
            return true;
        }
    }
    return false;
}
