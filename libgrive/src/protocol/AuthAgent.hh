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

#include "http/Agent.hh"
#include "OAuth2.hh"

#include <memory>

namespace gr {

/*!	\brief	An HTTP agent with support OAuth2
	
	This is a HTTP agent that provide support for OAuth2. It will also perform retries on
	certain HTTP errors.
*/
class AuthAgent : public http::Agent
{
public :
	AuthAgent( const OAuth2& auth, std::auto_ptr<http::Agent> real_agent ) ;

	long Put(
		const std::string&	url,
		const std::string&	data,
		DataStream			*dest,
		const http::Header&	hdr ) ;
	
	long Put(
		const std::string&	url,
		File*				file,
		DataStream			*dest,
		const http::Header&	hdr ) ;

	long Get(
		const std::string& 	url,
		DataStream			*dest,
		const http::Header&	hdr ) ;
	
	long Post(
		const std::string& 	url,
		const std::string&	data,
		DataStream			*dest,
		const http::Header&	hdr ) ;
	
	long Custom(
		const std::string&	method,
		const std::string&	url,
		DataStream			*dest,
		const http::Header&	hdr ) ;
	
	std::string RedirLocation() const ;
	
	std::string Escape( const std::string& str ) ;
	std::string Unescape( const std::string& str ) ;

private :
	http::Header AppendHeader( const http::Header& hdr ) const ;
	bool CheckRetry( long response ) ;
	long CheckHttpResponse(
		long 				response,
		const std::string&	url,
		const http::Header&	hdr  ) ;
	
private :
	OAuth2								m_auth ;
	const std::auto_ptr<http::Agent>	m_agent ;
} ;

} // end of namespace
