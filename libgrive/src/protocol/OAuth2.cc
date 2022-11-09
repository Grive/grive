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

#include "json/ValResponse.hh"

#include "http/CurlAgent.hh"
#include "http/Header.hh"
#include "util/log/Log.hh"

#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>

// for debugging
#include <iostream>

namespace gr {

const std::string token_url		= "https://accounts.google.com/o/oauth2/token" ;

OAuth2::OAuth2(
	http::Agent* agent,
	const std::string& refresh_code,
	const std::string&	client_id,
	const std::string&	client_secret ) :
	m_refresh( refresh_code ),
	m_agent( agent ),
	m_client_id( client_id ),
	m_client_secret( client_secret )
{
	Refresh( ) ;
}

OAuth2::OAuth2(
	http::Agent* agent,
	const std::string&	client_id,
	const std::string&	client_secret ) :
	m_agent( agent ),
	m_port( 0 ),
	m_socket( -1 ),
	m_client_id( client_id ),
	m_client_secret( client_secret )
{
}

OAuth2::~OAuth2()
{
	if ( m_socket >= 0 )
	{
		close( m_socket );
		m_socket = -1;
	}
}

bool OAuth2::Auth( const std::string&	auth_code )
{
	std::string post =
		"code="				+ auth_code +
		"&client_id="		+ m_client_id +
		"&client_secret="	+ m_client_secret +
		"&redirect_uri=http%3A%2F%2Flocalhost:" + std::to_string( m_port ) + "%2Fauth" +
		"&grant_type=authorization_code" ;

	http::ValResponse  resp ;

	long code = m_agent->Post( token_url, post, &resp, http::Header() ) ;
	if ( code >= 200 && code < 300 )
	{
		Val jresp	= resp.Response() ;
		m_access	= jresp["access_token"].Str() ;
		m_refresh	= jresp["refresh_token"].Str() ;
	}
	else
	{
		Log( "Failed to obtain auth token: HTTP %1%, body: %2%",
			code, m_agent->LastError(), log::error ) ;
		return false;
	}

	return true;
}

std::string OAuth2::MakeAuthURL()
{
	if ( !m_port )
	{
		sockaddr_storage addr = { 0 };
		addr.ss_family = AF_INET;
		m_socket = socket( AF_INET, SOCK_STREAM, 0 );
		if ( m_socket < 0 )
			throw std::runtime_error( std::string("socket: ") + strerror(errno) );
		if ( bind( m_socket, (sockaddr*)&addr, sizeof( addr ) ) < 0 )
		{
			close( m_socket );
			m_socket = -1;
			throw std::runtime_error( std::string("bind: ") + strerror(errno) );
		}
		socklen_t len = sizeof( addr );
		if ( getsockname( m_socket, (sockaddr *)&addr, &len ) == -1 )
		{
			close( m_socket );
			m_socket = -1;
			throw std::runtime_error( std::string("getsockname: ") + strerror(errno) );
		}
		m_port = ntohs(((sockaddr_in*)&addr)->sin_port);
		if ( listen( m_socket, 128 ) < 0 )
		{
			close( m_socket );
			m_socket = -1;
			m_port = 0;
			throw std::runtime_error( std::string("listen: ") + strerror(errno) );
		}
	}
	return "https://accounts.google.com/o/oauth2/auth"
		"?scope=" + m_agent->Escape( "https://www.googleapis.com/auth/drive" ) +
		"&redirect_uri=http%3A%2F%2Flocalhost:" + std::to_string( m_port ) + "%2Fauth" +
		"&response_type=code"
		"&client_id=" + m_client_id ;
}

bool OAuth2::GetCode( )
{
	sockaddr_storage addr = { 0 };
	int peer_fd = -1;
	while ( peer_fd < 0 )
	{
		socklen_t peer_addr_size = sizeof( addr );
		peer_fd = accept( m_socket, (sockaddr*)&addr, &peer_addr_size );
		if ( peer_fd == -1 && errno != EAGAIN && errno != EINTR )
			throw std::runtime_error( std::string("accept: ") + strerror(errno) );
	}
	fcntl( peer_fd, F_SETFL, fcntl( peer_fd, F_GETFL, 0 ) | O_NONBLOCK );
	struct pollfd pfd = (struct pollfd){
		.fd = peer_fd,
		.events = POLLIN|POLLRDHUP,
	};
	char buf[4096];
	std::string request;
	while ( true )
	{
		pfd.revents = 0;
		poll( &pfd, 1, -1 );
		if ( pfd.revents & POLLRDHUP )
			break;
		int r = 1;
		while ( r > 0 )
		{
			r = read( peer_fd, buf, sizeof( buf ) );
			if ( r > 0 )
				request += std::string( buf, r );
			else if ( r == 0 )
				break;
			else if ( errno != EAGAIN && errno != EINTR )
				throw std::runtime_error( std::string("read: ") + strerror(errno) );
		}
		if ( r == 0 || ( r < 0 && request.find( "\n" ) > 0 ) ) // GET ... HTTP/1.1\r\n
			break;
	}
	bool ok = false;
	if ( request.substr( 0, 10 ) == "GET /auth?" )
	{
		std::string line = request;
		int p = line.find( "\n" );
		if ( p > 0 )
			line = line.substr( 0, p );
		p = line.rfind( " " );
		if ( p > 0 )
			line = line.substr( 0, p );
		p = line.find( "code=" );
		if ( p > 0 )
			line = line.substr( p+5 );
		p = line.find( "&" );
		if ( p > 0 )
			line = line.substr( 0, p );
		ok = Auth( line );
	}
	std::string response = ( ok
		? "Authenticated successfully. Please close the page"
		: "Authentication error. Please try again" );
	response = "HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html; charset=utf-8\r\n"
		"Connection: close\r\n"
		"\r\n"+
		response+
		"\r\n";
	write( peer_fd, response.c_str(), response.size() );
	close( peer_fd );
	return ok;
}

void OAuth2::Refresh( )
{
	std::string post =
		"refresh_token="	+ m_refresh +
		"&client_id="		+ m_client_id +
		"&client_secret="	+ m_client_secret +
		"&grant_type=refresh_token" ;

	http::ValResponse  resp ;

	long code = m_agent->Post( token_url, post, &resp, http::Header() ) ;

	if ( code >= 200 && code < 300 )
		m_access = resp.Response()["access_token"].Str() ;
	else
	{
		Log( "Failed to refresh auth token: HTTP %1%, body: %2%",
			code, m_agent->LastError(), log::error ) ;
		BOOST_THROW_EXCEPTION( AuthFailed() );
	}
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
