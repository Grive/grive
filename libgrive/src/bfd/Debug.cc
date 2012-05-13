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

#include "Debug.hh"

#include <cassert>

#ifdef __GNUC__
#include <cxxabi.h>
#endif

#include <cstdlib>
#include <ostream>
#include <iomanip>

namespace gr {

std::string Demangle( const char *name )
{
#ifdef __GNUC__
	assert( name != 0 ) ;

	char *cname = abi::__cxa_demangle( name, 0, 0, 0 ) ;
	std::string result( name ) ;
	if ( cname != 0 )
	{
		result = cname ;
		std::free( cname ) ;
	}
	return result ;
#else
    return std::string( ) ;
#endif
}

std::ostream& PrintHex( std::ostream& os, const void *buf, std::size_t size )
{
	const unsigned char *raw = static_cast<const unsigned char*>( buf ) ;
	
	for ( std::size_t i = 0 ; i < size ; i++ )
	{
		if ( i % 8 == 0 && i > 0 )
			os << std::endl ;
		
		if ( i % 8 == 0 )
			os << std::hex << std::setw(8) << std::setfill('0') << i << " " ;
		
		os << "0x" << std::hex << std::setw(2) << std::setfill('0') 
			<< (int)raw[i] << ", " ;
	}
	os << std::endl ;
	return os ;
}

} // end of namespace
