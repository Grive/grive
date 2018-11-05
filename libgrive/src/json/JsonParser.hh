/*
	grive: an GPL program to sync a local directory with Google Drive
	Copyright (C) 2013 Wan Wai Ho

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
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
	MA  02110-1301, USA.
*/

#pragma once

#include "Val.hh"
#include "util/Exception.hh"
#include "util/DataStream.hh"

#include <string>
#include <memory>

namespace gr {

class ValVisitor ;

Val ParseJson( const std::string& json ) ;
Val ParseJson( DataStream &in ) ;

class JsonParser
{
public :
	struct Error : virtual Exception {} ;
	typedef boost::error_info<struct ParseErr,	std::string> ParseErr_ ;
	typedef boost::error_info<struct JsonText,	std::string> JsonText_ ;

	explicit JsonParser( ValVisitor *callback ) ;
	~JsonParser() ;
	
	void Parse( const char *str, std::size_t size ) ;
	void Parse( DataStream &in ) ;
	void Finish() ;
	
private :
	struct Impl ;
	std::unique_ptr<Impl>	m_impl ;
} ;

} // end of namespace

