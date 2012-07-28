/*
 Stellarium Wiimote Plug-in
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

#include "WiimotePlugin.hpp"

#include "StelApp.hpp"
#include "StelGui.hpp"
#include "StelGuiItems.hpp"
#include "StelLocaleMgr.hpp"
#include "StelModuleMgr.hpp"
#include "StelCore.hpp"
#include "StelMovementMgr.hpp"

#include <QAction>
#include <QString>
#include <QDebug>
#include <QVector3D>
#include <stdexcept>
#include "wiimote/wiimotemanager.h"

////////////////////////////////////////////////////////////////////////////////
// 
StelModule* WiimoteStelPluginInterface::getStelModule() const
{
    return new WiimotePlugin();
}

StelPluginInfo WiimoteStelPluginInterface::getPluginInfo() const
{
	// Allow to load the resources when used as a static plugin
    Q_INIT_RESOURCE(wiimotePlugin);
	
	StelPluginInfo info;
    info.id = "WiimotePlugin";
    info.displayedName = "Wiimote Plugin";
    info.authors = "Marco Gulino";
    info.contact = "http://rockman81.blogspot.com";
    info.description = "A plugin moving stellarium view center based on Wiimote gyroscope data.";
	return info;
}

Q_EXPORT_PLUGIN2(WiimotePlugin, WiimoteStelPluginInterface)


////////////////////////////////////////////////////////////////////////////////
// Constructor and destructor
WiimotePlugin::WiimotePlugin()
{
    setObjectName("WiimotePlugin");
}

WiimotePlugin::~WiimotePlugin()
{

}


////////////////////////////////////////////////////////////////////////////////
// Methods inherited from the StelModule class
// init(), update(), draw(), setStelStyle(), getCallOrder()
void WiimotePlugin::init()
{
    qDebug() << "WiimotePlugin::init()";
	try
	{
		StelGui* gui = dynamic_cast<StelGui*>(StelApp::getInstance().getGui());
		
		//Create the dialog window
        dialogWindow = new WiimotePluginWindow();
		//and set up the keyboard shortcut to show it
        gui->addGuiActions("actionShow_Wiimote_Plugin_Window", N_("Wiimote Plugin window"), "Alt+W", N_("Plugin Key Bindings"), true, false);
        connect(gui->getGuiActions("actionShow_Wiimote_Plugin_Window"), SIGNAL(toggled(bool)), dialogWindow, SLOT(setVisible(bool)));
        connect(dialogWindow, SIGNAL(visibleChanged(bool)), gui->getGuiActions("actionShow_Wiimote_Plugin_Window"), SLOT(setChecked(bool)));
		/*
		NOTE: Dialog content is created (createDialogContent() is called)
		when the dialog is displayed for the first time.
		Trying to access dialog content (e.g. widgets) before that
		causes a segmentation fault, as the objects don't exist yet.
		*/
		
		//Create toolbar button
		pixmapHover = new QPixmap(":/graphicGui/gui/glow32x32.png");
        pixmapOnIcon = new QPixmap(":/wiimotePlugin/bitmap_button_on.png");
        pixmapOffIcon = new QPixmap(":/wiimotePlugin/bitmap_button_off.png");
        toolbarButton = new StelButton(NULL, *pixmapOnIcon, *pixmapOffIcon, *pixmapHover, gui->getGuiActions("actionShow_Wiimote_Plugin_Window"));
		gui->getButtonBar()->addButton(toolbarButton);

        wiimoteManager = new WiimoteManager(this);

        connect(dialogWindow, SIGNAL(connectWiimote()), wiimoteManager, SLOT(connectWiimote()));
        connect(dialogWindow, SIGNAL(disconnectWiimote()), wiimoteManager, SLOT(disconnectWiimote()));
        connect(dialogWindow, SIGNAL(calibrateWiimote()), wiimoteManager, SLOT(calibrateWiimote()));
        connect(dialogWindow, SIGNAL(resetWiimote()), wiimoteManager, SLOT(resetAngles()));
        connect(dialogWindow, SIGNAL(move(Angles<BigDecimal>)), this, SLOT(move(Angles<BigDecimal>)));

        connect(wiimoteManager, SIGNAL(connected()), dialogWindow, SLOT(wiimoteConnected()));
        connect(wiimoteManager, SIGNAL(disconnected()), dialogWindow, SLOT(wiimoteDisconnected()));
        connect(wiimoteManager, SIGNAL(deviceMessage(QString,int)), dialogWindow, SLOT(showStatusMessage(QString, int)));
//        connect(wiimoteManager, SIGNAL(angleChanged(Angles<BigDecimal>)), this, SLOT(move(Angles<BigDecimal>)));
        connect(wiimoteManager, SIGNAL(deltaChanged(Angles<BigDecimal>)), this, SLOT(move(Angles<BigDecimal>)));
        connect(wiimoteManager, SIGNAL(needsCalibration()), dialogWindow, SLOT(needsCalibration()));
        connect(wiimoteManager, SIGNAL(calibrated()), dialogWindow, SLOT(calibrationFinished()));
        connect(wiimoteManager, SIGNAL(calibrationStep(int)), dialogWindow, SLOT(calibrating(int)));
        connect(wiimoteManager, SIGNAL(angleChanged(Angles<BigDecimal>)), dialogWindow, SLOT(gotReadings(Angles<BigDecimal>)));
        connect(wiimoteManager, SIGNAL(deltaChanged(Angles<BigDecimal>)), dialogWindow, SLOT(gotDelta(Angles<BigDecimal>)));
    }
	catch (std::runtime_error &e)
	{
        qWarning() << "WiimotePlugin::init() error: " << e.what();
		return;
	}
}

void WiimotePlugin::deinit()
{
    qDebug() << "WiimotePlugin::deinit()";
}

void WiimotePlugin::update(double deltaTime)
{
	//Stuff that needs to be updated
}

void WiimotePlugin::draw(StelCore* core)
{
	//Stuff that needs to be painted using StelPainter
}

double WiimotePlugin::getCallOrder(StelModuleActionName actionName) const
{
	if (actionName == StelModule::ActionDraw)
		return StelApp::getInstance().getModuleMgr().getModule("MeteorMgr")->getCallOrder(actionName) + 3.;
	return 0.;
}

bool WiimotePlugin::configureGui(bool show)
{
	if(show)
	{
		//Make sure that the toolbar button is toggled:
		StelGui* gui = dynamic_cast<StelGui*>(StelApp::getInstance().getGui());
        gui->getGuiActions("actionShow_Wiimote_Plugin_Window")->setChecked(true);
		//Alternatively, if no toolbar button is present:
		//dialogWindow->setVisible(true);
	}
	
    return true;
}

void WiimotePlugin::move(Angles<BigDecimal> angles)
{
    StelMovementMgr *movementMgr = StelApp::getInstance().getCore()->getMovementMgr();
    movementMgr->setEquatorialMount(false);
    Vec3d altAz(angles.yaw().toDouble(), angles.pitch().toDouble(), angles.roll().toDouble());
    Vec3d j2000 = StelApp::getInstance().getCore()->altAzToJ2000(altAz);
//    movementMgr->moveToJ2000(j2000, 0.3);
    qDebug() << "delta received: " << angles.toString();
    double sign = (dialogWindow->invertDeltaAxes() ? -1 : +1);
    movementMgr->panView(sign * angles.roll().toDouble(), sign * angles.yaw().toDouble());
}


