#include <QGriveApplication.hh>
#include <QGriveMainWindow.hh>

#include <QApplication>
#include <protocol/Json.hh>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <unistd.h>

QGriveException::QGriveException( const std::string& exception_msg )
	: std::runtime_error( exception_msg ),
	  _exception_msg( exception_msg )
{

}


QGriveException::~QGriveException( ) throw ( )
{

}

const char* QGriveException::what( ) const throw( )
{
	return _exception_msg.c_str() ;
}

std::ostream& operator<<( std::ostream& os,
		const QGriveApplication& application )
{
	if ( application._config_by_env )
	{
		os << "Using env: " << application._config_env ;
	}

	else
	{
		os << "Using file: " << application._config_file ;
	}

	return os ;
}


QGriveApplication::QGriveApplication( int argc, char *argv[] )
	: QApplication( argc, argv ),
	  _config_by_env( false ),
	  _config_by_file( false )
{

}

QGriveApplication::~QGriveApplication()
{

}

void QGriveApplication::setup_environment( ) throw ( QGriveException )
{
	const char* env_cfg = getenv( "GR_CONFIG" ) ;

	if ( env_cfg )
	{
		_config_env = getenv( "GR_CONFIG" ) ;
		_config_by_env = true ;
	}
	else
	{
		_config_file = getenv( "HOME" ) ;
		_config_file += "/.grive" ;
		_config_by_file = true ;
	}
}

int QGriveApplication::exec( )
{
	_main_window.show( ) ;
	_main_window.activate_log( ) ;
	return QApplication::exec( ) ;
}
