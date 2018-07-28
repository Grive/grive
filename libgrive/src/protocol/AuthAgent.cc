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

AuthAgent::AuthAgent( OAuth2& auth, Agent *real_agent ) :
	Agent(),
	m_auth	( auth ),
	m_agent	( real_agent )
{
}

http::ResponseLog* AuthAgent::GetLog() const
{
	return m_agent->GetLog();
}

void AuthAgent::SetLog( http::ResponseLog *log )
{
	return m_agent->SetLog( log );
}

void AuthAgent::SetProgressReporter( Progress *progress )
{
	m_agent->SetProgressReporter( progress );
}

void AuthAgent::SetUploadSpeed( unsigned kbytes )
{
	m_agent->SetUploadSpeed( kbytes );
}

void AuthAgent::SetDownloadSpeed( unsigned kbytes )
{
	m_agent->SetDownloadSpeed( kbytes );
}

http::Header AuthAgent::AppendHeader( const http::Header& hdr ) const
{
	http::Header h(hdr) ;
	h.Add( "Authorization: Bearer " + m_auth.AccessToken() ) ;
	h.Add( "GData-Version: 3.0" ) ;
	return h ;
}

long AuthAgent::Request(
	const std::string&	method,
	const std::string&	url,
	SeekStream			*in,
	DataStream			*dest,
	const http::Header&	hdr,
	u64_t			downloadFileBytes )
{
	long response;
	Header auth;
	m_interval = 0;
	do
	{
		auth = AppendHeader( hdr );
		if ( in )
			in->Seek( 0, 0 );
		response = m_agent->Request( method, url, in, dest, auth, downloadFileBytes );
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
	// HTTP 403 is the result of API rate limiting. attempt exponential backoff and try again
	else if ( response == 429 || ( response == 403 && (
		m_agent->LastError().find("\"reason\": \"userRateLimitExceeded\",") != std::string::npos ||
		m_agent->LastError().find("\"reason\": \"rateLimitExceeded\",") != std::string::npos ) ) )
	{
		m_interval = m_interval <= 0 ? 1 : ( m_interval < 64 ? m_interval*2 : 120 );
		Log( "request failed due to rate limiting: %1% (body: %2%). retrying in %3% seconds",
			response, m_agent->LastError(), m_interval, log::warning ) ;
		os::Sleep( m_interval ) ;
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
