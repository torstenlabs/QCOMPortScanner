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

    slotStopContinuous();
    searchCOMPorts(false);
    ui->statusBar->showMessage(QString(VERSION));
    trayIcon->show();
    readSettings();
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
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(scanAction);
    trayIconMenu->addAction(startConScanAction);
    trayIconMenu->addAction(stopConScanAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/icon/icons/connector-icon.gif"));
}

void MainWindow::createActions()
{
    restoreAction = new QAction(tr("Show Window"),this);
    connect(restoreAction,SIGNAL(triggered()),this,SLOT(showNormal()));

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

    for(int i=0;i<QSerialPortInfo::availablePorts().count();i++)
    {
        QString portName=QSerialPortInfo::availablePorts().at(i).portName();
        QString portDescribtion=QSerialPortInfo::availablePorts().at(i).description();
        QString portManu=QSerialPortInfo::availablePorts().at(i).manufacturer();
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
    checkTimeMS = settings.value("checkTimeMS",CHECKTIME_MS_DEFAULT).toInt();

}

void MainWindow::checkApplications()
{
    if(pathPutty != "")
       if(!QFile::exists(pathPutty))
            qDebug () << "Putty not found";

}

void MainWindow::slotTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::DoubleClick)
        if(this->isVisible())
            this->hide();
        else
            this->showNormal();

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
    if(scan)
    {
        trayIcon->setIcon(QIcon(":/icon/icons/connector-icon -progress.gif"));
        return ;
    }

    if(continous)
    {
        trayIcon->setIcon(QIcon(":/icon/icons/connector-icon -continous.gif"));
        return ;
    }

    trayIcon->setIcon(QIcon(":/icon/icons/connector-icon.gif"));

}
