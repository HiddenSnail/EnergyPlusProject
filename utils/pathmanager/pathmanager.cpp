#include "pathmanager.h"

PathManager* PathManager::instance()
{
    static PathManager pathManager;
    return &pathManager;
}

QString PathManager::getPath(QString pathKey)
{
    return _pathMap[pathKey];
}
