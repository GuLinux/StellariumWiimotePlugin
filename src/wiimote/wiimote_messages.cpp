#include "wiimote_messages.h"

class _RegisterWiimoteMessagesMetatypes {
public:
    _RegisterWiimoteMessagesMetatypes() {
        qRegisterMetaType<Coordinates>();
        qRegisterMetaType<WiimoteMessage>();
    }
};

static _RegisterWiimoteMessagesMetatypes registerWiimoteMessagesMetatypes;

Coordinates::Coordinates()
    : m_yaw(0), m_pitch(0), m_roll(0), m_valid(false)
{
}

Coordinates::Coordinates(quint16 yawRate, quint16 pitchRate, quint16 rollRate)
    : m_yaw(yawRate), m_pitch(pitchRate), m_roll(rollRate), m_valid(true)
{

}


WiimoteMessage::WiimoteMessage()
{
}

WiimoteMessage::WiimoteMessage(qint64 deltaTime)
    : m_deltaTime(deltaTime)
{
}

WiimoteMessage *WiimoteMessage::setGyro(Coordinates message)
{
    this->gyro_coordinates = message;
    return this;
}

WiimoteMessage *WiimoteMessage::setAccel(Coordinates message)
{
    this->accel_coordinates = message;
    return this;
}
