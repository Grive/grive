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

namespace gr { namespace log {

DefaultLog::DefaultLog() :
	m_log( std::cerr )
{
// 	Enable(log::debug,		true) ;
// 	Enable(log::verbose,	true) ;
}

DefaultLog::DefaultLog( const std::string& filename ) :
	m_file( filename.c_str() ),
	m_log( m_file )
{
}

void DefaultLog::Log( const log::Fmt& msg, log::Serverity s )
{
	if ( IsEnabled(s) )
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

} } // end of namespace
