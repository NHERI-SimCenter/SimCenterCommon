#ifndef PYTHONPROGRESSDIALOG_H
#define PYTHONPROGRESSDIALOG_H

#include <QDialog>
#include <QMutex>

class QPlainTextEdit;
class QProgressBar;

class PythonProgressDialog : public QDialog
{
    Q_OBJECT

private:
    explicit PythonProgressDialog(QWidget* parent =0);
    ~PythonProgressDialog();
    static PythonProgressDialog *theInstance;  
  
public:
    static PythonProgressDialog *getInstance(QWidget *parent = 0);  

    void appendText(const QString text);

    void appendInfoMessage(const QString text);

    void appendErrorMessage(const QString text);

    void clear(void);

    void setVisibility(bool visible);

    void setProgressBarValue(const int val);
    void setProgressBarRange(const int start,const int end);

    void hideAfterElapsedTime(int sec);


public slots:
    void showProgressBar(void);
    void hideProgressBar(void);
    void handleCloseButtonPress();
    void handleClearButtonPress();

signals:
    void processResults(QString);

private:

    QPlainTextEdit* progressTextEdit;

    QString cleanUpText(const QString text);

    QProgressBar* progressBar;

    QMutex* mutex;

};

#endif // PYTHONPROGRESSDIALOG_H
