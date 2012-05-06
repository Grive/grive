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
