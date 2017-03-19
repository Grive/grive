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

#include "CurlAgent.hh"

#include "Error.hh"
#include "Header.hh"

#include "util/log/Log.hh"
#include "util/DataStream.hh"
#include "util/File.hh"

#include <boost/throw_exception.hpp>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <limits>
#include <streambuf>
#include <iostream>

#include <signal.h>

namespace {

using namespace gr::http ;
using namespace gr ;

std::size_t ReadFileCallback( void *ptr, std::size_t size, std::size_t nmemb, SeekStream *file )
{
	assert( ptr != 0 ) ;
	assert( file != 0 ) ;

	if ( size*nmemb > 0 )
		return file->Read( static_cast<char*>(ptr), size*nmemb ) ;

	return 0 ;
}

} // end of local namespace

namespace gr { namespace http {

struct CurlAgent::Impl
{
	CURL			*curl ;
	std::string		location ;
	bool			error ;
	std::string		error_headers ;
	std::string		error_data ;
	DataStream		*dest ;
	u64_t			total_download, total_upload ;
} ;

static struct curl_slist* SetHeader( CURL* handle, const Header& hdr );

CurlAgent::CurlAgent() : Agent(),
	m_pimpl( new Impl ), m_pb( 0 )
{
	m_pimpl->curl = ::curl_easy_init();
}

void CurlAgent::Init()
{
	::curl_easy_reset( m_pimpl->curl ) ;
	::curl_easy_setopt( m_pimpl->curl, CURLOPT_SSL_VERIFYPEER,	0L ) ;
	::curl_easy_setopt( m_pimpl->curl, CURLOPT_SSL_VERIFYHOST,	0L ) ;
	::curl_easy_setopt( m_pimpl->curl, CURLOPT_HEADERFUNCTION,	&CurlAgent::HeaderCallback ) ;
	::curl_easy_setopt( m_pimpl->curl, CURLOPT_HEADERDATA,		this ) ;
	::curl_easy_setopt( m_pimpl->curl, CURLOPT_HEADER,			0L ) ;
	if ( mMaxUpload > 0 )
		::curl_easy_setopt( m_pimpl->curl, CURLOPT_MAX_SEND_SPEED_LARGE, mMaxUpload ) ;
	if ( mMaxDownload > 0 )
		::curl_easy_setopt( m_pimpl->curl, CURLOPT_MAX_RECV_SPEED_LARGE, mMaxDownload ) ;
	m_pimpl->error = false;
	m_pimpl->error_headers = "";
	m_pimpl->error_data = "";
	m_pimpl->dest = NULL;
	m_pimpl->total_download = m_pimpl->total_upload = 0;
}

CurlAgent::~CurlAgent()
{
	::curl_easy_cleanup( m_pimpl->curl );
}

ResponseLog* CurlAgent::GetLog() const
{
	return m_log.get();
}

void CurlAgent::SetLog(ResponseLog *log)
{
	m_log.reset( log );
}

void CurlAgent::SetProgressReporter(Progress *progress)
{
	m_pb = progress;
}

std::size_t CurlAgent::HeaderCallback( void *ptr, size_t size, size_t nmemb, CurlAgent *pthis )
{
	char *str = static_cast<char*>(ptr) ;
	std::string line( str, str + size*nmemb ) ;
	
	// Check for error (HTTP 400 and above)
	if ( line.substr( 0, 5 ) == "HTTP/" && line[9] >= '4' )
		pthis->m_pimpl->error = true;
	
	if ( pthis->m_pimpl->error )
		pthis->m_pimpl->error_headers += line;
	
	if ( pthis->m_log.get() )
		pthis->m_log->Write( str, size*nmemb );
	
	static const std::string loc = "Location: " ;
	std::size_t pos = line.find( loc ) ;
	if ( pos != line.npos )
	{
		std::size_t end_pos = line.find( "\r\n", pos ) ;
		pthis->m_pimpl->location = line.substr( pos+loc.size(), end_pos - loc.size() ) ;
	}
	
	return size*nmemb ;
}

std::size_t CurlAgent::Receive( void* ptr, size_t size, size_t nmemb, CurlAgent *pthis )
{
	assert( pthis != 0 ) ;
	if ( pthis->m_log.get() )
		pthis->m_log->Write( (const char*)ptr, size*nmemb );

	if ( pthis->m_pimpl->error && pthis->m_pimpl->error_data.size() < 65536 )
	{
		// Do not feed error responses to destination stream
		pthis->m_pimpl->error_data.append( static_cast<char*>(ptr), size * nmemb ) ;
		return size * nmemb ;
	}
	return pthis->m_pimpl->dest->Write( static_cast<char*>(ptr), size * nmemb ) ;
}

int CurlAgent::progress_callback( CurlAgent *pthis, curl_off_t totalDownload, curl_off_t finishedDownload, curl_off_t totalUpload, curl_off_t finishedUpload )
{
	// Only report download progress when set explicitly
	if ( pthis->m_pb )
	{
		totalDownload = pthis->m_pimpl->total_download;
		if ( !totalUpload )
			totalUpload = pthis->m_pimpl->total_upload;
		pthis->m_pb->reportProgress(
			totalDownload > 0 ? totalDownload : totalUpload,
			totalDownload > 0 ? finishedDownload : finishedUpload
		);
	}
	return 0;
}

long CurlAgent::ExecCurl(
	const std::string&	url,
	DataStream			*dest,
	const http::Header&	hdr )
{
	CURL *curl = m_pimpl->curl ;
	assert( curl != 0 ) ;

	char error[CURL_ERROR_SIZE] = {} ;
	::curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, 	error ) ;
	::curl_easy_setopt(curl, CURLOPT_URL, 			url.c_str());
	::curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,	&CurlAgent::Receive ) ;
	::curl_easy_setopt(curl, CURLOPT_WRITEDATA,		this ) ;
	m_pimpl->dest = dest ;

