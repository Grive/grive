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

#include "util/DataStream.hh"

#include <fstream>
#include <string>

namespace gr { namespace http {

class ResponseLog : public DataStream
{
public :
	ResponseLog(
		const std::string&	prefix,
		const std::string&	suffix ) ;
	
	std::size_t Write( const char *data, std::size_t count ) ;
	std::size_t Read( char *data, std::size_t count ) ;
	
	void Reset( const std::string& prefix, const std::string& suffix ) ;
	
private :
	static std::string Filename( const std::string& prefix, const std::string& suffix ) ;
	
private :
	bool	m_enabled ;
	std::ofstream	m_log ;
} ;

} } // end of namespace
