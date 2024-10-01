#ifndef FILEOPERATIONS_H
#define FILEOPERATIONS_H

#include <QString>

namespace SCUtils {

  bool recursiveCopy(const QString &sourcePath, const QString &destPath);
  bool isSafeToRemoveRecursivily(const QString &directoryPath);
  
}


#endif // FILEOPERATIONS_H
