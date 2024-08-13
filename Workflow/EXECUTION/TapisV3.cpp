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
#include "TapisV3.h"
#include <QApplication>
#include <QProcess>
#include <QDebug>
#include <QTemporaryFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QDir>
#include <QJsonObject>
#include <QDesktopServices>

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <iterator>
#include <string>

#include <QWindow>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStringList>

#include <QUuid>
#include <QFuture>
#include <QDialog>
#include <QStandardPaths>
#include <QDir>

using namespace std;

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}


struct data {
  char trace_ascii; /* 1 or 0 */
};
 
static
void dump(const char *text,
          FILE *stream, unsigned char *ptr, size_t size,
          char nohex)
{
  size_t i;
  size_t c;
 
  unsigned int width = 0x10;
 
  if(nohex)
    /* without the hex output, we can fit more on screen */
    width = 0x40;
 
  fprintf(stream, "%s, %10.10lu bytes (0x%8.8lx)\n",
          text, (unsigned long)size, (unsigned long)size);
 
  for(i = 0; i<size; i += width) {
 
    fprintf(stream, "%4.4lx: ", (unsigned long)i);
 
    if(!nohex) {
      /* hex not disabled, show it */
      for(c = 0; c < width; c++)
        if(i + c < size)
          fprintf(stream, "%02x ", ptr[i + c]);
        else
          fputs("   ", stream);
    }
 
    for(c = 0; (c < width) && (i + c < size); c++) {
      /* check for 0D0A; if found, skip past and start a new line of output */
      if(nohex && (i + c + 1 < size) && ptr[i + c] == 0x0D &&
         ptr[i + c + 1] == 0x0A) {
        i += (c + 2 - width);
        break;
      }
      fprintf(stream, "%c",
              (ptr[i + c] >= 0x20) && (ptr[i + c]<0x80)?ptr[i + c]:'.');
      /* check again for 0D0A, to avoid an extra \n if it's at width */
      if(nohex && (i + c + 2 < size) && ptr[i + c + 1] == 0x0D &&
         ptr[i + c + 2] == 0x0A) {
        i += (c + 3 - width);
        break;
      }
    }
    fputc('\n', stream); /* newline */
  }
  fputc('\n\n', stream); /* newline */  
  fflush(stream);
}

static char theBlahFile='a';

static
int my_trace(CURL *handle, curl_infotype type,
             char *data, size_t size,
             void *userp)
{
  struct data *config = (struct data *)userp;
  const char *text;
  (void)handle; /* prevent compiler warning */
 
  switch(type) {
  case CURLINFO_TEXT:
    fprintf(stderr, "== Info: %s", data);
    return 0;
  case CURLINFO_HEADER_OUT:
    text = "=> Send header";
    break;
  case CURLINFO_DATA_OUT:
    text = "=> Send data";
    break;
  case CURLINFO_SSL_DATA_OUT:
    text = "=> Send SSL data";
    break;
  case CURLINFO_HEADER_IN:
    text = "<= Recv header";
    break;
  case CURLINFO_DATA_IN:
    text = "<= Recv data";
    break;
  case CURLINFO_SSL_DATA_IN:
    text = "<= Recv SSL data";
    break;
  default: /* in case a new one is introduced to shock us */
    return 0;
  }

  QString debugFilePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
    + QDir::separator() + QCoreApplication::applicationName() + QDir::separator()
    + QString(theBlahFile) + QString(".txt");
  
  QByteArray byteArray = debugFilePath.toUtf8();
  const char* cString = byteArray.data();
  // theBlahFile++;
  //FILE *fileOut = fopen("Users/fmckenna/Documents/quoFEM/curl.txt","a");
  FILE *fileOut = fopen(cString,"a");  
  dump(text, fileOut, (unsigned char *)data, size, config->trace_ascii);
  fclose(fileOut);
  return 0;
}


TapisV3::TapisV3(QString &_tenant, QString &_storage, QString *appDir, QObject *parent)
    :RemoteService(parent), tenant(_tenant), storage(_storage), loggedInFlag(false), slotNeededLocally(false)
{
    //
    // for operation this class needs two temporary files to function
    //  - hence thing1 and thing2

    //Get application data folder
    QString writableLocation = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::AppLocalDataLocation);

    //Create the folder if it does not exist
    QDir writableDir(writableLocation);
    if(!writableDir.exists())
        writableDir.mkpath(".");

    uniqueFileName1 = writableDir.filePath("SimCenter.thing1");
    uniqueFileName2 = writableDir.filePath("SimCenter.thing2");

    //
    // init curl variables
    //
    
    curl_global_init(CURL_GLOBAL_DEFAULT);
    hnd = curl_easy_init();
    slist1 = NULL;
    slist2 = NULL;

    //tenantURL="https://designsafe.staging.tapis.io/";
    tenantURL="https://designsafe.tapis.io/";    
    appClient = QString("appClient");

    if (appDir != nullptr)
        appDirName = QString(*appDir);
}

