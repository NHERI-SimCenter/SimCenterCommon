#ifndef GOOGLEANALYTICS_H
#define GOOGLEANALYTICS_H
#include <QUuid>
#include <QtNetwork/QNetworkAccessManager>

namespace GoogleAnalytics {
enum SessionControl {Start, End, None};

void Report(QString eventCategory, QString eventName = "Action");
void ReportStart();
void ReportLocalRun();
void ReportDesignSafeRun();
void ReportAppUsage(QString appName);  
void StartSession();
void EndSession();
QUuid GetCommonClientId();
void SetMeasurementId(QString measurementId);
void SetAPISecret(QString api_secret);
void CreateSessionId();

static QString _measurementId = "";
static QNetworkAccessManager networkManager;
static QUuid _sessionId;
static QString _api_secret;
}
#endif // GOOGLEANALYTICS_H
