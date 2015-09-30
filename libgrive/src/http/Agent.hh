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
#include "ResponseLog.hh"

namespace gr {

class DataStream ;
class File ;

namespace http {

class Header ;

class Agent
{
public :
	virtual ~Agent() {}

	virtual ResponseLog* GetLog() const = 0 ;
	virtual void SetLog( ResponseLog* ) = 0 ;

	virtual long Put(
		const std::string&	url,
		const std::string&	data,
		DataStream			*dest,
		const Header&		hdr ) = 0 ;

	virtual long Put(
		const std::string&	url,
		File				*file,
		DataStream			*dest,
		const Header&		hdr ) = 0 ;
		
	virtual long Get(
		const std::string& 	url,
		DataStream			*dest,
		const Header&		hdr ) = 0 ;
	
	virtual long Post(
		const std::string& 	url,
		const std::string&	data,
		DataStream			*dest,
		const Header&		hdr ) = 0 ;
	
	virtual long Custom(
		const std::string&	method,
		const std::string&	url,
		DataStream			*dest,
		const Header&		hdr ) = 0 ;
	
	virtual std::string LastError() const = 0 ;
	virtual std::string LastErrorHeaders() const = 0 ;
	
	virtual std::string RedirLocation() const = 0 ;
	
	virtual std::string Escape( const std::string& str ) = 0 ;
	virtual std::string Unescape( const std::string& str ) = 0 ;
} ;

} } // end of namespace