TapisV3::~TapisV3()
{
    //
    // clean up, remove temp files, delete QProcess and delete login
    //  - deleted login as never set the widgets parent window

    // if we have logged in .. delete the client app

    if (loggedInFlag == true) {

        QString url = tenantURL + QString("clients/v2/") + appClient;
        QString user_passwd = username + QString(":") + password;

        // note should not use emit as object which was sending deleted before this one
        curl_easy_setopt(hnd, CURLOPT_URL, url.toStdString().c_str());
        curl_easy_setopt(hnd, CURLOPT_USERPWD, user_passwd.toStdString().c_str());
        curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "DELETE");
        this->invokeCurl();
    }

    curl_slist_free_all(slist1);
    slist1 = NULL;

    //
    // remove temporary files
    //

    QFile file1 (uniqueFileName1);
    file1.remove();

    QFile file2 (uniqueFileName2);
    file2.remove();

    //
    // finally invoke cleanup
    //

    curl_easy_cleanup(hnd);
}

bool
TapisV3::isLoggedIn()
{
    return loggedInFlag;
}


void
TapisV3::loginCall(QString uname, QString upassword)
{
    bool result = this->login(uname, upassword);
    emit loginReturn(result);
    if (result == true) {
        QString result = storage + username;
        if (!appDirName.isEmpty())
            result = result + "/" + appDirName;

	this->homeDir = result;
	
        emit getHomeDirPathReturn(result);
    }
}


bool
TapisV3::login(QString uname, QString upassword)
{
    username = uname;
    password = upassword;

    curl_slist_free_all(slist1);
    slist1 = NULL;

    //TODO: update this
    //
    // first try deleting old , needed if program crashed or old not deleted
    // before thread was shutdown
    //

    QString message = QString("Contacting ") + tenant + QString(" to log in.");
    emit statusMessage(message);

    QString url = tenantURL + QString("clients/v3/") + appClient;
    QString user_passwd = "{\"username\":\"" + username +"\", \"password\": \"" + password + "\", \"grant_type\": \"password\" }";

    /* FMK - uncomment to debug SSL calls
    struct data config;
    config.trace_ascii = 1;
    curl_easy_setopt(hnd, CURLOPT_DEBUGFUNCTION, my_trace);
    curl_easy_setopt(hnd, CURLOPT_DEBUGDATA, &config);
    curl_easy_setopt(hnd, CURLOPT_VERBOSE, 1L);
    */

    curl_easy_setopt(hnd, CURLOPT_URL, url.toStdString().c_str());
    curl_easy_setopt(hnd, CURLOPT_USERPWD, user_passwd.toStdString().c_str());
    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "DELETE");

    if (this->invokeCurl() == false) {
        return false;
    }

    //
    // now login & get auth token
    //
    
    username = uname;
    password = upassword;


    // Set URL to fetch    
    url = tenantURL + QString("v3/oauth2/tokens");
    curl_easy_setopt(hnd, CURLOPT_URL, url.toStdString().c_str());

    // Set HTTP headers    
    slist1 = NULL;
    slist1 = curl_slist_append(slist1, "Content-type: application/json");
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);    

    // Set POST data    
    user_passwd =QString("{\"username\": \"%1\", \"password\": \"%2\", \"grant_type\": \"password\" }").arg(username, password);
    std::string postData = user_passwd.toStdString();

    curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, postData.c_str());

    /* commented out to get working, never tested if needed
    curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
    curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
    curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/8.4.0");
    curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
    curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
    */
      
    if (this->invokeCurl() == false) {
        return false;
    }

    QFile file2(uniqueFileName1);
    if (!file2.open(QFile::ReadOnly | QFile::Text)) {
        emit errorMessage("ERROR: COULD NOT OPEN RESULT of OAuth");
        return false;
    }

    // read results file & check for errors
    QString val=file2.readAll();
    file2.close();

    if (val.contains("Invalid username/password combination.") || val.isEmpty()) {
        emit errorMessage("ERROR: Invalid Credentials in OAuth!");
        return false;
	
    } else {
      
        QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
        QJsonObject jsonObj = doc.object();
	
        if (jsonObj.contains("result")) {
            QJsonObject resultObj = jsonObj["result"].toObject();
            if (resultObj.contains("access_token")) {
	      
                QJsonObject tokenObj = resultObj["access_token"].toObject();
                accessToken = tokenObj["access_token"].toString();
                bearer = QString("Authorization: Bearer ") + accessToken;
                slist1 = curl_slist_append(slist1, bearer.toStdString().c_str());
                loggedInFlag = true;

                // now if appDir is specified make sure dir exists,
                // creating it does delete existing one and one call as opposed to 2 if not there
		
                if (!appDirName.isEmpty()) {
		  
                    QString home = storage + username;
                    bool ok = this->mkdir(appDirName, username);
                    if (ok != true) {
                        QString message = QString("WARNING - could not create " ) + appDirName
                                          + QString(" on login, using home dir instead");
                        appDirName = QString(""); // no erase function!
                        emit statusMessage(message);
                    } else {
                        emit statusMessage("Login SUCCESS");
                    }
                } else {
                    emit statusMessage("Login SUCCESS");
                }

                return true;
            }
            emit statusMessage("ERROR - no access token returned!");
            return false;
        }
        emit statusMessage("ERROR - no results returned for token call!");
        return false;
    }

    return false;
}

