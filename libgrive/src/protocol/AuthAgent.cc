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

#include "AuthAgent.hh"

#include "http/Error.hh"
#include "http/Header.hh"
#include "util/log/Log.hh"
#include "util/OS.hh"
#include "util/File.hh"

#include <cassert>

namespace gr {

using namespace http ;

AuthAgent::AuthAgent( const OAuth2& auth, std::auto_ptr<Agent> real_agent ) :
	m_auth	( auth ),
	m_agent	( real_agent )
{
	assert( m_agent.get() != 0 ) ;
}

Header AuthAgent::AppendHeader( const Header& hdr ) const
{
	Header h(hdr) ;
	h.Add( "Authorization: Bearer " + m_auth.AccessToken() ) ;
	h.Add( "GData-Version: 3.0" ) ;
	return h ;
}

long AuthAgent::Put(
	const std::string&	url,
	const std::string&	data,
	DataStream			*dest,
	const Header&		hdr )
{
	long response;
	Header auth;
	do
	{
		auth = AppendHeader( hdr );
		response = m_agent->Put( url, data, dest, auth );
	} while ( CheckRetry( response ) );
	return CheckHttpResponse( response, url, auth );
}

long AuthAgent::Put(
	const std::string&	url,
	File				*file,
	DataStream			*dest,
	const Header&		hdr )
{
	long response;
	Header auth;
	while ( true )
	{
		auth = AppendHeader( hdr );
		response = m_agent->Put( url, file, dest, auth );
		if ( !CheckRetry( response ) )
			break;
		file->Seek( 0, SEEK_SET );
	}

	// On 410 Gone or 412 Precondition failed, recovery may be possible so don't
	// throw an exception
	if ( response == 410 || response == 412 )
		return response;
	
	return CheckHttpResponse( response, url, auth );
}

long AuthAgent::Get(
	const std::string& 	url,
	DataStream			*dest,
	const Header&		hdr )
{
	long response;
	Header auth;
	do
	{
		auth = AppendHeader( hdr );
		response = m_agent->Get( url, dest, auth );
	} while ( CheckRetry( response ) );
	return CheckHttpResponse( response, url, auth );
}

long AuthAgent::Post(
	const std::string& 	url,
	const std::string&	data,
	DataStream			*dest,
	const Header&		hdr )
{
	long response;
	Header auth;
	do
	{
		auth = AppendHeader( hdr );
		response = m_agent->Post( url, data, dest, auth );
	} while ( CheckRetry( response ) );
	return CheckHttpResponse( response, url, auth );
}

long AuthAgent::Custom(
	const std::string&	method,
	const std::string&	url,
	DataStream			*dest,
	const Header&		hdr )
{
	long response;
	Header auth;
	do
	{
		auth = AppendHeader( hdr );
		response = m_agent->Custom( method, url, dest, auth );
	} while ( CheckRetry( response ) );
	return CheckHttpResponse( response, url, auth );
}

std::string AuthAgent::LastError() const
{
	return m_agent->LastError() ;
}

std::string AuthAgent::LastErrorHeaders() const
{
	return m_agent->LastErrorHeaders() ;
}

std::string AuthAgent::RedirLocation() const
{
	return m_agent->RedirLocation() ;
}

std::string AuthAgent::Escape( const std::string& str )
{
	return m_agent->Escape( str ) ;
}

std::string AuthAgent::Unescape( const std::string& str )
{
	return m_agent->Unescape( str ) ;
}

bool AuthAgent::CheckRetry( long response )
{
	// HTTP 500 and 503 should be temporary. just wait a bit and retry
	if ( response == 500 || response == 503 )
	{
		Log( "request failed due to temporary error: %1% (body: %2%). retrying in 5 seconds",
			response, m_agent->LastError(), log::warning ) ;
			
		os::Sleep( 5 ) ;
		return true ;
	}
	
	// HTTP 401 Unauthorized. the auth token has been expired. refresh it
	else if ( response == 401 )
	{
		Log( "request failed due to auth token expired: %1% (body: %2%). refreshing token",
			response, m_agent->LastError(), log::warning ) ;
			
		os::Sleep( 5 ) ;
		m_auth.Refresh() ;
		return true ;
	}
	else
		return false ;
}

long AuthAgent::CheckHttpResponse(
		long 				response,
		const std::string&	url,
		const http::Header&	hdr  )
{
	// throw for other HTTP errors
	if ( response >= 400 )
	{
		BOOST_THROW_EXCEPTION(
			Error()
				<< HttpResponseCode( response )
				<< HttpResponseHeaders( m_agent->LastErrorHeaders() )
				<< HttpResponseText( m_agent->LastError() )
				<< Url( url )
				<< HttpRequestHeaders( hdr ) ) ;
	}
	
	return response ;
}

} // end of namespace
