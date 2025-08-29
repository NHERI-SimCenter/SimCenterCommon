#ifndef ProgramOutputDialog_H
#define ProgramOutputDialog_H

#include <QDialog>


class QRecursiveMutex;
class QPlainTextEdit;
class QProgressBar;
class QLabel;
class QTimer;
class QElapsedTimer;

class ProgramOutputDialog : public QDialog
{
    Q_OBJECT

private:
    explicit ProgramOutputDialog(QWidget* parent =0);
    ~ProgramOutputDialog();
    static ProgramOutputDialog *theInstance;
  
public:
    static ProgramOutputDialog *getInstance(QWidget *parent = 0);

    void appendText(const QString text);

    void appendInfoMessage(const QString text);

    void appendErrorMessage(const QString text);

    void fatalMessage(const QString text);  

    void appendBlankLine();

    void setVisibility(bool visible);

    void setProgressBarValue(const int val);
    void setProgressBarRange(const int start,const int end);

    void hideAfterElapsedTime(int sec);

    QProgressBar *getProgressBar() const;

public slots:
    void showProgressBar(void);
    void hideProgressBar(void);
    void handleCloseButtonPress();
    void handleClearButtonPress();
    void clear(void);
    void updateTimerLabel();

signals:
    void showDialog(bool);

private:

    QPlainTextEdit* progressTextEdit = nullptr;

    QString cleanUpText(const QString text);

    QProgressBar* progressBar = nullptr;

    QRecursiveMutex* mutex = nullptr;

    QString getTimestamp(void);

    QLabel* timerText = nullptr;
    QLabel* timerLabel = nullptr;

    QTimer* timer = nullptr;
    QElapsedTimer* elapsedTimer = nullptr;

};

#endif // ProgramOutputDialog_H
