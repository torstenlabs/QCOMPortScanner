#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "global.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setWindowTitle(APP_NAME);
    ui->setupUi(this);
    checkTimer = new QTimer(this);
    createActions();
    createTrayIcon();

    initConnection();

    searchCOMPorts(false);
    ui->statusBar->showMessage(QString(VERSION));
    trayIcon->show();
    readSettings();


    if(continousScan)
        slotStartContinuous();
    else
        slotStopContinuous();


    checkApplications();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
if(trayIcon->isVisible())
    {
        hide();
        event->ignore();
    }
}

void MainWindow::createTrayIcon()
{
    trayIconMenu->clear();

    trayIconMenu->addAction(scanAction);
    trayIconMenu->addAction(startConScanAction);
    trayIconMenu->addAction(stopConScanAction);
    trayIconMenu->addSeparator();

    foreach(QAction *newAction, trayMenuActions)
    {
        trayIconMenu->addAction(newAction);
    }

    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon->setContextMenu(trayIconMenu);
}

void MainWindow::createActions()
{
    trayIconMenu = new QMenu(this);
    trayIcon = new QSystemTrayIcon(this);

    quitAction = new QAction(tr("Quit"),this);
    connect(quitAction,SIGNAL(triggered()),qApp,SLOT(quit()));

    scanAction = new QAction(tr("Scan"),this);
    connect(scanAction,SIGNAL(triggered()),this,SLOT(slotSingleScan()));

    startConScanAction = new QAction(tr("Start Continuous Scan"),this);
    connect(startConScanAction,SIGNAL(triggered()),this,SLOT(slotStartContinuous()));

    stopConScanAction = new QAction(tr("Stop Continuous Scan"),this);
    connect(stopConScanAction,SIGNAL(triggered()),this,SLOT(slotStopContinuous()));
}

void MainWindow::initConnection()
{ 
    connect(this->ui->actionExit_2,SIGNAL(triggered()),qApp,SLOT(quit()));
    connect(this->ui->actionClose_to_Tray,SIGNAL(triggered()),this,SLOT(hide()));
    connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(slotTrayIconActivated(QSystemTrayIcon::ActivationReason)));
    connect(this->checkTimer,SIGNAL(timeout()),this,SLOT(slotTimeout()));
    connect(this->ui->actionContinious_Scan,SIGNAL(triggered()),this,SLOT(slotStartContinuous()));
    connect(this->ui->actionStop_Continious_Scan,SIGNAL(triggered()),this,SLOT(slotStopContinuous()));
    connect(this->ui->actionScan,SIGNAL(triggered()),this,SLOT(slotSingleScan()));
}

void MainWindow::searchCOMPorts(bool continous)
{
    QStringList NewCOMPorts;
    QList<QTreeWidgetItem*> items;
    bool firstrun = false;

    if(COMPorts.count() == 0)
        firstrun = true;

    this->slotSetTrayIcon(true,continous);

    ui->treeWidget->clear();

    foreach(QAction *newAction, trayMenuActions)
    {
        disconnect(newAction,SIGNAL(triggered(bool)));
        newAction->deleteLater();
    }

    trayMenuActions.clear();

    for(int i=0;i<QSerialPortInfo::availablePorts().count();i++)
    {
        QString portName=QSerialPortInfo::availablePorts().at(i).portName();
        QString portDescribtion=QSerialPortInfo::availablePorts().at(i).description();
        QString portManu=QSerialPortInfo::availablePorts().at(i).manufacturer();

        QAction *newAction = new QAction(portName,this);
        connect(newAction,SIGNAL(triggered(bool)),this,SLOT(slotActionClicked(bool)));
        trayMenuActions.append(newAction);


        QTreeWidgetItem *newItem=new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("%1").arg(portName)));
        newItem->addChild(new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("%1").arg(portDescribtion))));
        newItem->addChild(new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("%1").arg(portManu))));
        items.append(newItem);
        NewCOMPorts.append(portName);

        if(COMPorts.contains(portName)==false)
        {
            if(firstrun == false)
            {
                trayIcon->showMessage(APP_NAME,tr("Found %1: %2, %3").arg(portName).arg(portDescribtion).arg(portManu));
            }
        }
    }

    ui->treeWidget->insertTopLevelItems(0, items);

    foreach(QString newPort, NewCOMPorts)
    {
        int element =0;
        foreach(QString oldPort, COMPorts)
        {

            if(newPort == oldPort)
            {
                COMPorts.removeAt(element);
            }
            element++;
        }
    }

    if(COMPorts.count() != 0)
    {
        foreach(QString oldPort, COMPorts)
            trayIcon->showMessage(APP_NAME,tr("Removed %1").arg(oldPort));
    }


    COMPorts.clear();

    for(int y=0; y < NewCOMPorts.count();y++)
    {
        COMPorts.append(NewCOMPorts.at(y));
    }



    createTrayIcon();
    this->slotSetTrayIcon(false,continous);
}

