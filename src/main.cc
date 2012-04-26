/*
	grive: an GPL program to sync a local directory with Google Drive
	Copyright (C) 2012  Wan Wai Ho

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "OAuth2.hh"
#include "Drive.hh"
#include "Json.hh"

#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>

namespace gr
{
	const std::string& ConfigFilename()
	{
		static const char *env_cfg = ::getenv( "GR_CONFIG" ) ;
		static const std::string filename =
			(env_cfg != 0) ? env_cfg : std::string( ::getenv( "HOME") ) + "/.grive" ;

		return filename ;
	}

	Json ReadConfig()
	{
		std::ifstream ifile( ConfigFilename().c_str() ) ;
		
		if ( ifile )
		{
			std::string cfg_str(
				(std::istreambuf_iterator<char>( ifile )),
				(std::istreambuf_iterator<char>()) ) ;
			
			return Json::Parse( cfg_str ) ;
		}
		else
			return Json() ;
	}
	
	void SaveConfig( const Json& config )
	{
		std::ofstream ofile( ConfigFilename().c_str() ) ;
		ofile << config ;
	}
}

int main( int argc, char **argv )
{
	using namespace gr ;

	Json config = ReadConfig() ;
	
	int c ;
	while ((c = getopt (argc, argv, "ac:")) != -1)
	{
		switch ( c )
		{
			case 'a' :
			{
				std::cout
					<< "Please go to this URL and enter the code:\n"
					<< OAuth2::MakeAuthURL( "22314510474.apps.googleusercontent.com" )
					<< std::endl ;
				
				std::string code ;
				std::cin >> code ;
				
				OAuth2 token ;
				token.Auth( code ) ;
				
				// print the refresh token an exist
				std::cout << "got refresh token: " << token.RefreshToken() << std::endl ;

				// save to config
				config.Add( "refresh_token", Json( token.RefreshToken() ) ) ;
				assert( config["refresh_token"].As<std::string>() == token.RefreshToken() ) ;
				SaveConfig( config ) ;
				
				break ;
			}
		}
	}
	
	OAuth2 token( config["refresh_token"].As<std::string>() ) ;
	Drive drive( token ) ;
	
	return 0 ;
}
