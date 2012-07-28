#include "wiimote.h"
#include <QDebug>
#include <QTimer>
#include <QMetaType>

Q_DECLARE_METATYPE(Angles<BigDecimal>)

class _RegisterAnglesMetatype{
public:
    _RegisterAnglesMetatype() { qRegisterMetaType<Angles<BigDecimal> > (); }
};
static _RegisterAnglesMetatype registerAnglesMetatype;

Wiimote::Wiimote(QObject *parent) :
    QObject(parent), m_calibration_gyro(true), m_is_calibrating(false), m_calibrationTimer(new QTimer(this))
{
    m_calibrationTimer->setInterval(1000);
    connect(m_calibrationTimer, SIGNAL(timeout()), this, SLOT(calibrationFinished()));
}

Angles<BigDecimal> calculateDelta(QVector<WiimoteMessage> messages, Angles<BigDecimal> calibration) {
    Angles<BigDecimal> sum(BigDecimal(0), BigDecimal(0), BigDecimal(0));
    qint64 deltaTime = 0;
    foreach(WiimoteMessage message, messages) {
        BigDecimal yaw = BigDecimal(message.gyro().yaw());
        BigDecimal pitch = BigDecimal(message.gyro().pitch());
        BigDecimal roll = BigDecimal(message.gyro().roll());
        sum += Angles<BigDecimal>(yaw, pitch, roll);
        deltaTime += message.deltaTime();
    }

    Angles<BigDecimal> angleRates = sum/BigDecimal(messages.size());
    angleRates/=BigDecimal(1000);
//    qDebug() << "Angle rates: " << angleRates.toString();
//    qDebug() << "calibration: " << (calibration/BigDecimal(1000)).toString();
//    qDebug() << "Angle rates calibrated: " << (angleRates - calibration/BigDecimal(1000)).toString();

    BigDecimal deltaT = BigDecimal(deltaTime);
    deltaT /= BigDecimal(1000000000);

//    qDebug() << "deltaT: " << deltaT;

    Angles<BigDecimal> delta = angleRates.calculateDelta(deltaT, calibration/BigDecimal(1000));

//    qDebug() << "Delta-angle (with calibration): " << delta.toString();
    return delta;
}

void Wiimote::motionPlusEvent(WiimoteMessage message)
{
    if(m_is_calibrating) {
        calibrate(message);
        return;
    }
    if(m_calibration_gyro.isNull() ) {
        emit needsCalibration();
        return;
    }
    if(m_messagesBuffer.size()<5) {
        m_messagesBuffer << message;
        return;
    }
    Angles<BigDecimal> delta = calculateDelta(m_messagesBuffer, m_calibration_gyro);
    emit deltaChanged(delta);
    m_current_angles += delta;
    m_messagesBuffer.clear();
    qDebug() << "current angle: " << m_current_angles.toString();
    emit angleChanged(m_current_angles);
}

void Wiimote::calibrate(WiimoteMessage message)
{
    m_calibration_messages << message;
}

void Wiimote::calibrate()
{
    m_is_calibrating = true;
    m_calibrationStep = 0;
    m_calibrationTimer->start();
}

Angles<BigDecimal> getAverageData(const QVector<WiimoteMessage> &messages) {
    Angles<BigDecimal> gyro;
    quint64 gyro_valid_messages = 0;
    foreach(WiimoteMessage message, messages) {
        if(message.gyro().isValid()) {
            gyro += Angles<BigDecimal>(message.gyro().yaw(),message.gyro().pitch(),message.gyro().roll());
            gyro_valid_messages++;
        }
    }
    gyro /= BigDecimal(gyro_valid_messages);
    return gyro;
}

void Wiimote::calibrationFinished()
{

    if(m_calibrationStep < CALIBRATION_STEPS ) {
        m_calibrationStep++;
        emit(calibrationStep(CALIBRATION_STEPS - m_calibrationStep));
        return;
    }
    m_calibrationTimer->stop();
    quint64 messages_per_section = m_calibration_messages.size()/10;
    qDebug() << "Partitioning " << m_calibration_messages.size() << " messages in partition of " << messages_per_section;
    int startPos = 0;
    QMap<qint64, Angles<BigDecimal> >averages;
    while(startPos < m_calibration_messages.size()) {
        QVector<WiimoteMessage> currentMessages = m_calibration_messages.mid(startPos, messages_per_section);
        m_calibration_gyro =getAverageData(currentMessages);
        startPos += messages_per_section;
        averages.insert(currentMessages.last().deltaTime(), m_calibration_gyro);
    }

    foreach(qint64 deltaT, averages.keys()) {
        qDebug() << "on deltaT=" << deltaT << " average was " << averages.value(deltaT).toString();
    }

    m_calibration_gyro = getAverageData(m_calibration_messages);

    qDebug() << "zero for gyro =" << m_calibration_gyro.toString() << " (" << m_calibration_messages.size() << " reads)";
//    qDebug() << "zero for accel=" << m_calibration_accel.toString() << " (" << accel_valid_messages << " reads)";
    m_is_calibrating = false;
    m_calibration_messages.clear();
    emit calibrated();
}

void Wiimote::reset()
{
    reset(0,0,0);
}

void Wiimote::reset(double yaw, double pitch, double roll)
{
    reset(Angles<BigDecimal>(BigDecimal(yaw), BigDecimal(pitch), BigDecimal(roll)));
}


void Wiimote::reset(Angles<BigDecimal> angles)
{
    m_current_angles=angles;
}
