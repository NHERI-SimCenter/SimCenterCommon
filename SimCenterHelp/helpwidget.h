#ifndef HELPWIDGET_H
#define HELPWIDGET_H

#include <QFrame>

namespace Ui {
class HelpWidget;
}

class HelpWidget : public QFrame
{
    Q_OBJECT

public:
    explicit HelpWidget(QWidget *parent = 0);
    ~HelpWidget();
    void setHelpResource(const QString &resFile);
    void lookup(const QString &lookfor);

private slots:
    void on_btn_goToIndex_clicked();
    void on_btn_goBack_clicked();
    void on_btn_goForward_clicked();
    void on_btn_OK_clicked();

    void on_searchTextBox_textEdited(const QString &arg1);

private:
    Ui::HelpWidget *ui;
    QString m_resourceName;
};

#endif // HELPWIDGET_H
