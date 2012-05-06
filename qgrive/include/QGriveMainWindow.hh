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
