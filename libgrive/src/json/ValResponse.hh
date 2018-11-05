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

#include "util/DataStream.hh"

#include "JsonParser.hh"
#include "ValBuilder.hh"

namespace gr
{
	class Val ;
}

namespace gr { namespace http {

class ValResponse : public DataStream
{
public :
	ValResponse() ;

	std::size_t Write( const char *data, std::size_t count ) ;
	std::size_t Read( char *data, std::size_t count ) ;

	void Finish() ;
	Val Response() const ;
	
private :
	ValBuilder	m_val ;
	JsonParser	m_parser ;
} ;

} } // end of namespace gr::http