void
TapisV3::logoutCall()
{
    bool result = this->logout();
    emit logoutReturn(result);
}

bool
TapisV3::logout()
{
    if (loggedInFlag == false)
        return true;


    // don't need to log out since v3 uses access tokens

    emit statusMessage("Logout SUCCESS");

    accessToken = "";
    loggedInFlag = false;

    return true;
}


void
TapisV3::getHomeDirPathCall(void) {
    QString result = storage + username;
    emit getHomeDirPathReturn(result);
}

QString
TapisV3::getHomeDirPath(void) {
    QString result = storage + username;
    return result;
}


void
TapisV3::uploadDirectoryCall(const QString &local, const QString &remote)
{
    bool result = this->uploadDirectory(local, remote);
    emit uploadDirectoryReturn(result);
}

bool
TapisV3::uploadDirectory(const QString &local, const QString &remote)
{
    //
    // check local exists
    //

    QDir originDirectory(local);
    if (! originDirectory.exists()) {
        emit errorMessage(QString("ERROR - local directory does not exist : ") + local);
        return false;
    }
    QString dirName = originDirectory.dirName();

    //
    // create remote dir
    //

    QString remoteCleaned = remote;
    remoteCleaned.remove(storage);
    if (this->mkdir(dirName, remoteCleaned) != true)
        return false;
    remoteCleaned = remoteCleaned + QString("/") + dirName;

    //
    // now we go through each file in local dir & upload to new remote directory
    //   - if dir we recursivily call the method
    //

    // originDirectory.mkpath(destinationDir);

    foreach (QString directoryName, originDirectory.entryList(QDir::Dirs | \
                                                              QDir::NoDotAndDotDot))
    {
        QString nextRemote = remoteCleaned + "/" + directoryName;
        QString nextLocal = local + QDir::separator() + directoryName;

        if (this->uploadDirectory(nextLocal, remoteCleaned) != true) {
            this->removeDirectory(remoteCleaned); // remove any directory we created if failure
            return false;
        }
    }

    foreach (QString fileName, originDirectory.entryList(QDir::Files))
    {
        QString localFile = local + QDir::separator() + fileName;
        QString remoteFile = remoteCleaned + "/" + fileName;
        if (this->uploadFile(localFile, remoteFile) != true) {
            this->removeDirectory(remoteCleaned);
            return false;
        }
    }

    return true;
}


void
TapisV3::removeDirectoryCall(const QString &remote) {
    bool result = this->removeDirectory(remote);
    emit removeDirectoryReturn(result);
}

bool
TapisV3::removeDirectory(const QString &remote)
{
    QString message = QString("Contacting ") + tenant + QString(" to remove dir ") + remote;
    emit statusMessage(message);

    // invoke curl to remove the file or directory
    QString url = tenantURL + QString("v3/files/ops/") + storage + remote;
    curl_easy_setopt(hnd, CURLOPT_URL, url.toStdString().c_str());
    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "DELETE");
    if (this->invokeCurl() == false) {
        return false;
    }

    //
    // process the results
    //

    // open results file
    QFile file(uniqueFileName1);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        emit errorMessage("ERROR: removeDirectory .. COULD NOT OPEN RESULT .. libCurl ERROR");
        return false;
    }

    // read results file & check for errors
    QString val;
    val=file.readAll();
    file.close();

    // read into json object
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject theObj = doc.object();

    // parse json object for status
    //   if error emit error message & return NOT FOUND, if no status it's an error
    //   if success then get result & return the job sttaus

    QString status;
    if (theObj.contains("status")) {
        status = theObj["status"].toString();
        if (status == "error") {

            // if error get errormessage and return error
            QString message("Job Not Found");
            if (theObj.contains("message"))
                message = theObj["message"].toString();

            emit errorMessage(message);
            return false;

        } else if (status == "success") {
            emit errorMessage("Succesfully removed directory");
            return true;
        }

    } else if (theObj.contains("fault")) {
        QJsonObject theFault = theObj["fault"].toObject();
        if (theFault.contains("message")) {
            QString message = theFault["message"].toString();
            emit errorMessage(message);
            return false;
        }
    }
    return false;
}

