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

#include <cstdlib>

#include <iostream>

int main( int argc, char **argv )
{
	using namespace gr ;
	
	int c ;
	while ((c = getopt (argc, argv, "ac:")) != -1)
	{
		switch ( c )
		{
			case 'a' :
			{
				std::cout << 
					OAuth2::MakeAuthURL( "22314510474.apps.googleusercontent.com" ) << std::endl ;
				return 0 ;
			}
			
			case 'c' :
			{
				OAuth2 token ;
				token.Auth( optarg ) ;
				
				// print the refresh token an exist
				std::cout << token.RefreshToken() << std::endl ;
				return 0 ;
			}
		}
	}
	
	OAuth2 token( getenv( "GR_REFRESH_CODE" ) ) ;
	Drive drive( token ) ;
	
	return 0 ;
}
