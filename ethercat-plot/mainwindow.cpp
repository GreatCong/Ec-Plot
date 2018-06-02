#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore>
#include <QMessageBox>

#include "ethercat.h"
#include "soem_main.h"
#include "my_queue.h"

/* variate */
void* ec_thread_main(void* arg);
pthread_t pthread_ecMain;
//QString ec_adaptert_name;
//int cycle_count = 100;
/* variate end*/

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    this->resize( QSize( 800, 600 ));//实现重绘窗口
//    ui->customPlot_ecPlot->resize(500,400);
    Init_cores();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Init_cores(void){

    statusBar_Label_ecSlavesNum = new QLabel(this);
//    statusBar_Label_ecSlavesNum->setFrameShape(QFrame::WinPanel);
//    statusBar_Label_ecSlavesNum->setFrameShadow(QFrame::Sunken);
    ui->statusBar->addWidget(statusBar_Label_ecSlavesNum);//temp, at the left of StatusBar
    statusBar_Label_ecSlavesNum->setText(QString::fromLocal8Bit("欢迎使用！！！"));//status bar无法通过设计器添加信息

    statusBar_Label_test = new QLabel(this);
    ui->statusBar->addWidget(statusBar_Label_test);
    statusBar_Label_test->setText("0");
    statusBar_Label_test->setFrameShape(QFrame::WinPanel);
    statusBar_Label_test->setFrameShadow(QFrame::Sunken);

    statusBar_Label_permanent = new QLabel;
//    statusBar_Label_permanent->setFrameStyle(QFrame::Box | QFrame::Sunken);
//    statusBar_Label_permanent->setText(tr("<a href=\"http://www.baidu.com\">baidu.com</a>"));
//    statusBar_Label_permanent->setTextFormat(Qt::RichText);     //设置为超文本
//    statusBar_Label_permanent->setOpenExternalLinks(true);      //开启自动打开超链接
    statusBar_Label_permanent->setText("Ethercat plot by lcj");
    ui->statusBar->addPermanentWidget(statusBar_Label_permanent);//permanet, at the right of StatusBar
    ui->statusBar->setStyleSheet("color:blue");
//    ui->statusBar->setStyleSheet("background-color:rgb(64,224,205)");

    Plot_init(ui->customPlot_ecPlot);

    my_plot_timer = new QTimer(this);    //新建定时器

    connect(my_plot_timer,SIGNAL(timeout()),this,SLOT(timerUpDate()));//关联定时器计满信号和相应的槽函数。

//    my_plot_timer->start(100);    //定时器开始计时，其中1000表示1000ms即1s,每1000ms就会触发一次槽函数。

    my_ec_initCheckThread();

}


void* ec_thread_main(void* arg){
    QString* str = (QString*)arg;

    //QByteArray str_array_temp = ec_adaptert_name.toLocal8Bit();
     QByteArray str_array_temp = str->toLocal8Bit();
    char* ch =str_array_temp.data();

    printf("SOEM (Simple Open EtherCAT Master)\nSimple test\n");

    //soem_main(ch,cycle_count,true);
    Is_ecRun = true;
    initQueue(&my_ecQueue.my_ecQueue_x);
    initQueue(&my_ecQueue.my_ecQueue_y);
    initQueue(&my_ecQueue.my_ecQueue_z);
    my_ec_test(ch);
    clearQueue(&my_ecQueue.my_ecQueue_x);
    clearQueue(&my_ecQueue.my_ecQueue_y);
    clearQueue(&my_ecQueue.my_ecQueue_z);
    printf("End program\n");

    return NULL;
}