void
TapisV3::mkdirCall(const QString &remoteName, const QString &remotePath) {
    bool result = mkdir(remoteName, remotePath);
    emit mkdirReturn(result);
}

bool
TapisV3::mkdir(const QString &remoteName, const QString &remotePath) {

    QString message = QString("Contacting ") + tenant + QString(" to create dir ") + remotePath + QString("/") + remoteName;
    emit statusMessage(message);

    bool result = false;

    /*
    // Set POST data    
    user_passwd =QString("{\"username\": \"%1\", \"password\": \"%2\", \"grant_type\": \"password\" }").arg(username, password);
    std::string postData = user_passwd.toStdString();

    curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, postData.c_str());      
     */

    /* FMK DEBUG */
    struct data config;
    config.trace_ascii = 1;
    curl_easy_setopt(hnd, CURLOPT_DEBUGFUNCTION, my_trace);
    curl_easy_setopt(hnd, CURLOPT_DEBUGDATA, &config);
    curl_easy_setopt(hnd, CURLOPT_VERBOSE, 1L);
    
    // Set URL to fetch    
    QString url = tenantURL + QString("v3/files/ops/") + storage;
    curl_easy_setopt(hnd, CURLOPT_URL, url.toStdString().c_str());

    // Set HTTP headers    
    slist1 = NULL;
    std::string headerData = QString("x-tapis-token: %1").arg(accessToken).toStdString();    
    slist1 = curl_slist_append(slist1, headerData.c_str());
    slist1 = curl_slist_append(slist1, "content-type:application/json");
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
    
    // Set Post Data
    QString postField = QString("{\"path\":\"%1\"}").arg(remotePath + "/" + remoteName);
    std::string postData = postField.toStdString();
    curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, postData.c_str());    

    /*
    int postFieldLength = postField.length() ; // strlen(postFieldChar);
    char *pField = new char[postFieldLength+1]; // have to do new char as seems to miss ending string char when pass directcly
    strncpy(pField, postField.toStdString().c_str(),postFieldLength+1);
    curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, pField);
    */
    
    /*
    curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
    curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
    std::string postData = postField.toStdString();
    curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, postData.c_str());
    curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
    curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
    */
    
    if (this->invokeCurl() == false) {
        return false;
    }

    //
    // process the results
    //

    // open results file
    QFile file(uniqueFileName1);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        emit errorMessage("ERROR: removeDirectory .. COULD NOT OPEN RESULT");
        return false;
    }

    // read results file & check for errors
    QString val;
    val=file.readAll();
    file.close();
    
    // read into json object
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject theObj = doc.object();

    // parse json object for status
    //   if error emit error message & return NOT FOUND, if no status it's an error
    //   if success then get result & return the job sttaus

    QString status;
    if (theObj.contains("status")) {
        status = theObj["status"].toString();
        if (status == "error") {
            QString message("Job Not Found");
            if (theObj.contains("message"))
                message = theObj["message"].toString();
            emit errorMessage(message);
            return false;
        } else if (status == "success") {
            return true;
        }
    } else {
        QJsonDocument doc(theObj);
        QString strJson(doc.toJson(QJsonDocument::Compact));
        emit errorMessage(strJson);
        return false;
    }

    return result;
}

void
TapisV3::uploadFileCall(const QString &local, const QString &remote) {
    bool result = this->uploadFile(local, remote);
    emit uploadFileReturn(result);
}

bool
TapisV3::uploadFile(const QString &local, const QString &remote) {

    QFileInfo localNameInfo(local);
    QString localName = localNameInfo.fileName();

    QString message = QString("Contacting ") + tenant + QString(" to upload file ") + localName;
    emit statusMessage(message);

    //
    // obtain filename and remote path from the remote string
    // note: for upload we need to remove the agave storage URL if there
    //

    QString remoteCleaned = remote;
    remoteCleaned.remove(storage);
    QFileInfo   fileInfo(remoteCleaned);


    curl_mime *mime1;
    curl_mimepart *part1;

    mime1 = curl_mime_init(hnd);
    part1 = curl_mime_addpart(mime1);
    curl_mime_filedata(part1, local.toStdString().c_str());
    curl_mime_name(part1, "file");
    curl_easy_setopt(hnd, CURLOPT_MIMEPOST, mime1);

    std::string headerData = QString("X-Tapis-Token: %1").arg(accessToken).toStdString();
    slist1 = NULL;
    slist1 = curl_slist_append(slist1, headerData.c_str());

    QString url = tenantURL + QString("v3/files/ops/") + storage + remoteCleaned;
    curl_easy_setopt(hnd, CURLOPT_URL, url.toStdString().c_str());
    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");

    if (this->invokeCurl() == false) {
        emit errorMessage("AgaveCurl:: invokeCurl failed!");
        return false;
    }

    //
    // process the results
    //

    // open results file
    QFile file(uniqueFileName1);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        emit errorMessage("ERROR: COULD NOT OPEN RESULT");
        return false;
    }

    // read results file & check for errors
    QString val;
    val=file.readAll();
    file.close();

    // emit errorMessage(val);

    // read into json object
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject theObj = doc.object();

    // parse json object for status
    //   if error emit error message & return NOT FOUND, if no status it's an error
    //   if success then get result & return the job sttaus

    QString status;
    if (theObj.contains("status")) {
        status = theObj["status"].toString();
        if (status == "error") {
            QString message("Job Not Found");
            if (theObj.contains("message"))
                message = theObj["message"].toString();
            emit errorMessage(message);
            return false;
        } else if (status == "success") {
            emit statusMessage("Successfully uploaded file");
            return true;
        }

    } else if (theObj.contains("fault")) {
        QJsonObject theFault = theObj["fault"].toObject();
        if (theFault.contains("message")) {
            QString message = theFault["message"].toString();
            emit errorMessage(message);
            return false;
        }
    }

    return false;
}


