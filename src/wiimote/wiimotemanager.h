#ifndef WIIMOTEMANAGER_H
#define WIIMOTEMANAGER_H

#include <QObject>
#include "angles.h"
#include "bigdecimal.h"

class Wiimote;
class CWiidConnectionWorker;
class WiimoteManager : public QObject
{
    Q_OBJECT
public:
    explicit WiimoteManager(QObject *parent = 0);
    ~WiimoteManager();
signals:
    void connected();
    void disconnected();
    void deviceMessage(const QString &message, int timeout);
    void angleChanged(Angles<BigDecimal> angles);
    void deltaChanged(Angles<BigDecimal> angles);
    void calibrationStep(int remainingSeconds);
    void calibrated();
    void needsCalibration();
public slots:
    void connectWiimote();
    void disconnectWiimote();
    void calibrateWiimote();
    void resetAngles();
    
private:
    CWiidConnectionWorker *worker;
    QThread *thread;
    Wiimote *wiimote;
};

#endif // WIIMOTEMANAGER_H
