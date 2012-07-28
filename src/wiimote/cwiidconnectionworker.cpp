#include "cwiidconnectionworker.h"
#include <QDebug>
#include <iostream>
#include "wiimote_messages.h"
#include <time.h>
#include "timeutils.h"
#include <QMap>


QMap<cwiid_wiimote_t *, CWiidConnectionWorker*> workers;
QMap<cwiid_wiimote_t *, timespec> lastMessageTime;


CWiidConnectionWorker::CWiidConnectionWorker(QObject *parent) :
    QObject(parent)
{
}

void CWiidConnectionWorker::emitMessage(WiimoteMessage message)
{
    emit wiimoteMessage(message);
}


#define curMessage mesg[message]

void cwiidCallback(cwiid_wiimote_t *wiimote, int mesg_count, cwiid_mesg mesg[], timespec *timestamp)
{
    if(!lastMessageTime.contains(wiimote)) {
        lastMessageTime[wiimote]=*timestamp;
        qDebug() << "Skipping first message";
        return;
    }
    CWiidConnectionWorker *worker = workers[wiimote];
    if(!worker) return;
    qint64 deltaTime = TimeUtils::nsecDifference(lastMessageTime[wiimote], *timestamp);
    lastMessageTime[wiimote]=*timestamp;

    WiimoteMessage wiimoteMessage(deltaTime);

    for(int message = 0; message<mesg_count; message++) {
        if(curMessage.type == CWIID_MESG_MOTIONPLUS) {
            Coordinates gyro(
              curMessage.motionplus_mesg.angle_rate[0],
              curMessage.motionplus_mesg.angle_rate[1],
              curMessage.motionplus_mesg.angle_rate[2]
              );
            wiimoteMessage.setGyro(gyro);
        }
        if(curMessage.type == CWIID_MESG_ACC) {
            Coordinates accel(
              curMessage.acc_mesg.acc[0],
              curMessage.acc_mesg.acc[1],
              curMessage.acc_mesg.acc[2]
              );
            wiimoteMessage.setAccel(accel);
        }
    }
    worker->emitMessage(wiimoteMessage);
}


void CWiidConnectionWorker::wiimoteDisconnect()
{
    if(cwiid_close(wiimote) == 0) {
        emit disconnected();
        workers.remove(wiimote);
        qDebug() << "Disconnected";
    }
}

void CWiidConnectionWorker::wiimoteConnect()
{
    bdaddr_t address = (bdaddr_t) {{0, 0, 0, 0, 0, 0}};
    qDebug() << "Trying to connect wiimote...";
    emit deviceMessage("Connecting to wiimote...", 0);
    wiimote =  cwiid_open(&address, CWIID_FLAG_CONTINUOUS);
    if(wiimote) {
        emit deviceMessage("Connected.", 5000);
        qDebug() << "Connected!";
        if(cwiid_command(wiimote, CWIID_CMD_LED, CWIID_LED1_ON)) qDebug() << "Error setting led1 on";
        if(cwiid_set_rpt_mode(wiimote, CWIID_RPT_MOTIONPLUS | CWIID_RPT_ACC)) qDebug() << "Error setting report for motionplus";
        if(cwiid_enable(wiimote, CWIID_FLAG_MESG_IFC)) qDebug() << "Error enabling messages";
        if(cwiid_enable(wiimote, CWIID_FLAG_MOTIONPLUS)) qDebug() << "Error enabling motionplus";
        cwiid_set_mesg_callback(wiimote, &cwiidCallback);
        if(cwiid_command(wiimote, CWIID_CMD_STATUS, 0)) qDebug() << "Error requesting status";
        workers.insert(wiimote, this);
        emit connected();
    } else {
        emit deviceMessage("Error connecting", 5000);
        emit disconnected();
    }
}



