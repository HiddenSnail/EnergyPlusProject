#include "utils/no_class_funcs.h"

bool deleteDirRecursive(QDir dir)
{
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList entryList = dir.entryInfoList();
    foreach (QFileInfo entry, entryList)
    {
        if (entry.isFile())
        {
            entry.dir().remove(entry.fileName());
        }
        else
        {
            deleteDirRecursive(entry.absoluteFilePath());
        }
    }
    return dir.rmdir(dir.absolutePath());
}