void MainWindow::on_pushButton_ecRun_clicked()
{
    int ec_adaptert_count = ui->comboBox_ecadaptert->count();
    if(ec_adaptert_count < 1){
       QMessageBox::warning(this, tr("warning"), tr("ec_adaptert_count=0!"));
    }
    else{

      if(!my_plot_timer->isActive()){
          my_plot_timer->start(50);
      }

      QString str = ui->comboBox_ecadaptert->currentText();

//      const char* ch = str.toStdString().c_str();//convert to const char*
//      QByteArray str_array_temp = str.toLocal8Bit();
//      char* ch =str_array_temp.data();
//      simpletest(ch);
//        soem_main(ch,100,true);
//       ec_adaptert_name = str;
       pthread_attr_t       attr;

       pthread_attr_init(&attr);
       pthread_attr_setstacksize(&attr, 128000);
//       cycle_count = ui->lineEdit_ecCycle->text().toInt();

       pthread_create(&pthread_ecMain, &attr, ec_thread_main, &str);
//      printf("End program\n");
    }

}

void MainWindow::timerUpDate()
{
    static int ii;
//       QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间

//       QString str = time.toString("yyyy-MM-dd hh:mm:ss dddd");//设置系统时间显示格式

//       ui->label->setText(str);//在标签上显示时间
       QString str = tr("find ")+QString::number(ec_slavecount)+tr(" slaves");
       statusBar_Label_ecSlavesNum->setText(str);
       Is_ecPrintToConsole = ui->checkBox_isPrintToConsole->isChecked();

//       ii+=1;

       if(ii>1023){
               ii = 0;
//               ui->customPlot_ecPlot->graph(0)->setData(xx,yy);
                 QVector<double> x1(1),y1(1);
                 QVector<double> x2(1),y2(1);
                 QVector<double> x3(1),y3(1);
                 ui->customPlot_ecPlot->graph(0)->data().clear();
                 ui->customPlot_ecPlot->graph(1)->data().clear();
                 ui->customPlot_ecPlot->graph(2)->data().clear();
                 if(!emptyQueue(&my_ecQueue.my_ecQueue_x)){
                     y1[0] = outQueue(&my_ecQueue.my_ecQueue_x);
                     y2[0] = outQueue(&my_ecQueue.my_ecQueue_y);
                     y3[0] = outQueue(&my_ecQueue.my_ecQueue_z);
                 }
                 ui->customPlot_ecPlot->graph(0)->setData(x1,y1);//不用setData清除不掉，但是会出现一个问题，第一个数字是0
                 ui->customPlot_ecPlot->graph(1)->setData(x2,y2);//不用setData清除不掉
                 ui->customPlot_ecPlot->graph(2)->setData(x3,y3);//不用setData清除不掉
//                 ui->customPlot_ecPlot->repaint();
//                 ui->customPlot_ecPlot->replot();
           }

           statusBar_Label_test->setText(QString::number(ii));

            if(!emptyQueue(&my_ecQueue.my_ecQueue_x)){
               while(!emptyQueue(&my_ecQueue.my_ecQueue_x)){
                    ui->customPlot_ecPlot->graph(0)->addData(ii++,outQueue(&my_ecQueue.my_ecQueue_x));
                    ui->customPlot_ecPlot->graph(1)->addData(ii++,outQueue(&my_ecQueue.my_ecQueue_y));
                    ui->customPlot_ecPlot->graph(2)->addData(ii++,outQueue(&my_ecQueue.my_ecQueue_z));
                    if(ii>1023)
                        break;
                }
            }

        //自动设定graph(1)曲线y轴的范围，如果不设定，有可能看不到图像
       //也可以用ui->customPlot->yAxis->setRange(up,low)手动设定y轴范围
//           ui->customPlot_ecPlot->graph(0)->rescaleValueAxis();
          ui->customPlot_ecPlot->xAxis->setRange(-10, 1024);
           //这里的8，是指横坐标时间宽度为8秒，如果想要横坐标显示更多的时间
           //就把8调整为比较大到值，比如要显示60秒，那就改成60。
           //这时removeDataBefore(key-8)中的8也要改成60，否则曲线显示不完整。
           //ui->qCustomPlot->xAxis->setRange(key+0.25, 8, Qt::AlignRight);//设定x轴的范围
           ui->customPlot_ecPlot->replot();
}

