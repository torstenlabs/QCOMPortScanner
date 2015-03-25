#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QtGui>
#include <QSystemTrayIcon>
#include <QMainWindow>
#include <QMessageBox>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    void closeEvent(QCloseEvent *);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void slotTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void slotTimeout();
    void slotSingleScan();
    void slotStartContinuous();
    void slotStopContinuous();

private:
    void createTrayIcon();
    void createActions();
    void initConnection();
    void searchCOMPorts(bool firstrun = false);


    Ui::MainWindow *ui;
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *scanAction;
    QAction *startConScanAction;
    QAction *stopConScanAction;
    QAction *quitAction;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QTimer *checkTimer;
    QStringList COMPorts;
    qint32 checkTime;

};

#endif // MAINWINDOW_H
