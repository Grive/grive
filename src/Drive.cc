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

#include "Drive.hh"

#include "HTTP.hh"
#include "Json.hh"
#include "OAuth2.hh"

// for debugging only
#include <iostream>

namespace gr {

Drive::Drive( OAuth2& auth ) :
	m_auth( auth )
{
	m_http_hdr.push_back( "Authorization: Bearer " + m_auth.AccessToken() ) ;
	m_http_hdr.push_back( "GData-Version: 3.0" ) ;
	
	Json resp = Json::Parse(HttpGet( "https://docs.google.com/feeds/default/private/full?alt=json", m_http_hdr )) ;

/*	Json::Object map = resp["feed"]["id"].As<Json::Object>() ;
	for ( Json::Object::iterator i = map.begin() ; i != map.end() ; ++i )
	{
		std::cout << i->first << "\t" << i->second.DataType() << std::endl ;
	}*/
	
	Json::Array a = resp["feed"]["entry"].As<Json::Array>() ;
	for ( Json::Array::iterator i = a.begin() ; i != a.end() ; ++i )
	{
		DownloadEntry( *i ) ;
	}
}

void Drive::DownloadEntry( const Json& entry )
{
	Json::Object map = entry.As<Json::Object>() ;
	for ( Json::Object::iterator i = map.begin() ; i != map.end() ; ++i )
	{
//		std::cout << i->first << "\t" << i->second.DataType() << std::endl ;
	}
	
	// only handle uploaded files
	if ( entry.Has( "docs$filename" ) )
	{
		// use title as the filename
		std::string filename	= entry["docs$filename"]["$t"].As<std::string>() ;
		std::string url			= entry["content"]["src"].As<std::string>() ;
		
		std::cout << "downloading " << filename << " from " << url << std::endl ;
		
		HttpGetFile( url, filename, m_http_hdr ) ;
	}
}

} // end of namespace
