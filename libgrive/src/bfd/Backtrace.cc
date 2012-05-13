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

#include "Backtrace.hh"

#include "SymbolInfo.hh"
#include "util/CArray.hh"

#include <algorithm>
#include <cstring>
#include <sstream>

namespace gr {

Backtrace::Backtrace( std::size_t skip ) :
	m_count( SymbolInfo::Instance()->Backtrace(m_stack, Count(m_stack) )),
	m_skip( std::min( skip, m_count ) )
{
}

Backtrace::Backtrace( const Backtrace& bt ) :
	m_count( bt.m_count ),
	m_skip( bt.m_skip )
{
	std::memcpy( m_stack, bt.m_stack, m_count * sizeof(m_stack[0]) ) ;
}

/*!	\brief	operator<< for printing backtraces
    \internal

	This function will call SymbolInfo::PrintTrace() to print out a backtrace
	to the stream. It will use the SymbolInfo::Instance() singleton to get
	the symbol information.
	\param	os	The output stream.
	\param	b	The backtrace information.
	\sa SymbolInfo::Backtrace(), SymbolInfo::Instance()
*/
std::ostream& operator<<( std::ostream& os, const gr::Backtrace& b )
{
	// the 1st function in the stack is SymbolInfo::Backtrace()
	// the 2nd one is the Backtrace() constructor
	// both are not interesting to user
	for ( std::size_t i = b.m_skip ; i < b.m_count ; i++ )
		SymbolInfo::Instance()->PrintTrace( b.m_stack[i], os, i - b.m_skip ) ;

	return os ;
}

std::string Backtrace::ToString( ) const
{
	std::ostringstream oss ;
	oss << *this ;
	return oss.str( ) ;
}

} // end of namespace
