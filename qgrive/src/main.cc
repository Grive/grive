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
