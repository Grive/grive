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
#include "util/ProgressBar.hh"

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

const std::string default_id            = "22314510474.apps.googleusercontent.com" ;
const std::string default_secret        = "bl4ufi89h-9MkFlypcI7R785" ;

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
	std::unique_ptr<log::CompositeLog> comp_log( new log::CompositeLog ) ;
	std::unique_ptr<LogBase> def_log( new log::DefaultLog );
	LogBase* console_log = comp_log->Add( def_log ) ;

	if ( vm.count( "log" ) )
	{
		std::unique_ptr<LogBase> file_log( new log::DefaultLog( vm["log"].as<std::string>() ) ) ;
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
	LogBase::Inst( comp_log.release() ) ;
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
                ( "id,i",               po::value<std::string>(), "Authentication ID")
                ( "secret,e",           po::value<std::string>(), "Authentication secret")
		( "path,p",		po::value<std::string>(), "Path to working copy root")
		( "dir,s",		po::value<std::string>(), "Single subdirectory to sync")
		( "verbose,V",	"Verbose mode. Enable more messages than normal.")
		( "log-http",	po::value<std::string>(), "Log all HTTP responses in this file for debugging.")
		( "new-rev",	"Create new revisions in server for updated files.")
		( "debug,d",	"Enable debug level messages. Implies -v.")
		( "log,l",		po::value<std::string>(), "Set log output filename." )
		( "force,f",	"Force grive to always download a file from Google Drive "
						"instead of uploading it." )
		( "upload-only,u", "Do not download anything from Google Drive, only upload local changes" )
		( "no-remote-new,n", "Download only files that are changed in Google Drive and already exist locally" )
		( "dry-run",	"Only detect which files need to be uploaded/downloaded, "
						"without actually performing them." )
		( "upload-speed,U", po::value<unsigned>(), "Limit upload speed in kbytes per second" )
		( "download-speed,D", po::value<unsigned>(), "Limit download speed in kbytes per second" )
		( "progress-bar,P", "Enable progress bar for upload/download of files")
	;
	
	po::variables_map vm;
	try
	{
		po::store( po::parse_command_line( argc, argv, desc ), vm );
	}
	catch( po::error &e )
	{
		std::cerr << "Options are incorrect. Use -h for help\n";
		return -1;
	}
	po::notify( vm );
	
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
	InitLog( vm ) ;
	
	Config config( vm ) ;
	
	Log( "config file name %1%", config.Filename(), log::verbose );

	std::unique_ptr<http::Agent> http( new http::CurlAgent );
	if ( vm.count( "log-http" ) )
		http->SetLog( new http::ResponseLog( vm["log-http"].as<std::string>(), ".txt" ) );

	std::unique_ptr<ProgressBar> pb;
	if ( vm.count( "progress-bar" ) )
	{
		pb.reset( new ProgressBar() );
		http->SetProgressReporter( pb.get() );
	}

	std::string id = default_id;
	std::string secret = default_secret;

	if( vm.count( "id" ) )
	{
		id = vm["id"].as<std::string>();
	}

	if( vm.count( "secret" ) )
	{
		secret = vm["secret"].as<std::string>();
	}

	if ( vm.count( "auth" ) )
	{
		OAuth2 token( http.get(), id, secret ) ;
		
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
	
	OAuth2 token( http.get(), refresh_token, id, secret ) ;
	AuthAgent agent( token, http.get() ) ;
	v2::Syncer2 syncer( &agent );

	if ( vm.count( "upload-speed" ) > 0 )
		agent.SetUploadSpeed( vm["upload-speed"].as<unsigned>() * 1000 );
	if ( vm.count( "download-speed" ) > 0 )
		agent.SetDownloadSpeed( vm["download-speed"].as<unsigned>() * 1000 );

	Drive drive( &syncer, config.GetAll() ) ;
	drive.DetectChanges() ;

	if ( vm.count( "dry-run" ) == 0 )
	{
		// The progress bar should just be enabled when actual file transfers take place
		if ( pb )
			pb->setShowProgressBar( true ) ;
		drive.Update() ;
		if ( pb )
			pb->setShowProgressBar( false ) ;

		drive.SaveState() ;
	}
	else
		drive.DryRun() ;
		
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
