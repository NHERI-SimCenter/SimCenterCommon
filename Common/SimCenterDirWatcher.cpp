/* *****************************************************************************
Copyright (c) 2016-2017, The Regents of the University of California (Regents).
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS 
PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, 
UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

*************************************************************************** */

// Written: fmckenna

#include <SimCenterDirWatcher.h>
#include <QFileSystemWatcher>
#include <QDebug>
#include <QString>

SimCenterDirWatcher::SimCenterDirWatcher()
  :currentCount(0), maxCount(0)
{
  theDirWatcher = new QFileSystemWatcher();
  connect(theDirWatcher, SIGNAL(directoryChanged(QString)),
      this, SLOT(changed(QString)));
}
  
SimCenterDirWatcher::~SimCenterDirWatcher()
{
  delete theDirWatcher;
}

void
SimCenterDirWatcher::setCount(int count)
{
  maxCount = count;
}

void
SimCenterDirWatcher::resetCount(void) {
  currentCount = 0;
}

bool
SimCenterDirWatcher::setDirToMonitor(QString dir)
{
   qDebug() << "\n\nsetDirToMonitor: " << dir << "\n\n";
  if (!currentDir.isEmpty())
    theDirWatcher->removePath(currentDir);

  if (theDirWatcher->addPath(dir) != true) {
    QString errorMessage = QString("SimCenterDirWatcher - could not addPath: ") + dir;
    qDebug() << errorMessage;
  }
  currentDir = dir;
  currentCount = 0;
  return true;
};

void
SimCenterDirWatcher::changed(const QString &dir)
{
  currentCount++;
  if (currentCount == maxCount) // don't worry about going over
    emit countReached();
  else
    emit dirChanged();

  qDebug() << "\n\nSimCenterDirWatcher::changed " << dir << " " << currentCount << "\n\n";
};
