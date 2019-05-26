#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_selectionView_clicked(const QModelIndex &index);
    void on_btn_option1_clicked();
    void on_btn_option2_clicked();
    void on_btn_option3_clicked();

private:
    Ui::MainWindow *ui;
    int currentLegend   = 0;
    int currentLocation = 0;
};

#endif // MAINWINDOW_H
