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

#ifndef _WIIMOTE_PLUGIN_HPP_
#define _WIIMOTE_PLUGIN_HPP_

#include "StelGui.hpp"
#include "StelModule.hpp"
#include "WiimotePluginWindow.hpp"

#include <QString>
#include "wiimote/angles.h"
#include "wiimote/bigdecimal.h"
#include "wiimote/wiimote_messages.h"

class WiimotePluginWindow;
class WiimoteManager;

//! Main class of the plug-in
class WiimotePlugin : public StelModule
{
	Q_OBJECT

public:
    WiimotePlugin();
    virtual ~WiimotePlugin();
	
	///////////////////////////////////////////////////////////////////////////
	// Methods inherited from the StelModule class
	//! called when the plug-in is loaded.
	//! All initializations should be done here.
	virtual void init();
	//! called before the plug-in is un-loaded.
	//! Useful for stopping processes, unloading textures, etc.
	virtual void deinit();
	virtual void update(double deltaTime);
	//! draws on the view port.
	//! Dialog windows don't need explicit drawing, it's done automatically.
	//! If a plug-in draws on the screen, it should be able to respect
	//! the night vision mode.
	virtual void draw(StelCore * core);
	virtual double getCallOrder(StelModuleActionName actionName) const;
	//! called when the "configure" button in the "Plugins" tab is pressed
	virtual bool configureGui(bool show);

	///////////////////////////////////////////////////////////////////////////
    // Methods specific to WiimotePlugin
	
public slots:
    virtual void move(Angles<BigDecimal> angles);
	
private:
	//Toolbar button
	QPixmap* pixmapHover;
	QPixmap* pixmapOnIcon;
	QPixmap* pixmapOffIcon;
	StelButton* toolbarButton;
    WiimoteManager *wiimoteManager;
	//Dialog window
    WiimotePluginWindow* dialogWindow;
};


#include "fixx11h.h"
#include <QObject>
#include "StelPluginInterface.hpp"

//! This class is used by Qt to manage a plug-in interface
class WiimoteStelPluginInterface : public QObject, public StelPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(StelPluginInterface)
public:
	virtual StelModule* getStelModule() const;
	virtual StelPluginInfo getPluginInfo() const;
};

#endif /*_WIIMOTE_PLUGIN_HPP_*/
