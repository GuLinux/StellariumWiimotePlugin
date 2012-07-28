#ifndef WIIMOTE_H
#define WIIMOTE_H

#include <QObject>
#include <QVector>
#include <QVector3D>
#include "wiimote_messages.h"
#include "angles.h"
#include "bigdecimal.h"

#define CALIBRATION_STEPS 30

class QTimer;
class Wiimote : public QObject
{
    Q_OBJECT
public:
    explicit Wiimote(QObject *parent = 0);
signals:
    void angleChanged(Angles<BigDecimal> angles);
    void deltaChanged(Angles<BigDecimal> angles);
    void calibrationStep(int remainingSeconds);
    void calibrated();
    void needsCalibration();

public slots:
    void motionPlusEvent(WiimoteMessage message);
    void calibrate(WiimoteMessage message);
    void calibrate();
    void calibrationFinished();
    void reset();
    void reset(double yaw, double pitch, double roll);
    void reset(Angles<BigDecimal> angles);

private:
    Angles<BigDecimal>  m_calibration_gyro;
    Angles<BigDecimal>  m_current_angles;
    QVector<WiimoteMessage> m_calibration_messages;
    QVector<WiimoteMessage> m_messagesBuffer;
    QDateTime m_lastUpdated;
    bool m_is_calibrating;
    int m_calibrationStep;
    QTimer *m_calibrationTimer;
};

#endif // WIIMOTE_H
