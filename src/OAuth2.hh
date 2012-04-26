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

#pragma once

#include <string>

namespace gr {

class OAuth2
{
public :
	OAuth2(
		const std::string&	client_id,
		const std::string&	client_secret ) ;
	OAuth2(
		const std::string&	refresh_code,
		const std::string&	client_id,
		const std::string&	client_secret ) ;

	std::string Str() const ;
	
	static std::string MakeAuthURL(
		const std::string&	client_id,
		const std::string&	state = std::string() ) ;

	void Auth( const std::string& auth_code ) ;
	void Refresh( ) ;
		
	std::string RefreshToken( ) const ;
	std::string AccessToken( ) const ;
	
	// adding HTTP auth header
	std::string HttpHeader( ) const ;
	
private :
	std::string m_access ;
	std::string m_refresh ;
	
	const std::string	m_client_id ;
	const std::string	m_client_secret ;
} ;
	
} // end of namespace
