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

#include <QApplication>
#include <QGriveMainWindow.hh>

#include <protocol/Json.hh>

#include <string>
#include <stdexcept>
#include <iostream>

class QGriveException : public std::runtime_error
{
public:
	explicit QGriveException( const std::string& exception_msg ) ;
	virtual ~QGriveException( ) throw( ) ;
	const char* what( ) const throw( ) ;

private:
	std::string _exception_msg;
};


class QGriveApplication : public QApplication
{
public:
	explicit QGriveApplication( int argc, char *argv[] ) ;
	virtual ~QGriveApplication( ) ;

	void setup_environment( ) throw ( QGriveException ) ;
	int exec( ) ;

	// Logging facility
	friend std::ostream& operator<<( std::ostream& os,
			const QGriveApplication& application ) ;

private:
	bool _config_by_env;
	bool _config_by_file;
	std::string _config_file;
	std::string _config_env;
	QGriveMainWindow _main_window;
	gr::Json _configuration;
};
