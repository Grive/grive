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

#include "DefaultLog.hh"

#include <cassert>
#include <iostream>

namespace gr {

DefaultLog::DefaultLog() :
	m_log( std::cerr )
{
	m_enabled[log::debug]		= false ;
	m_enabled[log::verbose]		= false ;
	m_enabled[log::info]		= true ;
	m_enabled[log::warning]		= true ;
	m_enabled[log::error]		= true ;
	m_enabled[log::critical]	= true ;
}

DefaultLog::DefaultLog( const std::string& filename ) :
	m_file( filename.c_str() ),
	m_log( m_file )
{
	m_enabled[log::debug]		= false ;
	m_enabled[log::verbose]		= true ;
	m_enabled[log::info]		= true ;
	m_enabled[log::warning]		= true ;
	m_enabled[log::error]		= true ;
	m_enabled[log::critical]	= true ;
}

void DefaultLog::Log( const log::Fmt& msg, log::Serverity s )
{
	assert( s >= log::debug && s <= log::critical ) ;
	if ( m_enabled[s] )
	{
		switch ( s )
		{
			case log::debug:
			case log::info:
				m_log << msg << std::endl ;
				break ;
			
			default:
				m_log << msg << std::endl ;
				break ;
		}
	}
}

void DefaultLog::Enable( log::Serverity s, bool enable )
{
	assert( s >= log::debug && s <= log::critical ) ;
	m_enabled[s] = enable ;
}

} // end of namespace
