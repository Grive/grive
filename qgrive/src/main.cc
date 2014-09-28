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

#include <iostream>
#include <sstream>
#include <fstream>
#include <QGriveMainWindow.hh>
#include <QGriveApplication.hh>

int main ( int argc, char *argv[] )
{
	QGriveApplication app( argc, argv ) ;
	app.setup_environment( ) ;

	std::ofstream log_file( "qgrive.log", std::ios_base::out|std::ios_base::app ) ;
	log_file << " ==== starting log ====" << std::endl ;

	log_file << app << std::endl ;

	int ret_value = app.exec( ) ;

	log_file << " ==== closing log ====" << std::endl << std::endl ;
	log_file.close( ) ;

	return ret_value;
}
