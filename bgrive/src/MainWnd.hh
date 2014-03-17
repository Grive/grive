/*
	grive: an GPL program to sync a local directory with Google Drive
	Copyright (C) 2013 Wan Wai Ho

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation version 2
	of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
	MA  02110-1301, USA.
*/

#pragma once

#include <QtGui/QMainWindow>
#include <QSystemTrayIcon>
#include <QDialog>

#include "ui_MainWindow.h"
#include "ui_Console.h"
#include "ui_Settings.h"
#include "ui_About.h"

#include "DriveModel.hh"

class QModelIndex ;

namespace gr {

namespace http
{
	class Agent ;
}

class MainWnd : public QMainWindow
{
	Q_OBJECT

public :
	MainWnd( http::Agent *agent ) ;


private :
	void ShowResource( const v2::Resource *res ) ;
    void createTrayIcon();
    void setIcon(int index);
    void setVisible(bool visible);


public slots :
	void OnClickItem( const QModelIndex& index ) ;
	void OnClickDetailTab() ;
	void OnClickPreviewTab() ;
	void OnClose(QCloseEvent *event);
	void iconActivated(QSystemTrayIcon::ActivationReason reason);

private :
	// Ui components
	Ui::MainWindow	m_mainwindow;
	Ui::Settings	m_settings;
	Ui::Console		m_console;
	Ui::About		m_about;

	// Items
	QModelIndex 	currentItem;
	DriveModel		m_drive ;

	// Actions
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

} ;

} // end of namespace

