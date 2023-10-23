#include "GoogleAnalytics.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QHostInfo>
#include <QCoreApplication>
#include <QSettings>

namespace GoogleAnalytics {

void ReportLocalRun()
{
    Report("Simulation", "Local");
}

void ReportDesignSafeRun()
{
    Report("Simulation", "DesignSafe");
}

void ReportAppUsage(QString appName)
{
    Report("Application", appName);
}  

void StartSession()
{
    Report("Session", "Start");

}

void EndSession()
{
    Report("Session", "End");
}

//TODO: This code may need to be refactored and shared in SimCenterCommon
QUuid GetCommonClientId()
{
    QSettings commonSettings("SimCenter", "Common"); //These names will need to be constants to be shared
    QVariant clientIdSetting = commonSettings.value("clientId");
    if (!clientIdSetting.isValid())
    {
        commonSettings.setValue("clientId", QUuid::createUuid());
        clientIdSetting = commonSettings.value("clientId");
    }
    return clientIdSetting.toUuid();
}

void SetMeasurementId(QString measurementId)
{
    _measurementId = measurementId;
}

void SetAPISecret(QString api_secret)
{
    _api_secret = api_secret;
}

void CreateSessionId()
{
    _sessionId = QUuid::createUuid();
}

void sendReport();

void Report(QString eventCategory, QString eventName)
{
    // no tracking if no tracking ID set
    if (_measurementId.isEmpty() || _api_secret.isEmpty())
        return;

    //build JSON payload
    //ex:
    /*
    {
        "client_id": UUID,
        "events": [{
            "name": eventCategory,
            "params": {
                "type": "eventName",
                "engagement_time_msec": 100 (default),
                "session_id": UUID,
                "os": "computer os"
            }
        }]
    }
    */

    // JSON Root
    QJsonObject jsonData = QJsonObject();

    // Defining Client Id
    QString clientId = GetCommonClientId().toString();
    jsonData["client_id"] = clientId;

    // Defining the Event
    QJsonObject event = QJsonObject();
    event["name"] = eventCategory;

    // Defining the Event Parameters
    // In order for user activity to display in standard reports like Realtime, engagement_time_msec and session_id must be supplied as part of the params for an event.
    QJsonObject eventParams = QJsonObject();
    eventParams["type"] = eventName;
    eventParams["engagement_time_msec"] = 100;
    eventParams["session_id"] = _sessionId.toString();
    #ifdef Q_OS_WIN
        eventParams["os"] = "Windows";
    #endif
    #ifdef Q_OS_LINUX
        eventParams["os"] = "Linux";
    #endif
    #ifdef Q_OS_MAC
        eventParams["os"] = "MAC";
    #endif

    // Attach Parameters to the Event
    event["params"] = eventParams;

    // Add the Event to the Events Array (as GA4 expects)
    QJsonArray events = QJsonArray();
    events += event;
    jsonData["events"] = events;

    // Sending the request
    // Convert JSON object into string
    QByteArray requestJson = QJsonDocument(jsonData).toJson();

    // Create the Request
    QNetworkRequest garequest = QNetworkRequest();
    garequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8");

    // Create and set the Google Analytics 4 Measurement Protocol URL
    QString requestUrl = "https://www.google-analytics.com/mp/collect?measurement_id=%1&api_secret=%2";
    requestUrl = requestUrl.arg(_measurementId).arg(_api_secret);
    garequest.setUrl(QUrl(requestUrl));

    // create custom temporary event loop on stack
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QObject::connect(&networkManager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    // the HTTP request
    QNetworkReply *reply = networkManager.post(garequest, requestJson);
    eventLoop.exec(); // blocks stack until "finished()" has been called
    if (reply->error() == QNetworkReply::NoError) {
        //success
        delete reply;
    }
    else {
        //failure
        qDebug() << "Google Analytics request failure: " << reply->errorString();
        delete reply;
    }
}

}
