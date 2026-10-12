#ifndef FILEOPERATIONS_H
#define FILEOPERATIONS_H

#include <QString>

namespace SCUtils {

  QString getAppWorkDir();
  bool recursiveCopy(const QString &sourcePath, const QString &destPath);
  bool isSafeToRemoveRecursivily(const QString &directoryPath);
  bool copyAndOverwrite(const QString &source, const QString &destination, bool overwrite = true);

  //
  // some needed functionality for obtaining filepaths when loading input file from a remote application run or run shared with a user
  //   
  
  void setInputFileDir(const QString &path);
  QString getInputFileDir();
  QString getFilePath(const QString filename);
}


#endif // FILEOPERATIONS_H
