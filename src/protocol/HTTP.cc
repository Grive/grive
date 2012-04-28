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

#include "HTTP.hh"

#include "Download.hh"

// dependent libraries
#include <curl/curl.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#include <streambuf>

namespace {

using namespace gr::http ;

// libcurl callback to append to a string
std::size_t WriteCallback( char *data, size_t size, size_t nmemb, std::string *resp )
{
	assert( resp != 0 ) ;
	assert( data != 0 ) ;
	
	std::size_t count = size * nmemb ;
	resp->append( data, count ) ;
	return count ;
}

size_t ReadCallback( void *ptr, std::size_t size, std::size_t nmemb, std::string *data )
{
	assert( ptr != 0 ) ;
	assert( data != 0 ) ;

	std::size_t count = std::min( size * nmemb, data->size() ) ;
	if ( count > 0 )
	{
		std::memcpy( &(*data)[0], ptr, count ) ;
		data->erase( 0, count ) ;
	}
	return count ;
}

CURL* InitCurl( const std::string& url, std::string *resp, const Headers& hdr )
{
	CURL *curl = curl_easy_init();
	if ( curl == 0 )
		throw std::bad_alloc() ;

	// set common options
	curl_easy_setopt(curl, CURLOPT_URL, 			url.c_str());
	curl_easy_setopt(curl, CURLOPT_HEADER, 			0);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION,	1);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,	WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA,		resp ) ;
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER,	0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST,	0L);

	// set headers
	struct curl_slist *curl_hdr = 0 ;
    for ( Headers::const_iterator i = hdr.begin() ; i != hdr.end() ; ++i )
		curl_hdr = curl_slist_append( curl_hdr, i->c_str() ) ;
	curl_easy_setopt( curl, CURLOPT_HTTPHEADER, curl_hdr ) ;
	
	return curl ;
}

void DoCurl( CURL *curl )
{
	char error_buf[CURL_ERROR_SIZE] = {} ;
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, 	error_buf ) ;

	CURLcode curl_code = curl_easy_perform(curl);

	int http_code = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

	// clean up
	curl_easy_cleanup(curl);
	
	if ( curl_code != CURLE_OK )
	{
		throw Exception( curl_code, http_code, error_buf ) ;
	}
	else if (http_code >= 400 )
	{
		std::cout << "http error " << http_code << std::endl ;
		throw Exception( curl_code, http_code, error_buf ) ;
	}
}

} // end of local namespace

namespace gr { namespace http {

Exception::Exception( int curl_code, int http_code, const char *err_buf )
	: runtime_error( Format( curl_code, http_code, err_buf ) )
{
}

std::string Exception::Format( int curl_code, int http_code, const char *err_buf )
{
	std::ostringstream ss ;
	ss << "CURL code = " << curl_code << " HTTP code = " << http_code << " (" << err_buf << ")" ;
	return ss.str() ;
}

std::string Get( const std::string& url, const Headers& hdr )
{
	std::string resp ;
	CURL *curl = InitCurl( url, &resp, hdr ) ;
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	DoCurl( curl ) ;
	return resp;
}

void GetFile(
	const std::string&	url,
	const std::string&	filename,
	const Headers& 		hdr )
{
	Download dl( filename, Download::NoChecksum() ) ;
		
	CURL *curl = InitCurl( url, 0, hdr ) ;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,	&Download::Callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA,		&dl ) ;
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	DoCurl( curl ) ;
}

void GetFile(
	const std::string&	url,
	const std::string&	filename,
	std::string&		md5sum,
	const Headers& 		hdr )
{
}

std::string PostData( const std::string& url, const std::string& data, const Headers& hdr )
{
	std::string resp ;
	CURL *curl = InitCurl( url, &resp, hdr ) ;

	std::string post_data = data ;
	
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS,		&post_data[0] ) ;
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 	post_data.size() ) ;

	DoCurl( curl ) ;
	return resp;
}

std::string PostFile( const std::string& url, const std::string& filename, const Headers& hdr )
{
	std::string resp ;
	return resp;
}

std::string Put(
	const std::string&	url,
	const std::string&	data,
	const Headers&		hdr )
{
	std::string resp ;
	CURL *curl = InitCurl( url, &resp, hdr ) ;

	std::string put_data = data ;
	
	curl_easy_setopt(curl, CURLOPT_UPLOAD,			1);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION,	&ReadCallback ) ;
	curl_easy_setopt(curl, CURLOPT_READDATA ,		&put_data ) ;
	curl_easy_setopt(curl, CURLOPT_INFILESIZE, 		put_data.size() ) ;

	DoCurl( curl ) ;
	return resp;
}

std::string Escape( const std::string& str )
{
	CURL *curl = curl_easy_init();
	
	char *tmp = curl_easy_escape( curl, str.c_str(), str.size() ) ;
	std::string result = tmp ;
	curl_free( tmp ) ;
	
	curl_easy_cleanup(curl);
	
	return result ;
}

std::string Unescape( const std::string& str )
{
	CURL *curl = curl_easy_init();
	
	int r ;
	char *tmp = curl_easy_unescape( curl, str.c_str(), str.size(), &r ) ;
	std::string result = tmp ;
	curl_free( tmp ) ;
	
	curl_easy_cleanup(curl);
	
	return result ;
}

} } // end of namespace
