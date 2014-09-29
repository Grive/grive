/*
	qgrive: an GPL program to sync a local directory with Google Drive
	Copyright (C) 2012  Wan Wai Ho (libgrive + original idea)
	Copyright (C) 2012  Massimo Gengarelli (qgrive)

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
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <fstream>
#include <sstream>
#include <iostream>
#include <QtCore>
#include <QtGui>
#include <QGriveMainWindow.hh>
#include "ui_mainwindow.h"


QGriveMainWindow::QGriveMainWindow( ) :
	QMainWindow(0),
	ui(new Ui::QGriveMainWindow),
	_log_activated(false)
{
	ui->setupUi(this);

	QObject::connect( ui->refresh_btn,
			SIGNAL(clicked()),
			this,
			SLOT(refresh_tree()) ) ;

	QObject::connect( ui->launch_sync_btn,
			SIGNAL(clicked()),
			this,
			SLOT(sync_directory()) ) ;

	ui->statusbar->setEnabled( true ) ;
	ui->statusbar->addWidget( new QLabel(" Welcome to QGrive!! ") ) ;
}

QGriveMainWindow::~QGriveMainWindow( )
{

}

void QGriveMainWindow::activate_log( )
{
	_log_activated = true ;
	std::cout << "LOG ACTIVATED" << std::endl ;
}

void QGriveMainWindow::refresh_tree( )
{
	if (_log_activated)
	{
		std::cout << "refresh_tree()" << std::endl;
	}
}

void QGriveMainWindow::sync_directory( )
{
	if (_log_activated)
	{
		std::cout << "sync_directory()" << std::endl;
	}

}
