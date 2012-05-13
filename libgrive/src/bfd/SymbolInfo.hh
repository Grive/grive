/*
	grive: an GPL program to sync a local directory with Google Drive
	Copyright (C) 2006  Wan Wai Ho

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

#include <memory>
#include <iosfwd>

namespace gr {

///	ource code symbolic information
/**	\internal
	
	This class represents symbolic information about the source code,
	e.g. function names and line numbers. It provides an interface to
	lookup these informations by address.
*/
class SymbolInfo
{
public :
	SymbolInfo( ) ;
	~SymbolInfo( ) ;

    /*! \brief  singleton function
        \internal
        
        Returns the SymbolInfo singleton. Normally only one object
        of SymbolInfo is enough for one application, so a singleton
        is enough. This function will create the SymbolInfo object
        in the first call.
    */
	static SymbolInfo* Instance( ) ;

	std::size_t Backtrace( addr_t *stack, std::size_t count ) ;
	void PrintTrace( addr_t addr, std::ostream& os, std::size_t idx = 0 ) ;
	
private :
	struct Impl ;
	const std::auto_ptr<Impl> m_impl ;
	
	struct BacktraceInfo ;
	friend struct BacktraceInfo ;
} ;

} // end of namespace
