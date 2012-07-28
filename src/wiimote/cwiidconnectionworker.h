#ifndef CWIIDCONNECTIONWORKER_H
#define CWIIDCONNECTIONWORKER_H
#include "cwiid.h"
#include "wiimote_messages.h"
#include <QObject>

class CWiidConnectionWorker : public QObject
{
    Q_OBJECT
public:
    explicit CWiidConnectionWorker(QObject *parent = 0);
    void emitMessage(WiimoteMessage message);
signals:
    void connected();
    void disconnected();
    void wiimoteMessage(WiimoteMessage message);
    void deviceMessage(const QString &message, int timeout);
public slots:
    void wiimoteConnect();
    void wiimoteDisconnect();
private slots:
private:
    cwiid_wiimote_t *wiimote;
    
};

#endif // CWIIDCONNECTIONWORKER_H
