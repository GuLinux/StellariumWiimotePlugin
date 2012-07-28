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

#include "Dialog.hpp"
#include "StelApp.hpp"
#include "StelModuleMgr.hpp"
#include "StelLocaleMgr.hpp"
#include "WiimotePlugin.hpp"
#include "WiimotePluginWindow.hpp"
#include "ui_wiimotePluginDialogWindow.h"
#include <QStatusBar>
#include <math.h>
#include "wiimote/wiimote.h"
#include <QMessageBox>

#include <QDebug>

WiimotePluginWindow::WiimotePluginWindow()
{
    ui = new Ui_wiimotePluginForm;
	//TODO: Fix this - it's in the same plugin
    wiimotePluginModule = GETSTELMODULE(WiimotePlugin);
}

WiimotePluginWindow::~WiimotePluginWindow()
{
	delete ui;
}

void WiimotePluginWindow::languageChanged()
{
	if (dialog)
        ui->retranslateUi(dialog);
}

bool WiimotePluginWindow::invertDeltaAxes()
{
    return ui->invertDelta->isChecked();
}

void WiimotePluginWindow::emitMove()
{
//    BigDecimal yaw = ui->yaw->value();
//    BigDecimal pitch = ui->pitch->value();
//    BigDecimal roll = ui->roll->value();
//    Angles<BigDecimal> angles(yaw, pitch, roll);
//    qDebug() << "Emitting manualMove: " << angles.toString();
//    emit move(angles);
}

void WiimotePluginWindow::emitConnect()
{
    QMessageBox::information(0, QString("Wiimote pairing"), QString("Press 1+2 on Wiimote"));
    ui->connWiimote->setEnabled(false);
    emit connectWiimote();
}

void WiimotePluginWindow::emitDisconnect()
{
    ui->disconnectWiimote->setEnabled(false);
    emit disconnectWiimote();
}

void WiimotePluginWindow::clearStatusMessage()
{
    ui->statusBar->clearMessage();
}

void WiimotePluginWindow::showStatusMessage(const QString &message, int timeout)
{
    ui->statusBar->showMessage(message, timeout);
}

// Initialize the dialog widgets and connect the signals/slots
void WiimotePluginWindow::createDialogContent()
{
	ui->setupUi(dialog);
    ui->statusBar->showMessage("Wiimote not connected.");
    connect(ui->connWiimote, SIGNAL(clicked()), this, SLOT(emitConnect()));
    connect(ui->disconnectWiimote, SIGNAL(clicked()), this, SLOT(emitDisconnect()));
    connect(ui->calibrate, SIGNAL(clicked()), this, SLOT(emitCalibrate()));
    connect(ui->resetWiimote, SIGNAL(clicked()), this, SIGNAL(resetWiimote()));
	//Connect all signals and slots here: sender, signal, receiver, method
	connect(ui->closeStelWindow, SIGNAL(clicked()), this, SLOT(close()));
//    connect(ui->manualMove, SIGNAL(clicked()), this, SLOT(emitMove()));
	
	//Initialize controls and their state here:
	
    qDebug() << "WiimoteWindow::createDialogContent() has been called.";
}


void WiimotePluginWindow::wiimoteConnected()
{
    ui->connStack->setCurrentWidget(ui->disconnectWiimotePage);
    ui->connWiimote->setEnabled(false);
    ui->disconnectWiimote->setEnabled(true);
}

void WiimotePluginWindow::wiimoteDisconnected()
{
    ui->connStack->setCurrentWidget(ui->connectWiimotePage);
    ui->connWiimote->setEnabled(true);
    ui->disconnectWiimote->setEnabled(false);
}

void WiimotePluginWindow::calibrationFinished()
{
    ui->calibrate->setEnabled(true);
}

void WiimotePluginWindow::needsCalibration()
{
    showStatusMessage("Wiimote needs calibration first");
}

void WiimotePluginWindow::calibrating(int remainingSeconds)
{
    showStatusMessage(QString("Calibrating: %1 seconds remaining").arg(remainingSeconds), 1000);
}

void WiimotePluginWindow::gotReadings(Angles<BigDecimal> angles)
{
    ui->readings->setText(anglesLabel(angles));
}

void WiimotePluginWindow::gotDelta(Angles<BigDecimal> angles)
{
    ui->delta->setText(anglesLabel(angles));
}



QString WiimotePluginWindow::anglesLabel(Angles<BigDecimal> angles)
{
    BigDecimal yawGrad = angles.yaw() * 180 / M_PI;
    BigDecimal pitchGrad = angles.pitch() * 180 / M_PI;
    BigDecimal rollGrad = angles.roll() * 180 / M_PI;
    return QString("Yaw: %1\tGrad: %2\nPitch: %3\tGrad: %4\nRoll: %5\tGrad: %6")
            .arg(angles.yaw().toString(4)).arg(yawGrad.toString(4))
            .arg(angles.pitch().toString(4)).arg(pitchGrad.toString(4))
            .arg(angles.roll().toString(4)).arg(rollGrad.toString(4));
}

void WiimotePluginWindow::emitCalibrate()
{
    QString message = QString("Wiimote needs to be calibrate to find proper \"still\" values.\n \
Calibration will last %1 seconds, and you need to keep Wiimote perfectly still.\nContinue?")
            .arg(CALIBRATION_STEPS);
    int result = QMessageBox::warning(0, "Calibration", message, QMessageBox::Ok, QMessageBox::Cancel);

    if(result != QMessageBox::Ok) return;
    ui->calibrate->setEnabled(false);
    emit calibrateWiimote();
}
