/*
 * Stellarium Wiimote Plug-in
 * 
 * Copyright (C) 2012 Marco Gulino
 * plugin structure copied from Stellarium Example plugin (Copyright (C) 2010 Bogdan Marinov)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
 
#ifndef _WIIMOTEPLUGINWINDOW_HPP_
#define _WIIMOTEPLUGINWINDOW_HPP_

#include <QObject>

//#include "StelDialog.hpp"
//Windows bug workaround:
#include "StelDialogWiimotePlugin.hpp"
#include "wiimote/angles.h"
#include "wiimote/bigdecimal.h"

class Ui_wiimotePluginForm;
class WiimotePlugin;


class WiimotePluginWindow : public WiimotePluginDialog4Winows
{
	Q_OBJECT
public:
    WiimotePluginWindow();
    virtual ~WiimotePluginWindow();
	void languageChanged();
    bool invertDeltaAxes();
signals:
    void connectWiimote();
    void disconnectWiimote();
    void calibrateWiimote();
    void resetWiimote();
    void move(Angles<BigDecimal> angles);
public slots:
    void clearStatusMessage();
    void showStatusMessage(const QString & message, int timeout = 0);
    void wiimoteConnected();
    void wiimoteDisconnected();
    void calibrationFinished();
    void needsCalibration();
    void calibrating(int remainingSeconds);
    void gotReadings(Angles<BigDecimal> angles);
    void gotDelta(Angles<BigDecimal> angles);
private slots:
    void emitCalibrate();
    void emitMove();
    void emitConnect();
    void emitDisconnect();
protected:
	//! Initialize the dialog widgets and connect the signals/slots.
	virtual void createDialogContent();
    Ui_wiimotePluginForm * ui;
	
private:
    WiimotePlugin * wiimotePluginModule;
    QString anglesLabel(Angles<BigDecimal> angles);
};

#endif // _WIIMOTEPLUGINWINDOW_
