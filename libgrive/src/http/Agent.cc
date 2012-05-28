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

#include "Agent.hh"

#include "Download.hh"
#include "Error.hh"
#include "Receivable.hh"

#include "util/Log.hh"

#include <boost/throw_exception.hpp>

// dependent libraries
#include <curl/curl.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <sstream>
#include <streambuf>
#include <iostream>

#include <signal.h>

namespace {

using namespace gr::http ;

size_t ReadCallback( void *ptr, std::size_t size, std::size_t nmemb, std::string *data )
{
	assert( ptr != 0 ) ;
	assert( data != 0 ) ;

	std::size_t count = std::min( size * nmemb, data->size() ) ;
	if ( count > 0 )
	{
		std::memcpy( ptr, &(*data)[0], count ) ;
		data->erase( 0, count ) ;
	}
	
	return count ;
}

// Callback for SIGINT
void CallbackInt( int )
{
	// TODO: instead of just disabling the signal, clean up the environment
	//       and exit gracefully
	std::cout << " Signal disabled while downloading file..\n";
}

} // end of local namespace

namespace gr { namespace http {

struct Agent::Impl
{
	CURL			*curl ;
	std::string		location ;
	std::string		log_prefix ;
} ;

Agent::Agent() :
	m_pimpl( new Impl )
{
	m_pimpl->curl = ::curl_easy_init();
}

void Agent::Init()
{
	::curl_easy_reset( m_pimpl->curl ) ;
	::curl_easy_setopt( m_pimpl->curl, CURLOPT_SSL_VERIFYPEER,	0L ) ; 
	::curl_easy_setopt( m_pimpl->curl, CURLOPT_SSL_VERIFYHOST,	0L ) ;
	::curl_easy_setopt( m_pimpl->curl, CURLOPT_HEADERFUNCTION,	&Agent::HeaderCallback ) ;
	::curl_easy_setopt( m_pimpl->curl, CURLOPT_WRITEHEADER ,	this ) ;
	::curl_easy_setopt( m_pimpl->curl, CURLOPT_HEADER, 			0L ) ;
}

Agent::~Agent()
{
	::curl_easy_cleanup( m_pimpl->curl );
}

void Agent::SetLogFile( const std::string& prefix )
{
	m_pimpl->log_prefix = prefix ;
}

std::size_t Agent::HeaderCallback( void *ptr, size_t size, size_t nmemb, Agent *pthis )
{
	char *str = reinterpret_cast<char*>(ptr) ;
	std::string line( str, str + size*nmemb ) ;
	
	static const std::string loc = "Location: " ;
	std::size_t pos = line.find( loc ) ;
	if ( pos != line.npos )
	{
		std::size_t end_pos = line.find( "\r\n", pos ) ;
		pthis->m_pimpl->location = line.substr( loc.size(), end_pos - loc.size() ) ;
	}
	
	return size*nmemb ;
}

std::size_t Agent::Receive( void* ptr, size_t size, size_t nmemb, Receivable *recv )
{
	assert( recv != 0 ) ;
	return recv->OnData( ptr, size * nmemb ) ;
}

long Agent::ExecCurl(
	const std::string&		url,
	Receivable				*dest,
	const http::Headers&	hdr )
{
	CURL *curl = m_pimpl->curl ;
	assert( curl != 0 ) ;

	char error[CURL_ERROR_SIZE] = {} ;
	::curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, 	error ) ;
	::curl_easy_setopt(curl, CURLOPT_URL, 			url.c_str());
	::curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,	&Agent::Receive ) ;
	::curl_easy_setopt(curl, CURLOPT_WRITEDATA,		dest ) ;

	SetHeader( hdr ) ;

	dest->Clear() ;
	CURLcode curl_code = ::curl_easy_perform(curl);

	long http_code = 0;
	::curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

	Trace( "HTTP response %1%", http_code ) ;
	::curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, 	0 ) ;
	
	if ( curl_code != CURLE_OK || http_code >= 400 )
	{
		BOOST_THROW_EXCEPTION(
			Error()
				<< CurlCode( curl_code )
				<< HttpResponse( http_code )
				<< Url( url )
				<< expt::ErrMsg( error )
		) ;
	}

	return http_code ;
}

long Agent::Put(
	const std::string&		url,
	const std::string&		data,
	Receivable				*dest,
	const http::Headers&	hdr )
{
	Trace("HTTP PUT \"%1%\"", url ) ;
	
	Init() ;
	CURL *curl = m_pimpl->curl ;

	std::string put_data = data ;

	// set common options
	::curl_easy_setopt(curl, CURLOPT_UPLOAD,		1L ) ;
	::curl_easy_setopt(curl, CURLOPT_READFUNCTION,	&ReadCallback ) ;
	::curl_easy_setopt(curl, CURLOPT_READDATA ,		&put_data ) ;
	::curl_easy_setopt(curl, CURLOPT_INFILESIZE, 	put_data.size() ) ;
	
	return ExecCurl( url, dest, hdr ) ;
}

long Agent::Get(
	const std::string& 		url,
	Receivable				*dest,
	const http::Headers&	hdr )
{
	Trace("HTTP GET \"%1%\"", url ) ;
	
	Init() ;
	CURL *curl = m_pimpl->curl ;

	// set get specific options
	::curl_easy_setopt(curl, CURLOPT_HTTPGET, 		1L);

	return ExecCurl( url, dest, hdr ) ;
}

long Agent::Post(
	const std::string& 		url,
	const std::string&		data,
	Receivable				*dest,
	const http::Headers&	hdr )
{
	Trace("HTTP POST \"%1%\" with \"%2%\"", url, data ) ;

	Init() ;
	CURL *curl = m_pimpl->curl ;

	// make a copy because the parameter is const
	std::string post_data = data ;
	
	// set post specific options
	::curl_easy_setopt(curl, CURLOPT_POST, 			1L);
	::curl_easy_setopt(curl, CURLOPT_POSTFIELDS,	&post_data[0] ) ;
	::curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, post_data.size() ) ;

	return ExecCurl( url, dest, hdr ) ;
}

long Agent::Custom(
	const std::string&		method,
	const std::string&		url,
	Receivable				*dest,
	const http::Headers&	hdr )
{
	Trace("HTTP %2% \"%1%\"", url, method ) ;

	CURL *curl = m_pimpl->curl ;

	::curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str() );

	return ExecCurl( url, dest, hdr ) ;
}

void Agent::SetHeader( const http::Headers& hdr )
{
	// set headers
	struct curl_slist *curl_hdr = 0 ;
    for ( Headers::const_iterator i = hdr.begin() ; i != hdr.end() ; ++i )
		curl_hdr = curl_slist_append( curl_hdr, i->c_str() ) ;
	
	::curl_easy_setopt( m_pimpl->curl, CURLOPT_HTTPHEADER, curl_hdr ) ;
}

std::string Agent::RedirLocation() const
{
	return m_pimpl->location ;
}

std::string Agent::Escape( const std::string& str )
{
	CURL *curl = m_pimpl->curl ;
	
	char *tmp = curl_easy_escape( curl, str.c_str(), str.size() ) ;
	std::string result = tmp ;
	curl_free( tmp ) ;
	
	return result ;
}

std::string Agent::Unescape( const std::string& str )
{
	CURL *curl = m_pimpl->curl ;
	
	int r ;
	char *tmp = curl_easy_unescape( curl, str.c_str(), str.size(), &r ) ;
	std::string result = tmp ;
	curl_free( tmp ) ;
	
	return result ;
}

} } // end of namespace
