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
#include "util/Log.hh"
#include "util/DefaultLog.hh"

#include <boost/exception/all.hpp>

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <unistd.h>

const std::string client_id		= "22314510474.apps.googleusercontent.com" ;
const std::string client_secret	= "bl4ufi89h-9MkFlypcI7R785" ;

int main( int argc, char **argv )
{
	using namespace gr ;

	Config config ;
	
	DefaultLog nofile_log ;
	LogBase::Inst( &nofile_log ) ;
	
	Json options ;
	
	int c ;
	while ((c = getopt(argc, argv, "al:vVf")) != -1)
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
				static DefaultLog log( optarg ) ;
				LogBase::Inst( &log ) ;
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
				LogBase::Inst()->Enable( log::verbose ) ;
				break ;
			}
			
			case 'f' :
			{
				options.Add( "force", Json(true) ) ;
				break ;
			}
		}
	}
		
	
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
	
	try
	{
		OAuth2 token( refresh_token, client_id, client_secret ) ;
		Drive drive( token, options ) ;

		drive.Update() ;
		drive.SaveState() ;
		
		config.Save() ;
	}
	catch ( gr::Exception& e )
	{
		Log( "exception: %1%", boost::diagnostic_information(e), log::critical ) ;
		return -1 ;
	}
	
	return 0 ;
}
