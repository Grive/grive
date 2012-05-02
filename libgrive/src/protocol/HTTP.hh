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

#include <memory>
#include <string>
#include <stdexcept>
#include <vector>

namespace gr {

	namespace http
	{
		typedef std::vector<std::string> Headers ;
		
		std::string Get( const std::string& url, const Headers& hdr = Headers() ) ;
		void GetFile(
			const std::string&	url,
			const std::string&	filename,
			const Headers& 		hdr = Headers() ) ;
		
		void GetFile(
			const std::string&	url,
			const std::string&	filename,
			std::string&		md5sum,
			const Headers& 		hdr = Headers() ) ;
		
		std::string PostData(
			const std::string&	url,
			const std::string&	data,
			const Headers&		hdr = Headers() ) ;
		std::string PostDataWithHeader(
			const std::string&	url,
			const std::string&	data,
			const Headers&		hdr = Headers() ) ;
		std::string PostFile(
			const std::string&	url,
			const std::string& 	filename,
			const Headers&		hdr = Headers() ) ;
		
		std::string Put(
			const std::string&	url,
			const std::string&	data,
			const Headers&		hdr = Headers() ) ;
		
		std::string Escape( const std::string& str ) ;
		std::string Unescape( const std::string& str ) ;
		
		class Exception : public std::runtime_error
		{
		public :
			Exception( int curl_code, int http_code, const char *err_buf ) ;
		
		private :
			static std::string Format( int curl_code, int http_code, const char *err_buf ) ;
		} ;
	}
	
	class Http
	{
	public :
		Http() ;
		~Http() ;
		
		std::string Put(
			const std::string&		url,
			const std::string&		data,
			const http::Headers&	hdr = http::Headers() ) ;

		std::string RedirLocation() const ;
	
	private :
		static std::size_t HeaderCallback( void *ptr, size_t size, size_t nmemb, Http *pthis ) ;
	
	private :
		struct Impl ;
		std::auto_ptr<Impl>	m_pimpl ;
	} ;

} // end of namespace