void
TapisV3::downloadFilesCall(const QStringList &remoteFiles, const QStringList &localFiles, QObject* sender) {
    bool result = true;
    for (int i=0; i<remoteFiles.size(); i++) {
        QString remote = remoteFiles.at(i);
        QString local = localFiles.at(i);

        result = this->downloadFile(remote, local);
        if (result == false) {
            emit downloadFilesReturn(result, sender);
        }
    }

    emit downloadFilesReturn(result, sender);
}

bool
TapisV3::downloadFile(const QString &remoteFile, const QString &localFile)
{
    // this method does not invoke the invokeCurl() as want to write to local file directtly

    CURLcode ret;

    QFileInfo remoteFileInfo(remoteFile);
    QString remoteName = remoteFileInfo.fileName();
    //   QString localName = localFile.fileName();

    QString message = QString("Contacting ") + tenant + QString(" to download remote file ") + remoteName; // + QString( " to ") + localFile;


    // set up the call
    QString url = tenantURL + QString("v3/files/content/") + storage + remoteFile;

    std::string headerData = QString("X-Tapis-Token: %1").arg(accessToken).toStdString();
    slist1 = NULL;
    slist1 = curl_slist_append(slist1, headerData.c_str());
    curl_easy_setopt(hnd, CURLOPT_URL, url.toStdString().c_str());
    curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.54.0");
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
    curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
    curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0L);
    // curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(hnd, CURLOPT_FAILONERROR, true);

    // openup localfile and set the writedata pointer to it
    FILE *pagefile = fopen(localFile.toStdString().c_str(), "wb");
    if(pagefile) {
        curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(hnd, CURLOPT_WRITEDATA, pagefile);
    }

    // make the call
    ret = curl_easy_perform(hnd);
    fclose(pagefile);

    // reset the handle for next method as per normal
    curl_easy_reset(hnd);

    // if no error, return
    if (ret == CURLE_OK) {
        emit statusMessage("Successfully downloaded file");
        return true;
    }

    // if failure, go get message, emit signal and return false;

    message = QString("Failed to Download File: ") + remoteFile; // more descriptive message

    emit errorMessage(message);
    return false;
}


QJsonArray
TapisV3::remoteLS(const QString &remotePath)
{
    QJsonArray result;
    // this method does not invoke the invokeCurl() as want to write to local file directtly

    QFileInfo remoteFileInfo(remotePath);
    if(remotePath.isEmpty())
        remoteFileInfo.setFile(username);


    QString remoteName = remoteFileInfo.fileName();
    QString message = QString("Contacting ") + tenant + QString(" to get dir listing ") + remoteName;
    emit statusMessage(message);

    // set up the call
    QString url = tenantURL + QString("v3/files/ops/") + storage + remotePath;
    if(remotePath.isEmpty())
        url.append(username);

    std::string headerData = QString("X-Tapis-Token: %1").arg(accessToken).toStdString();
    slist1 = NULL;
    slist1 = curl_slist_append(slist1, headerData.c_str());
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
    curl_easy_setopt(hnd, CURLOPT_URL, url.toStdString().c_str());
    if (this->invokeCurl() == false) {
        return result;
    }

    //
    // process the results
    //

    // open results file
    QFile file(uniqueFileName1);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        emit errorMessage("ERROR: COULD NOT OPEN RESULT .. remoteLS!");
        return result;
    }

    // read results file & check for errors
    QString val;
    val=file.readAll();
    file.close();

    // read into json object
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject theObj = doc.object();

    QString status;
    if (theObj.contains("status")) {
        status = theObj["status"].toString();
        if (status == "error") {
            QString message("Directory Not Found");
            if (theObj.contains("message"))
                message = QString("ERROR: " ) + theObj["message"].toString();
            emit errorMessage(message);
            return result;
        } else if (status == "success") {
            if (theObj.contains("result")) {
                result = theObj["result"].toArray();
                QString message = QString("Succesfully obtained listing: ") + remoteName;
                emit statusMessage(message);
                return result;
            }
        }

    } else if (theObj.contains("fault")) {
        QJsonObject theFault = theObj["fault"].toObject();
        if (theFault.contains("message")) {
            QString message = theFault["message"].toString();
            emit errorMessage(message);
            return result;
        }
    }

    return result;

}