void MainWindow::Plot_init(QCustomPlot *customPlot){
//    // CustomPlot的基础功能设置
//    m_CustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iMultiSelect  | QCP::iSelectOther | QCP::iSelectItems);
//    // 基础功能共有以下几种，大体意思是：
//    // 1、轴可拖动     2、通过鼠标滚轮改变轴显示范围 3、用户可以选择多个对象,设定的修饰符（不是特别明白）
//    // 4、图形是可选的  5、轴是可选的 6、图例是可选的。。。
//    /*enum Interaction { iRangeDrag         = 0x001 ///< <tt>0x001</tt> Axis ranges are draggable (see \ref QCPAxisRect::setRangeDrag, \ref QCPAxisRect::setRangeDragAxes)
//                         ,iRangeZoom        = 0x002 ///< <tt>0x002</tt> Axis ranges are zoomable with the mouse wheel (see \ref QCPAxisRect::setRangeZoom, \ref QCPAxisRect::setRangeZoomAxes)
//                         ,iMultiSelect      = 0x004 ///< <tt>0x004</tt> The user can select multiple objects by holding the modifier set by \ref QCustomPlot::setMultiSelectModifier while clicking
//                         ,iSelectPlottables = 0x008 ///< <tt>0x008</tt> Plottables are selectable (e.g. graphs, curves, bars,... see QCPAbstractPlottable)
//                         ,iSelectAxes       = 0x010 ///< <tt>0x010</tt> Axes are selectable (or parts of them, see QCPAxis::setSelectableParts)
//                         ,iSelectLegend     = 0x020 ///< <tt>0x020</tt> Legends are selectable (or their child items, see QCPLegend::setSelectableParts)
//                         ,iSelectItems      = 0x040 ///< <tt>0x040</tt> Items are selectable (Rectangles, Arrows, Textitems, etc. see \ref QCPAbstractItem)
//                         ,iSelectOther      = 0x080 ///< <tt>0x080</tt> All other objects are selectable (e.g. your own derived layerables, the plot title,...)
//                     };*/
//    // 设置矩形边框
//    m_CustomPlot->axisRect()->setupFullAxesBox();
//    // 清空CustomPlot中的图形
//    m_CustomPlot->clearGraphs();
//    // 在CustomPlot中添加图形
//    m_CustomPlot->addGraph();
//    // 设置图形中的数据m_x和m_y是两个QVector容器
//    m_CustomPlot->graph(0)->setData(m_x, m_y);
//    // 这个是设置图形显示为合适范围（感觉设置的只是Y轴）
//    m_CustomPlot->graph(0)->rescaleValueAxis(true);
//    // 设置X轴的显示范围（这里是4条轴，x是下面那条，x2是上面那条，Y是先左后右）
//    m_CustomPlot->xAxis->setRange(m_x.at(0) - 1, m_x.at(m_x.size() - 1) + 1 );
//    // 刷新m_CustomPlot中数据
//    m_CustomPlot->replot();

    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iMultiSelect  | QCP::iSelectOther | QCP::iSelectItems);
    //设定背景为黑色
    //customPlot->setBackground(QBrush(Qt::black));
     //设定右上角图形标注可见
    customPlot->legend->setVisible(true);
    //设定右上角图形标注的字体
    customPlot->legend->setFont(QFont("Helvetica", 9));
    //向绘图区域QCustomPlot(从widget提升来的)添加一条曲线
    customPlot->addGraph();
    //设置坐标轴标签名称
    customPlot->xAxis->setLabel("x");
    customPlot->yAxis->setLabel("y");
    //customPlot->xAxis->setVisible(false);//设置没有用？？
    //设置右上角图形标注名称
    customPlot->graph(0)->setName(QString::fromLocal8Bit("X轴"));//QString::fromLocal8Bit为了避免中文乱码

    customPlot->addGraph();
    customPlot->graph(1)->setName(QString::fromLocal8Bit("Y轴"));
    customPlot->graph(1)->setPen(QPen(Qt::red));
    customPlot->addGraph();
    customPlot->graph(2)->setPen(QPen(Qt::green));
    customPlot->graph(2)->setName(QString::fromLocal8Bit("Z轴"));

    customPlot->axisRect()->setupFullAxesBox();

    //定义两个可变数组存放绘图的坐标数据
