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

#include "Log.hh"

#include <bitset>
#include <fstream>
#include <string>

namespace gr {

class DefaultLog : public LogBase
{
public :
	DefaultLog() ;
	explicit DefaultLog( const std::string& filename ) ;

	void Log( const log::Fmt& msg, log::Serverity s ) ;
	void Enable( log::Serverity s, bool enable ) ;
	
private :
	std::ofstream	m_file ;
	std::ostream&	m_log ;

	std::bitset<5>	m_enabled ;
} ;

} // end of namespace