QString
TapisV3::startJob(const QString &jobDescriptionFile)
{
    QString result = "FAILURE";

    //
    // openfile, put in QJsonObj and call other method
    //

    // open results file
    QFile file(jobDescriptionFile);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        emit errorMessage("ERROR: COULD NOT OPEN RESULT");
        return result;
    }

    // read results file & check for errors
    QString val;
    val=file.readAll();
    file.close();

    // read into json object
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject theObj = doc.object();

    return startJob(theObj);
}

void
TapisV3::startJobCall(const QJsonObject &theJob) {

    QString result = startJob(theJob);
    emit startJobReturn(result);
}

QString
TapisV3::startJob(const QJsonObject &theJob)
{

    QString result = "FAILURE";

    slist1 = NULL;
    slist1 = curl_slist_append(slist1, "Content-Type: application/json");
    slist1 = curl_slist_append(slist1, bearer.toStdString().c_str());

    std::string headerData = QString("X-Tapis-Token: %1").arg(accessToken).toStdString();
    slist1 = curl_slist_append(slist1, headerData.c_str());
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);

    QJsonDocument docJob(theJob);
    QString strJson(docJob.toJson(QJsonDocument::Compact));
    QByteArray ba = strJson.toLocal8Bit();
    const char *c_str2 = ba.data();
    curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, c_str2);
    //  curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)strlen(c_str2));

    QString url = tenantURL + QString("v3/jobs/submit");
    curl_easy_setopt(hnd, CURLOPT_URL, url.toStdString().c_str());
    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");

    if (this->invokeCurl() == false) {
        return result;
    }

    //
    // process the results
    //

    // open results file
    QFile file(uniqueFileName1);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        emit errorMessage("ERROR: COULD NOT OPEN RESULT");
        return result;
    }

    // read results file & check for errors
    QString val;
    val=file.readAll();
    file.close();

    // read into json object
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject theObj = doc.object();

    // parse json object for status
    //   if error emit error message & return NOT FOUND, if no status it's an error
    //   if success then get result & return the job sttaus

    QString status;
    if (theObj.contains("status")) {
        status = theObj["status"].toString();
        if (status == "error") {
            QString message("Job Not Found");
            if (theObj.contains("message")) {
	      
	      message = theObj["message"].toString();
	      if (message.contains("SYSTEMS_MISSING_CREDENTIALS")) {
		if (message.contains("stampede3")) {
		  QDesktopServices::openUrl(QUrl(QString("https://www.designsafe-ci.org/rw/workspace/stampede3-credential"), QUrl::TolerantMode));
		  message = QString("ERROR: You need TACC machine credentials, log-in to DesignSafe & click on green 'Submit Job' button on webpage");
		} else if (message.contains("frontera")) {
		  QDesktopServices::openUrl(QUrl(QString("https://www.designsafe-ci.org/rw/workspace/frontera-credential"), QUrl::TolerantMode));
		 message = QString("ERROR: You need TACC machine credentials, log-in to DesignSafe & click on green 'Submit Job' button on webpage");
		} else if (message.contains("ls6")) {
		  QDesktopServices::openUrl(QUrl(QString("https://www.designsafe-ci.org/rw/workspace/ls6-credential"), QUrl::TolerantMode));
		 message = QString("ERROR: You need TACC machine credentials, log-in to DesignSafe & click on green 'Submit Job' button on webpage");
		} else {
		  message = QString("ERROR: Credentials have not been set, go to tool home page for instruction");
		}
	      } else {
		message = QString("ERROR: " ) + theObj["message"].toString();
	      }
	    }
	    
            emit errorMessage(message);
            return result;
        } else if (status == "success") {
            if (theObj.contains("result")) {
                QJsonObject resObj = theObj["result"].toObject();
                if (resObj.contains("uuid")) {
                    QString jobID =  resObj["uuid"].toString();
                    QString message = QString("Successfully started job: ") + jobID;
                    emit statusMessage(message);
                    QString msg1("Press the \"Get from DesignSafe\" Button to see status and download results");
                    emit statusMessage(msg1);
                    return jobID;
                }
            }
        }

    } else if (theObj.contains("fault")) {
        QJsonObject theFault = theObj["fault"].toObject();
        if (theFault.contains("message")) {
            QString message = theFault["message"].toString();
            emit errorMessage(message);
            return result;
        }
    } else {
        QString message("Job failed for unknown reason");;
        emit errorMessage(message);
        return result;
    }

    return result;
}


