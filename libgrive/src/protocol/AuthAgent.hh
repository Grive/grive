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
	AuthAgent( OAuth2& auth, http::Agent* real_agent ) ;

	http::ResponseLog* GetLog() const ;
	void SetLog( http::ResponseLog *log ) ;

	long Request(
		const std::string&	method,
		const std::string&	url,
		SeekStream			*in,
		DataStream			*dest,
		const http::Header&	hdr,
		u64_t			downloadFileBytes = 0 ) ;
	
	std::string LastError() const ;
	std::string LastErrorHeaders() const ;
	
	std::string RedirLocation() const ;
	
	std::string Escape( const std::string& str ) ;
	std::string Unescape( const std::string& str ) ;

	void SetUploadSpeed( unsigned kbytes ) ;
	void SetDownloadSpeed( unsigned kbytes ) ;

	void SetProgressReporter( Progress *progress ) ;

private :
	http::Header AppendHeader( const http::Header& hdr ) const ;
	bool CheckRetry( long response ) ;
	long CheckHttpResponse(
		long 				response,
		const std::string&	url,
		const http::Header&	hdr  ) ;
	
private :
	OAuth2&		m_auth ;
	http::Agent*	m_agent ;
	int		m_interval ;
} ;

} // end of namespace