	struct curl_slist *slist = SetHeader( m_pimpl->curl, hdr ) ;

	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    #if LIBCURL_VERSION_NUM >= 0x072000
  	  curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
	  curl_easy_setopt(curl, CURLOPT_XFERINFODATA, this);
    #endif

	CURLcode curl_code = ::curl_easy_perform(curl);

	curl_slist_free_all(slist);

	// get the HTTP response code
	long http_code = 0;
	::curl_easy_getinfo(curl,	CURLINFO_RESPONSE_CODE, &http_code);
	Trace( "HTTP response %1%", http_code ) ;

	// reset the curl buffer to prevent it from touching our "error" buffer
	::curl_easy_setopt(curl,	CURLOPT_ERRORBUFFER, 	0 ) ;

	m_pimpl->dest = NULL;

	// only throw for libcurl errors
	if ( curl_code != CURLE_OK )
	{
		BOOST_THROW_EXCEPTION(
			Error()
				<< CurlCode( curl_code )
				<< Url( url )
				<< CurlErrMsg( error )
				<< HttpRequestHeaders( hdr )
		) ;
	}

	return http_code ;
}

long CurlAgent::Request(
	const std::string&	method,
	const std::string&	url,
	SeekStream			*in,
	DataStream			*dest,
	const Header&		hdr,
	u64_t			downloadFileBytes )
{
	Trace("HTTP %1% \"%2%\"", method, url ) ;

	Init() ;
	m_pimpl->total_download = downloadFileBytes ;
	CURL *curl = m_pimpl->curl ;

	// set common options
	::curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str() );
	if ( in )
	{
		::curl_easy_setopt(curl, CURLOPT_UPLOAD,			1L ) ;
		::curl_easy_setopt(curl, CURLOPT_READFUNCTION,		&ReadFileCallback ) ;
		::curl_easy_setopt(curl, CURLOPT_READDATA ,			in ) ;
		::curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, 	static_cast<curl_off_t>( in->Size() ) ) ;
	}

	return ExecCurl( url, dest, hdr ) ;
}

static struct curl_slist* SetHeader( CURL *handle, const Header& hdr )
{
	// set headers
	struct curl_slist *curl_hdr = 0 ;
    for ( Header::iterator i = hdr.begin() ; i != hdr.end() ; ++i )
		curl_hdr = curl_slist_append( curl_hdr, i->c_str() ) ;
	
	::curl_easy_setopt( handle, CURLOPT_HTTPHEADER, curl_hdr ) ;
	return curl_hdr;
}

std::string CurlAgent::LastError() const
{
	return m_pimpl->error_data ;
}

std::string CurlAgent::LastErrorHeaders() const
{
	return m_pimpl->error_headers ;
}

std::string CurlAgent::RedirLocation() const
{
	return m_pimpl->location ;
}

std::string CurlAgent::Escape( const std::string& str )
{
	CURL *curl = m_pimpl->curl ;
	
	char *tmp = curl_easy_escape( curl, str.c_str(), str.size() ) ;
	std::string result = tmp ;
	curl_free( tmp ) ;
	
	return result ;
}

std::string CurlAgent::Unescape( const std::string& str )
{
	CURL *curl = m_pimpl->curl ;
	
	int r ;
	char *tmp = curl_easy_unescape( curl, str.c_str(), str.size(), &r ) ;
	std::string result = tmp ;
	curl_free( tmp ) ;
	
	return result ;
}

} } // end of namespace
