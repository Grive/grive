/*
	grive: an GPL program to sync a local directory with Google Drive
	Copyright (C) 2012  Wan Wai Ho

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; version 2
	of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "Drive.hh"

#include "protocol/HTTP.hh"
#include "protocol/Json.hh"
#include "OAuth2.hh"

#include <openssl/evp.h>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>

// for debugging only
#include <iostream>

namespace gr {

std::string MD5( std::streambuf *file )
{
	char buf[64 * 1024] ;
	EVP_MD_CTX	md ;
	EVP_MD_CTX_init( &md );
	EVP_DigestInit_ex( &md, EVP_md5(), 0 ) ;
	
	std::size_t count = 0 ;
	while ( (count = file->sgetn( buf, sizeof(buf) )) > 0 )
	{
		EVP_DigestUpdate( &md, buf, count ) ;
	}
	
	unsigned int md5_size = EVP_MAX_MD_SIZE ;
	unsigned char md5[EVP_MAX_MD_SIZE] ;
	EVP_DigestFinal_ex( &md, md5, &md5_size ) ;
	
	std::ostringstream ss ;
	for ( unsigned int i = 0 ; i < md5_size ; i++ )
		ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(md5[i]) ;
	
	return ss.str() ;
}

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
// 	Json::Object map = entry.As<Json::Object>() ;
// 	for ( Json::Object::iterator i = map.begin() ; i != map.end() ; ++i )
// 	{
// 		std::cout << i->first << "\t" << i->second.DataType() << std::endl ;
// 	}
	
	// only handle uploaded files
	if ( entry.Has( "docs$filename" ) )
	{
		// use title as the filename
		std::string filename	= entry["docs$filename"]["$t"].As<std::string>() ;
		std::string url			= entry["content"]["src"].As<std::string>() ;
		
		bool changed = true ;
		
		// compare checksum first if file exists
		std::ifstream ifile( filename.c_str(), std::ios::binary | std::ios::out ) ;
		if ( ifile && entry.Has("docs$md5Checksum") )
		{
			std::string remote_md5	= entry["docs$md5Checksum"]["$t"].As<std::string>() ;
			std::string local_md5	= MD5( ifile.rdbuf() ) ;
			
			std::transform( remote_md5.begin(), remote_md5.end(), remote_md5.begin(), tolower ) ;
			if ( local_md5 == remote_md5 )
				changed = false ;
		}
		
		if ( changed )
		{
std::cout << "downloading " << filename << std::endl ;
			HttpGetFile( url, filename, m_http_hdr ) ;
		}
	}
}

} // end of namespace
