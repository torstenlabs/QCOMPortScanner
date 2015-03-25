#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    checkTimer = new QTimer(this);
    checkTime = 5000;
    createActions();
    createTrayIcon();
    initConnection();
    searchCOMPorts(true);
    trayIcon->show();
    hide();
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
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/icon/icons/connector-icon.gif"));
}

void MainWindow::createActions()
{
    minimizeAction = new QAction(tr("Minimize"),this);
    connect(minimizeAction,SIGNAL(triggered()),this,SLOT(hide()));

    maximizeAction = new QAction(tr("Maximize"),this);
    connect(maximizeAction,SIGNAL(triggered()),this,SLOT(showMaximized()));

    restoreAction = new QAction(tr("Restore"),this);
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

void MainWindow::searchCOMPorts(bool firstrun)
{
    QStringList NewCOMPorts;
    QList<QTreeWidgetItem*> items;
    ui->treeWidget->clear();
    for(int i=0;i<QSerialPortInfo::availablePorts().count();i++)
    {
        QString portName=QSerialPortInfo::availablePorts().at(i).portName();
        QString portDescribtion=QSerialPortInfo::availablePorts().at(i).description();
        QString portManu=QSerialPortInfo::availablePorts().at(i).manufacturer();
        qDebug() << portName << ": " << portDescribtion << "," << portManu;
        QTreeWidgetItem *newItem=new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("%1").arg(portName)));
        newItem->addChild(new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("%1").arg(portDescribtion))));
        newItem->addChild(new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("%1").arg(portManu))));
        items.append(newItem);
        NewCOMPorts.append(portName);
        if(COMPorts.contains(portName)==false)
        {
            if(firstrun == false)
            {
                trayIcon->showMessage(tr("Detected %1").arg(portName),tr("%1: %2, %3").arg(portName).arg(portDescribtion).arg(portManu));
            }
        }
    }
    ui->treeWidget->insertTopLevelItems(0, items);
    COMPorts.clear();
    for(int y=0; y < NewCOMPorts.count();y++)
    {
        COMPorts.append(NewCOMPorts.at(y));
    }
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
       searchCOMPorts();
    }
}

void MainWindow::slotTimeout()
{
    this->searchCOMPorts();
}

void MainWindow::slotSingleScan()
{
    this->searchCOMPorts();
}

void MainWindow::slotStartContinuous()
{
    checkTimer->start(this->checkTime);
}

void MainWindow::slotStopContinuous()
{
    checkTimer->stop();
}
