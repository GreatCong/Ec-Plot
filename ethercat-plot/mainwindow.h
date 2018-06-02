#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "qcustomplot.h"
#include <QLabel>
#include <QTime>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_pushButton_ecRun_clicked();

    void timerUpDate();

    void on_pushButton_ecStop_clicked();

    void on_action_ScanAdapyert_triggered();

    void on_action_PlotAdapt_zoom_triggered();

    void on_pushButton_ecSendProcessData_clicked();

    void on_action_Scan_Slaves_triggered();

private:
    Ui::MainWindow *ui;

    void Plot_init(QCustomPlot *customPlot);
    void Init_cores(void);

    QLabel *statusBar_Label_ecSlavesNum;
    QLabel *statusBar_Label_permanent;
    QLabel *statusBar_Label_test;
    QTimer *my_plot_timer;
};

#endif // MAINWINDOW_H