void MainWindow::readSettings()
{
    QSettings settings(CONFIG_FILE,QSettings::IniFormat);

    if(!QFile::exists(CONFIG_FILE))
    {
        trayIcon->showMessage(APP_NAME,tr("No Configuration available"));
    }

    pathPutty = settings.value("pathPutty","").toString();
    defaultSerialConfig = settings.value("defaultSerialConfig","115200,8,n,1,N").toString();
    checkTimeMS = settings.value("checkTimeMS",CHECKTIME_MS_DEFAULT).toInt();
    continousScan = settings.value("continousScan","false").toBool();
}

void MainWindow::checkApplications()
{
    if(pathPutty != "")
       if(!QFile::exists(pathPutty))
            qDebug () << "Putty not found";

}

void MainWindow::slotTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
   /* if(reason == QSystemTrayIcon::DoubleClick)
        if(this->isVisible())
            this->hide();
        else
            this->showNormal();
*/
    if(reason == QSystemTrayIcon::Trigger)
    {
       slotSingleScan();
    }
}

void MainWindow::slotTimeout()
{
    this->searchCOMPorts(true);
}

void MainWindow::slotSingleScan()
{
    this->searchCOMPorts(false);
}

void MainWindow::slotStartContinuous()
{
    this->slotSetTrayIcon(false,true);
    checkTimer->start(checkTimeMS);
}

void MainWindow::slotStopContinuous()
{
    this->slotSetTrayIcon(false,false);
    checkTimer->stop();
}

void MainWindow::slotSetTrayIcon(bool scan, bool continous)
{
    QPixmap myIcon;

    myIcon.load(":/icon/icons/connector-icon.gif");

    if(continous)
        myIcon.load(":/icon/icons/connector-icon -continous.gif");

    if(scan)
        myIcon.load(":/icon/icons/connector-icon -progress.gif");

    if(continous)
    {
        QPainter painter(&myIcon);
        painter.setFont(QFont("Arial",40));
        painter.setPen(QPen(Qt::black, 3));
        painter.setBrush(Qt::SolidPattern);
        painter.drawRoundRect(QRect(15,15,myIcon.width()-30,myIcon.height()-30));
        painter.setPen(QPen(QColor(255,128,0),10));

        if(scan)
            painter.setPen(QPen(Qt::green,10));

        painter.drawText(QRect(10,10,myIcon.width()-20,myIcon.height()-20),Qt::AlignCenter,QString("%1").arg(checkTimeMS/1000));
    }

    trayIcon->setIcon(QIcon(myIcon));
}

void MainWindow::slotActionClicked(bool checked)
{
    Q_UNUSED(checked);
    QAction *senderAction = dynamic_cast<QAction*>(QObject::sender());
    QProcess putty;
    putty.startDetached(pathPutty,QStringList() << "-serial" << senderAction->text() << "-sercfg" << defaultSerialConfig);
}
