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
#include <QSettings>

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
    void slotSetTrayIcon(bool scan, bool continous);

private:
    void createTrayIcon();
    void createActions();
    void initConnection();
    void searchCOMPorts(bool continous);
    void readSettings();


    Ui::MainWindow *ui;
    QAction *restoreAction;
    QAction *scanAction;
    QAction *startConScanAction;
    QAction *stopConScanAction;
    QAction *quitAction;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QTimer *checkTimer;
    QStringList COMPorts;
    qint32 checkTimeMS;

};

#endif // MAINWINDOW_H