void
TapisV3::getJobListCall(const QString &matchingName, QString appIdFilter) {
    QJsonObject result = getJobList(matchingName, appIdFilter);
    emit getJobListReturn(result);
}

QJsonObject
TapisV3::getJobList(const QString &matchingName, QString appIdFilter)
{
    //TODO: implement matching
    Q_UNUSED(matchingName);
    QString message = QString("Contacting ") + tenant + QString(" to Get Job list");
    emit statusMessage(message);

    QJsonObject result;

    QString url = tenantURL + QString("v3/jobs/list?orderBy=lastUpdated(desc),name(asc)&computeTotal=true");
    if (!appIdFilter.isEmpty())
    {
        QString params = QString("?appId.like=%1").arg(appIdFilter);
        url.append(params);
    }

    std::string headerData = QString("X-Tapis-Token: %1").arg(accessToken).toStdString();
    slist1 = NULL;
    slist1 = curl_slist_append(slist1, headerData.c_str());
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
    curl_easy_setopt(hnd, CURLOPT_URL, url.toStdString().c_str());
    this->invokeCurl();

    //
    // process the results
    //

    // open results file
    QFile file(uniqueFileName1);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        emit errorMessage("ERROR: COULD NOT OPEN RESULT");
        return result;
    }

    // read results file & check for errors
    QString val;
    val=file.readAll();
    file.close();

    if ((val.contains("Missing Credentals")) || (val.contains("Invalid Credentals"))){
        emit errorMessage("ERROR: Trouble LOGGING IN .. try Logout and Login Again");
        return result;
    } else if ((val.contains("Service Unavailable"))){
        QString message = QString("ERROR ") + tenant + QString(" Jobs Service Unavailable .. contact DesignSafe-ci");
        emit errorMessage(message);
        return result;
    }

    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject theObj = doc.object();
    if (theObj.contains("result")){
        QJsonArray jobs = theObj["result"].toArray();
        result["jobs"] = jobs;
        emit statusMessage("Successfully obtained list of submitted jobs");
        emit statusMessage("Click in any job shown in table to update the job status, download the job or delete the job.");
    }

    return result;
}

void
TapisV3::getJobDetailsCall(const QString &jobID)
{
    QJsonObject result = getJobDetails(jobID);
    emit getJobDetailsReturn(result);
}


QJsonObject
TapisV3::getJobDetails(const QString &jobID)
{
    QJsonObject result;

    QString message = QString("Contacting ") + tenant + QString(" to Get Job Details of ") + jobID;
    emit statusMessage(message);

    QString url = tenantURL + QString("v3/jobs/") + jobID;

    std::string headerData = QString("X-Tapis-Token: %1").arg(accessToken).toStdString();
    slist1 = NULL;
    slist1 = curl_slist_append(slist1, headerData.c_str());
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
    curl_easy_setopt(hnd, CURLOPT_URL, url.toStdString().c_str());
    if (this->invokeCurl() == false) {
        return result;
    }

    //
    // process the results
    //

    // open results file
    QFile file(uniqueFileName1);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        emit errorMessage("ERROR: COULD NOT OPEN RESULT .. getJobDetails!");
        return result;
    }

    // read results file & check for errors
    QString val;
    val=file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject theObj = doc.object();
    if (theObj.contains("result")){
        result = theObj["result"].toObject();
    }

    return result;
}

void
TapisV3::getJobStatusCall(const QString &jobID){
    QString result = this->getJobStatus(jobID);
    emit getJobStatusReturn(result);
}


QString
TapisV3::getJobStatus(const QString &jobID){

    QString result("NO JOB FOUND");

    //
    // invoke curl
    //

    QString message = QString("Contacting ") + tenant + QString(" to Get Job Status of ") + jobID;
    emit statusMessage(message);

    QString url = tenantURL + QString("v3/jobs/") + jobID + QString("/status");
    std::string headerData = QString("X-Tapis-Token: %1").arg(accessToken).toStdString();
    slist1 = NULL;
    slist1 = curl_slist_append(slist1, headerData.c_str());
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
    curl_easy_setopt(hnd, CURLOPT_URL, url.toStdString().c_str());
    if (this->invokeCurl() == false) {
        return result;
    }

    //
    // process the results
    //

    QFile file(uniqueFileName1);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        emit errorMessage("ERROR: COULD NOT OPEN RESULT from getJobStatus .. permission issue!");
        return result;
    }

    // read results file
    QString val;
    val=file.readAll();
    file.close();

    // read into json object
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject theObj = doc.object();

    // parse json object for status
    //   if error emit error message & return NOT FOUND
    //   if sucess then get result & return the job sttaus

    QString status;
    if (theObj.contains("status")) {
        status = theObj["status"].toString();
        if (status == "error") {
            QString message("Job Not Found");
            if (theObj.contains("message"))
                message = theObj["message"].toString();
            emit errorMessage(message);
            return result;

        } else if (status == "success")
            if (theObj.contains("result")) {
                QJsonObject resultObj = theObj["result"].toObject();
                if (resultObj.contains("status")) {
                    result = resultObj["status"].toString();
                    QString message = QString("Successfully obtained job status: " + result);
                    emit statusMessage(message);
                    return result;
                }
            }
    }

    return result;
}

