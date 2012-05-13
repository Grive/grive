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

#include "Addr.hh"

#include <iosfwd>
#include <string>

namespace gr {

///	A shortcut to print out backtrace information
/**	\internal
	The sole reason for this class to exists is to provide the
	operator<< overload to allow:
	
\code
std::cout << Backtrace() << std::endl ;
\endcode
	
	\sa SymbolInfo
*/
class Backtrace
{
public :
	explicit Backtrace( std::size_t skip = 2 ) ;
	Backtrace( const Backtrace& bt ) ;
	
	friend std::ostream& operator<<( std::ostream& os,
	                                 const gr::Backtrace& bt ) ;

	std::string ToString( ) const ;

private :
	addr_t		m_stack[100] ;
	std::size_t	m_count, m_skip ;
} ;

} // end of namespace
