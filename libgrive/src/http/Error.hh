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

#include "Header.hh"
#include "util/Exception.hh"

namespace gr { namespace http {

struct Error : virtual Exception {} ;

// CURL error code
typedef boost::error_info<struct CurlCodeTag, int>					CurlCode ;

// CURL error message
typedef boost::error_info<struct CurlErrMsgTag, std::string>		CurlErrMsg ;

// URL
typedef boost::error_info<struct UrlTag, std::string>				Url ;

// HTTP request headers
typedef boost::error_info<struct RequestHeadersTag, Header>			HttpRequestHeaders ;

// HTTP response code
typedef boost::error_info<struct ResponseCodeTag, int>				HttpResponseCode ;

// HTTP response headers
typedef boost::error_info<struct ResponseHeadersTag, std::string>	HttpResponseHeaders ;

// HTTP response body
typedef boost::error_info<struct ResponseBodyTag, std::string>		HttpResponseText ;

} } // end of namespace
