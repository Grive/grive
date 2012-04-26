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
#include <stdexcept>
#include <vector>

namespace gr
{
	typedef std::vector<std::string> Headers ;

	std::string HttpGet( const std::string& url, const Headers& hdr = Headers() ) ;
	void HttpGetFile(
		const std::string&	url,
		const std::string&	filename,
		const Headers& 		hdr = Headers() ) ;
	
	void HttpGetFile(
		const std::string&	url,
		const std::string&	filename,
		std::string&		md5sum,
		const Headers& 		hdr = Headers() ) ;
	
	std::string HttpPostData(
		const std::string&	url,
		const std::string&	data,
		const Headers&		hdr = Headers() ) ;
	std::string HttpPostFile(
		const std::string&	url,
		const std::string& 	filename,
		const Headers&		hdr = Headers() ) ;
	
	std::string Escape( const std::string& str ) ;
	std::string Unescape( const std::string& str ) ;
	
	class HttpException : public std::runtime_error
	{
	public :
		HttpException( int curl_code, int http_code, const char *err_buf ) ;
	
	private :
		static std::string Format( int curl_code, int http_code, const char *err_buf ) ;
	} ;
}