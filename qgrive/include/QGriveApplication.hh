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
