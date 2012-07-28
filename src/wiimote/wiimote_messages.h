#ifndef WIIMOTE_MESSAGES_H
#define WIIMOTE_MESSAGES_H

#include <Qt>
#include <QMetaType>
#include <QDateTime>


class Coordinates {
public:
    Coordinates();
    Coordinates(quint16 yawRate, quint16 pitchRate, quint16 rollRate);
    inline quint16 yaw() { return m_yaw; }
    inline quint16 pitch() { return m_pitch; }
    inline quint16 roll() { return m_roll; }
    inline bool isValid() { return m_valid; }

private:
    quint16 m_yaw;
    quint16 m_pitch;
    quint16 m_roll;
    bool m_valid;
};


class WiimoteMessage {
public:
    WiimoteMessage();
    WiimoteMessage(qint64 deltaTime);
    inline qint64 deltaTime() { return m_deltaTime; }
    WiimoteMessage *setGyro(Coordinates message);
    WiimoteMessage *setAccel(Coordinates message);
    inline Coordinates gyro() { return gyro_coordinates; }
    inline Coordinates accel() { return accel_coordinates; }
private:
    qint64 m_deltaTime;
    Coordinates gyro_coordinates;
    Coordinates accel_coordinates;
};

Q_DECLARE_METATYPE(Coordinates)
Q_DECLARE_METATYPE(WiimoteMessage)

#endif // WIIMOTE_MESSAGES_H