void
TapisV3::deleteJobCall(const QString &jobID, const QStringList &dirToRemove) {
    bool result = this->deleteJob(jobID, dirToRemove);
    emit deleteJobReturn(result);
}

void TapisV3::remoteLSCall(const QString &remotePath)
{
    QJsonArray dirList = this->remoteLS(remotePath);
    emit remoteLSReturn(dirList);
}

bool
TapisV3::deleteJob(const QString &jobID, const QStringList &dirToRemove)
{
    bool result = false;

    //
    // first remove the directories
    //

    foreach(QString item, dirToRemove) {
        result = this->removeDirectory(item);
    }
    Q_UNUSED(result);

    //
    // invoke curl to delete the job
    //

    QString message = QString("Contacting ") + tenant + QString(" to delete job");
    emit statusMessage(message);

    QString url = tenantURL + QString("v3/jobs/") + jobID + QString("/hide");
    std::string headerData = QString("X-Tapis-Token: %1").arg(accessToken).toStdString();
    slist1 = NULL;
    slist1 = curl_slist_append(slist1, headerData.c_str());
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);

    curl_easy_setopt(hnd, CURLOPT_URL, url.toStdString().c_str());
    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");

    if (this->invokeCurl() == false) {
        return false;
    }

    //
    // process the results
    //

    // open results file
    QFile file(uniqueFileName1);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        emit errorMessage("ERROR: COULD NOT OPEN RESULT");
        return false;
    }

    // read into json object
    QString val;
    val=file.readAll();
    file.close();
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject theObj = doc.object();

    // parse json object for status
    //   if error emit error message & return NOT FOUND
    //   if sucess then get result & return the job sttaus

    QString status;
    if (theObj.contains("status")) {
        status = theObj["status"].toString();
        if (status == "error") {
            QString message("Unknown ERROR with Curl Call - deleteJob");
            if (theObj.contains("message"))
                message = theObj["message"].toString();
            qDebug() << "ERROR MESSAGE: " << message;
            emit errorMessage(message);
            return false;
        } else if (status == "success") {
            emit statusMessage("Successfully deleted job");
            return true;
        }
    }
    return false;
}




bool
TapisV3::invokeCurl(void) {

    CURLcode ret;

    //
    // the methods set many of the options, this private method just sets the
    // default ones, invokes curl and then reset the handler for the next call
    // if an error occurs it gets the error messag and emits a signal before returning false
    //

    // set default options
    curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.54.0");

    if (slist2 != NULL) {
        curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist2);
    }
    else if (slist1 != NULL) {
        curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
    }

    curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
    //curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
    curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0L);
    //curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYHOST, 0L); - not recomended and not working on the mac
    curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);

    //  curl_easy_setopt(hnd, CURLOPT_VERBOSE, 1L);

    // we send the result of curl request to a file > uniqueFileName1
    FILE *pagefile = fopen(uniqueFileName1.toStdString().c_str(), "wb");
    if(pagefile) {
        curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(hnd, CURLOPT_WRITEDATA, pagefile);
    }

    // perform the curl operation that has been set up
    ret = curl_easy_perform(hnd);
    fclose(pagefile);

    // reset the handle so methods can fill in the different options before next call
    curl_easy_reset(hnd);
    if (slist2 != NULL) {
        curl_slist_free_all(slist2);
        slist2 = NULL;
    }

    /* ************************************************
  // spit out to error the return data
  QFile file(uniqueFileName1);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    emit errorMessage("ERROR: COULD NOT OPEN RESULT");
  }
  QString val;
  val=file.readAll();
  file.close();
  errorMessage(val);
  *************************************************** */

    // check for success
    if (ret == CURLE_OK)
        return true;

    // if failure, go get message, emit signal and return false;
    const char *str = curl_easy_strerror(ret);
    QString errorString(str);
    emit errorMessage(QString("TapisV3 ERROR: " ) + QString(errorString));

    return false;
}