//    QVector<double> x(101),y(101);//分别存放x和y坐标的数据,101为数据长度
//    //添加数据，我们这里演示y=x^3,为了正负对称，我们x从-10到+10
//    for(int i=0;i<101;i++)
//    {
//        x[i] = i/5 - 10;
//        y[i] = x[i] * x[i] * x[i];
//    }
    //添加数据
//    customPlot->graph(0)->setData(x,y);

    //设置坐标轴显示范围,否则我们只能看到默认的范围
//    customPlot->xAxis->setRange(-11,11);
//    customPlot->yAxis->setRange(-1100,1100);

    //重绘，这里可以不用，官方例子有，执行setData函数后自动重绘
    //我认为应该用于动态显示或者是改变坐标轴范围之后的动态显示，我们以后探索
    //ui->qCustomPlot->replot();

}

void MainWindow::on_pushButton_ecStop_clicked()
{
    Is_ecRun = false;
//    static bool xx= true;
//    if(xx){
//    this->setWindowFlags(Qt::FramelessWindowHint);
//    this->show();
//        xx = false;
//    }else{
//        xx =true;
//        this->setWindowFlags(0);//设置为0即可清除相关配置
//        this->show();
//    }
}


void MainWindow::on_action_ScanAdapyert_triggered()
{
    ui->comboBox_ecadaptert->clear();

    ec_adaptert * adapter = NULL;

//     printf ("Available adapters\n");
     adapter = ec_find_adapters ();
     while (adapter != NULL)
     {
//        printf ("Description : %s, Device to use for wpcap: %s\n", adapter->desc,adapter->name);
         ui->comboBox_ecadaptert->addItem(adapter->name);
        adapter = adapter->next;
     }
}

void MainWindow::on_action_PlotAdapt_zoom_triggered()
{
     ui->customPlot_ecPlot->graph(0)->rescaleValueAxis();//自动设定坐标范围
     ui->customPlot_ecPlot->graph(1)->rescaleValueAxis();//自动设定坐标范围
     ui->customPlot_ecPlot->graph(2)->rescaleValueAxis();//自动设定坐标范围
}

void MainWindow::on_pushButton_ecSendProcessData_clicked()
{
    QString str[4];

    str[0] = ui->lineEdit_ecCMD1->text();
    str[1] = ui->lineEdit_ecCMD2->text();
    str[2] = ui->lineEdit_ecCMD3->text();
    str[3] = ui->lineEdit_ecCMD4->text();

    my_CMD[0] = str[0].toInt();
    my_CMD[1] = str[1].toInt();
    my_CMD[2] = str[2].toInt();
    my_CMD[3] = str[3].toInt();

}

void MainWindow::on_action_Scan_Slaves_triggered()
{
    int ec_adaptert_count = ui->comboBox_ecadaptert->count();
    if(ec_adaptert_count < 1){
       //QMessageBox::warning(this, tr("warning"), tr("ec_adaptert_count=0!"));
        ui->statusBar->showMessage(tr("Error!! Please Scan Adaptert first!"),2000);
    }
    else{

      if(!my_plot_timer->isActive()){
          my_plot_timer->start(50);
      }

      QString str = ui->comboBox_ecadaptert->currentText();

      QByteArray str_array_temp = str.toLocal8Bit();
      char* ch =str_array_temp.data();

      bool is_print_sdo = ui->checkBox_isPrintSDO->isChecked();
      bool is_print_map = ui->checkBox_isPrintMAP->isChecked();
//      printf("%d,%d\n",is_print_sdo,is_print_map);

      my_ec_slaveinfo(ch,is_print_sdo,is_print_map);

    }

}
