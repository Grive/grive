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

#include "OAuth2.hh"

#include "protocol/HTTP.hh"
#include "protocol/Json.hh"

// for debugging
#include <iostream>

namespace gr {

const std::string token_url		= "https://accounts.google.com/o/oauth2/token" ;

OAuth2::OAuth2(
	const std::string& refresh_code,
	const std::string&	client_id,
	const std::string&	client_secret ) :
	m_refresh( refresh_code ),
	m_client_id( client_id ),
	m_client_secret( client_secret )
{
	Refresh( ) ;
}

OAuth2::OAuth2(
	const std::string&	client_id,
	const std::string&	client_secret ) :
	m_client_id( client_id ),
	m_client_secret( client_secret )
{
}

void OAuth2::Auth( const std::string&	auth_code )
{
	std::string post =
		"code="				+ auth_code +
		"&client_id="		+ m_client_id +
		"&client_secret="	+ m_client_secret +
		"&redirect_uri="	+ "urn:ietf:wg:oauth:2.0:oob" +
		"&grant_type=authorization_code" ;

	Json resp = Json::Parse( http::PostData( token_url, post ) ) ;
	m_access	= resp["access_token"].As<std::string>() ;
	m_refresh	= resp["refresh_token"].As<std::string>() ;
}

std::string OAuth2::MakeAuthURL(
	const std::string&	client_id,
	const std::string&	state )
{
	using gr::http::Escape ;

	return "https://accounts.google.com/o/oauth2/auth"
		"?scope=" +
			Escape( "https://www.googleapis.com/auth/userinfo.email" )		+ "+" + 
			Escape( "https://www.googleapis.com/auth/userinfo.profile" )	+ "+" +
			Escape( "https://docs.google.com/feeds/" )						+ "+" + 
			Escape( "https://docs.googleusercontent.com/" )					+ "+" + 
			Escape( "https://spreadsheets.google.com/feeds/" )				+ /*"+" +
			Escape( "https://www.googleapis.com/auth/drive.file/" )			+*/
		"&redirect_uri=urn:ietf:wg:oauth:2.0:oob"
		"&response_type=code"
		"&client_id=" + client_id ;
}

void OAuth2::Refresh( )
{
	std::string post =
		"refresh_token="	+ m_refresh +
		"&client_id="		+ m_client_id +
		"&client_secret="	+ m_client_secret +
		"&grant_type=refresh_token" ;

	Json resp	= Json::Parse( http::PostData( token_url, post ) ) ;
	m_access	= resp["access_token"].As<std::string>() ;
}

std::string OAuth2::RefreshToken( ) const
{
	return m_refresh ;
}

std::string OAuth2::AccessToken( ) const
{
	return m_access ;
}

std::string OAuth2::HttpHeader( ) const
{
	return "Authorization: Bearer " + m_access ;
}

} // end of namespace
