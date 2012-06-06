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

#include "Config.hh"

#include "drive/Drive.hh"

#include "protocol/OAuth2.hh"
#include "protocol/Json.hh"

#include "bfd/Backtrace.hh"
#include "util/Exception.hh"
#include "util/log/Log.hh"
#include "util/log/CompositeLog.hh"
#include "util/log/DefaultLog.hh"

#include <boost/exception/all.hpp>

#include <gcrypt.h>

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <unistd.h>

const std::string client_id		= "22314510474.apps.googleusercontent.com" ;
const std::string client_secret	= "bl4ufi89h-9MkFlypcI7R785" ;

using namespace gr ;

// libgcrypt insist this to be done in application, not library
void InitGCrypt()
{
	if ( !gcry_check_version(GCRYPT_VERSION) )
		throw Exception() << expt::ErrMsg( "libgcrypt version mismatch" ) ;

	// disable secure memory
	gcry_control(GCRYCTL_DISABLE_SECMEM, 0);

	// tell Libgcrypt that initialization has completed
	gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);
}

int Main( int argc, char **argv )
{
	InitGCrypt() ;
	
	Config config ;
	
	std::auto_ptr<log::CompositeLog> comp_log(new log::CompositeLog) ;
	LogBase* console_log = comp_log->Add( std::auto_ptr<LogBase>( new log::DefaultLog ) ) ;
	
	Json options ;
	
	int c ;
	while ((c = getopt(argc, argv, "al:vVfd")) != -1)
	{
		switch ( c )
		{
			case 'a' :
			{
				std::cout
					<< "-----------------------\n"
					<< "Please go to this URL and get an authentication code:\n\n"
					<< OAuth2::MakeAuthURL( client_id )
					<< std::endl ;
				
				std::cout
					<< "\n-----------------------\n"
					<< "Please input the authentication code here: " << std::endl ;
				std::string code ;
				std::cin >> code ;
				
				OAuth2 token( client_id, client_secret ) ;
				token.Auth( code ) ;
				
				// save to config
				config.Get().Add( "refresh_token", Json( token.RefreshToken() ) ) ;
				config.Save() ;
				
				break ;
			}
			
			case 'l' :
			{
				std::auto_ptr<LogBase> file_log(new log::DefaultLog(optarg)) ;
				file_log->Enable( log::debug ) ;
				file_log->Enable( log::verbose ) ;
				file_log->Enable( log::info ) ;
				file_log->Enable( log::warning ) ;
				file_log->Enable( log::error ) ;
				file_log->Enable( log::critical ) ;
				comp_log->Add( file_log ) ;
				break ;
			}
			
			case 'v' :
			{
				std::cout
					<< "grive version " VERSION " " __DATE__ " " __TIME__ << std::endl ;
				return 0 ;
			}
			
			case 'V' :
			{
				console_log->Enable( log::verbose ) ;
				break ;
			}
			
			case 'd' :
			{
				console_log->Enable( log::verbose ) ;
				console_log->Enable( log::debug ) ;
				break ;
			}
			
			case 'f' :
			{
				options.Add( "force", Json(true) ) ;
				break ;
			}
		}
	}
	LogBase::Inst( std::auto_ptr<LogBase>(comp_log.release()) ) ;
	
	std::string refresh_token ;
	try
	{
		refresh_token = config.Get()["refresh_token"].Str() ;
	}
	catch ( Exception& e )
	{
		Trace( "cannot read config: %1%", boost::diagnostic_information(e) ) ;
		Log(
			"Please run grive with the \"-a\" option if this is the "
			"first time you're accessing your Google Drive!",
			log::critical ) ;
		
		return -1;
	}
	
	OAuth2 token( refresh_token, client_id, client_secret ) ;
	Drive drive( token, options ) ;

	drive.Update() ;
	drive.SaveState() ;
	
	config.Save() ;
	
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
		return -1 ;
	}
}
