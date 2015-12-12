/*
	grive: an GPL program to sync a local directory with Google Drive
	Copyright (C) 2012  Wan Wai Ho

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

#include "util/Config.hh"

#include "base/Drive.hh"
#include "drive2/Syncer2.hh"

#include "http/CurlAgent.hh"
#include "protocol/AuthAgent.hh"
#include "protocol/OAuth2.hh"
#include "json/Val.hh"

#include "bfd/Backtrace.hh"
#include "util/Exception.hh"
#include "util/log/Log.hh"
#include "util/log/CompositeLog.hh"
#include "util/log/DefaultLog.hh"

// boost header
#include <boost/exception/all.hpp>
#include <boost/program_options.hpp>

// initializing libgcrypt, must be done in executable
#include <gcrypt.h>

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <unistd.h>

const std::string client_id		= "22314510474.apps.googleusercontent.com" ;
const std::string client_secret	= "bl4ufi89h-9MkFlypcI7R785" ;

using namespace gr ;
namespace po = boost::program_options;

// libgcrypt insist this to be done in application, not library
void InitGCrypt()
{
	if ( !gcry_check_version(GCRYPT_VERSION) )
		throw std::runtime_error( "libgcrypt version mismatch" ) ;

	// disable secure memory
	gcry_control(GCRYCTL_DISABLE_SECMEM, 0);

	// tell Libgcrypt that initialization has completed
	gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);
}

void InitLog( const po::variables_map& vm )
{
	std::auto_ptr<log::CompositeLog> comp_log(new log::CompositeLog) ;
	LogBase* console_log = comp_log->Add( std::auto_ptr<LogBase>( new log::DefaultLog ) ) ;

	if ( vm.count( "log" ) )
	{
		std::auto_ptr<LogBase> file_log(new log::DefaultLog( vm["log"].as<std::string>() )) ;
		file_log->Enable( log::debug ) ;
		file_log->Enable( log::verbose ) ;
		file_log->Enable( log::info ) ;
		file_log->Enable( log::warning ) ;
		file_log->Enable( log::error ) ;
		file_log->Enable( log::critical ) ;
		
		// log grive version to log file
		file_log->Log( log::Fmt("grive version " VERSION " " __DATE__ " " __TIME__), log::verbose ) ;
		file_log->Log( log::Fmt("current time: %1%") % DateTime::Now(), log::verbose ) ;
		
		comp_log->Add( file_log ) ;
	}
	
	if ( vm.count( "verbose" ) )
	{
		console_log->Enable( log::verbose ) ;
	}
	
	if ( vm.count( "debug" ) )
	{
		console_log->Enable( log::verbose ) ;
		console_log->Enable( log::debug ) ;
	}
	LogBase::Inst( std::auto_ptr<LogBase>(comp_log.release()) ) ;
}

int Main( int argc, char **argv )
{
	InitGCrypt() ;
	
	// construct the program options
	po::options_description desc( "Grive options" );
	desc.add_options()
		( "help,h",		"Produce help message" )
		( "version,v",	"Display Grive version" )
		( "auth,a",		"Request authorization token" )
		( "path,p",		po::value<std::string>(), "Root directory to sync")
		( "dir,s",		po::value<std::string>(), "Single subdirectory to sync (remembered for next runs)")
		( "verbose,V",	"Verbose mode. Enable more messages than normal.")
		( "log-http",	po::value<std::string>(), "Log all HTTP responses in this file for debugging.")
		( "new-rev",	"Create new revisions in server for updated files.")
		( "debug,d",	"Enable debug level messages. Implies -v.")
		( "log,l",		po::value<std::string>(), "Set log output filename." )
		( "force,f",	"Force grive to always download a file from Google Drive "
						"instead of uploading it." )
		( "dry-run",	"Only detect which files need to be uploaded/downloaded, "
						"without actually performing them." )
		( "ignore",		po::value<std::string>(), "Perl RegExp to ignore files (matched against relative paths, remembered for next runs)." )
		( "move,m", po::value<std::vector<std::string> >()->multitoken(), "Syncs, then moves a file (first argument) to new location (second argument) without reuploading or redownloading." )
	;
	
	po::variables_map vm;
	po::store(po::parse_command_line( argc, argv, desc), vm );
	po::notify(vm);
	
	// simple commands that doesn't require log or config
	if ( vm.count("help") )
	{
		std::cout << desc << std::endl ;
		return 0 ;
	}
	else if ( vm.count( "version" ) )
	{
		std::cout
			<< "grive version " << VERSION << ' ' << __DATE__ << ' ' << __TIME__ << std::endl ;
		return 0 ;
	}

	// initialize logging
	InitLog(vm) ;
	
	Config config(vm) ;
	
	Log( "config file name %1%", config.Filename(), log::verbose );

	std::auto_ptr<http::Agent> http( new http::CurlAgent );
	if ( vm.count( "log-http" ) )
		http->SetLog( new http::ResponseLog( vm["log-http"].as<std::string>(), ".txt" ) );

	if ( vm.count( "auth" ) )
	{
		OAuth2 token( http.get(), client_id, client_secret ) ;
		
		std::cout
			<< "-----------------------\n"
			<< "Please go to this URL and get an authentication code:\n\n"
			<< token.MakeAuthURL()
			<< std::endl ;
		
		std::cout
			<< "\n-----------------------\n"
			<< "Please input the authentication code here: " << std::endl ;
		std::string code ;
		std::cin >> code ;
		
		token.Auth( code ) ;
		
		// save to config
		config.Set( "refresh_token", Val( token.RefreshToken() ) ) ;
		config.Save() ;
	}
	
	std::string refresh_token ;
	try
	{
		refresh_token = config.Get("refresh_token").Str() ;
	}
	catch ( Exception& e )
	{
		Log(
			"Please run grive with the \"-a\" option if this is the "
			"first time you're accessing your Google Drive!",
			log::critical ) ;
		
		return -1;
	}
	
	OAuth2 token( http.get(), refresh_token, client_id, client_secret ) ;
	AuthAgent agent( token, http.get() ) ;
	v2::Syncer2 syncer( &agent );

	Drive drive( &syncer, config.GetAll() ) ;
	drive.DetectChanges() ;

	if ( vm.count( "dry-run" ) == 0 )
	{
		drive.Update() ;
		drive.SaveState() ;
	}
	else
		drive.DryRun() ;
		
	if ( vm.count ( "move" ) > 0 && vm.count( "dry-run" ) == 0 )
	{
		if (vm["move"].as<std::vector<std::string> >().size() < 2 )
			Log( "Not enough arguments for move. Move failed.", log::error );
		else
		{
			bool success = drive.Move( vm["move"].as<std::vector<std::string> >()[0],
			                           vm["move"].as<std::vector<std::string> >()[1] );
			if (success)
				Log( "Move successful!", log::info );
			else
				Log( "Move failed.", log::error);
		}
	}
	
	config.Save() ;
	Log( "Finished!", log::info ) ;
	return 0 ;
}

int main( int argc, char **argv )
{
	try
	{
		return Main( argc, argv ) ;
	}
	catch ( Exception& e )
	{
		Log( "exception: %1%", boost::diagnostic_information(e), log::critical ) ;
	}
	catch ( std::exception& e )
	{
		Log( "exception: %1%", e.what(), log::critical ) ;
	}
	catch ( ... )
	{
		Log( "unexpected exception", log::critical ) ;
	}
	return -1 ;
}
