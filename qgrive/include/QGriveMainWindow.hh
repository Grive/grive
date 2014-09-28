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

#pragma once

#include <QtGui>
#include <QtCore>

#include <iostream>
#include <sstream>
#include <fstream>

// Forward declarations for Qt generated files
namespace Ui {
	class QGriveMainWindow;
}

class QGriveMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit QGriveMainWindow( );
	virtual ~QGriveMainWindow( );

	void activate_log( ) ;

signals:

public slots:

private slots:
	void refresh_tree( ) ;
	void sync_directory( ) ;

private:
	Ui::QGriveMainWindow* ui ;

	bool _log_activated ;
};
