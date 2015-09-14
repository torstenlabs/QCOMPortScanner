#ifndef MANAGECOMPORTS_H
#define MANAGECOMPORTS_H

#include <QObject>
#include <QStringList>

class ManageComPorts : public QObject
{
    Q_OBJECT
public:
    explicit ManageComPorts(QObject *parent = 0);
    int count();

signals:

public slots:

private:


};

#endif // MANAGECOMPORTS_H
