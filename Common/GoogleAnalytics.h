#ifndef GOOGLEANALYTICS_H
#define GOOGLEANALYTICS_H
#include <QUuid>
#include <QtNetwork/QNetworkAccessManager>
#include <QDateTime>

namespace GoogleAnalytics {
enum SessionControl {Start, End, None};

void Report(QString eventCategory, QString eventName = "Action");
void ReportStart();
void ReportLocalRun();
void ReportDesignSafeRun();
void ReportAppUsage(QString appName);
void ReportExample(QString exampleName);
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
static qint64 _session_start_time;
}
#endif // GOOGLEANALYTICS_H
