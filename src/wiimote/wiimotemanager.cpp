#include "wiimotemanager.h"
#include "wiimote.h"
#include "cwiidconnectionworker.h"
#include <QThread>

WiimoteManager::WiimoteManager(QObject *parent) :
    QObject(parent), wiimote(new Wiimote(this))
{
    worker = new CWiidConnectionWorker();
    thread = new QThread(this);
    worker->moveToThread(thread);
    thread->start();
    connect(worker, SIGNAL(wiimoteMessage(WiimoteMessage)), wiimote, SLOT(motionPlusEvent(WiimoteMessage)));
    connect(worker, SIGNAL(connected()), this, SIGNAL(connected()));
    connect(worker, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    connect(worker, SIGNAL(deviceMessage(QString,int)), this, SIGNAL(deviceMessage(QString,int)));
    connect(wiimote, SIGNAL(angleChanged(Angles<BigDecimal>)), this, SIGNAL(angleChanged(Angles<BigDecimal>)));
    connect(wiimote, SIGNAL(deltaChanged(Angles<BigDecimal>)), this, SIGNAL(deltaChanged(Angles<BigDecimal>)));
    connect(wiimote, SIGNAL(calibrationStep(int)), this, SIGNAL(calibrationStep(int)));
    connect(wiimote, SIGNAL(calibrated()), this, SIGNAL(calibrated()));
    connect(wiimote, SIGNAL(needsCalibration()), this, SIGNAL(needsCalibration()));
}

WiimoteManager::~WiimoteManager()
{
    delete worker;
    thread->quit();
    thread->wait();
    thread->terminate();
    delete thread;
}


void WiimoteManager::connectWiimote()
{
    QMetaObject::invokeMethod(worker, "wiimoteConnect", Qt::QueuedConnection);
}

void WiimoteManager::disconnectWiimote()
{
    QMetaObject::invokeMethod(worker, "wiimoteDisconnect", Qt::QueuedConnection);
}

void WiimoteManager::calibrateWiimote()
{
    wiimote->calibrate();
}

void WiimoteManager::resetAngles()
{
    wiimote->reset();
}
