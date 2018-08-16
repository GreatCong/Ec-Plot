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
    void timerUpDate();

private slots:
    void on_pushButton_ecRun_clicked();

    void on_pushButton_ecStop_clicked();

    void on_action_ScanAdapyert_triggered();

    void on_pushButton_ecSendProcessData_clicked();

    void on_action_Scan_Slaves_triggered();

    void on_comboBox_ecadaptert_currentIndexChanged(int index);

    void on_comboBox_ecadaptert_dec_currentIndexChanged(int index);

    void on_action_PlotAdapt_zoom_triggered();

    void on_action_Adapt_zoom1_triggered();

    void on_action_Adapt_zoom2_triggered();

    void on_action_Adapt_zoom3_triggered();

    void on_action_windowsMax_triggered();

    void on_action_Adapt_zoom4_triggered();

    void on_pushButton_ecScan_clicked();

    void on_action_ecSend_triggered();

    void on_checkBox_isPlotShowLegend_stateChanged(int arg1);

    void on_pushButton_debug_clear_clicked();

    void on_pushButton_ecScan_Adaptert_clicked();

    void on_actionExit_triggered();

private:
    Ui::MainWindow *ui;

    void Plot_init(QCustomPlot *customPlot);
    void Init_cores(void);
    void Init_userUI(void);
    void Destroy_cores(void);
    int ec_ScanAdapyert_ui(void);

    QLabel *statusBar_Label_ecSlavesNum;
    QLabel *statusBar_Label_permanent;
    QLabel *statusBar_Label_test;
    QTimer *my_plot_timer;
};

#endif // MAINWINDOW_H
