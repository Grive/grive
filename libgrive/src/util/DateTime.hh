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

#include <ctime>
#include <string>
#include <iosfwd>

#include <sys/time.h>

namespace gr {

class DateTime
{
public :
	DateTime( ) ;
	explicit DateTime( const std::string& iso ) ;
	explicit DateTime( std::time_t sec, unsigned long nsec = 0 ) ;

	void Assign( std::time_t sec, unsigned long nsec = 0 ) ;
	
	static DateTime Now() ;
	
	std::time_t Sec( ) const ;
	unsigned long NanoSec( ) const ;
	std::string Format( const std::string& format ) const ;
	
	tm Tm() const ;
	timeval Tv() const ;
	
	bool operator==( const DateTime& dt ) const ;
	bool operator!=( const DateTime& dt ) const ;
	bool operator>( const DateTime& dt ) const ;
	bool operator>=( const DateTime& dt ) const ;
	bool operator<( const DateTime& dt ) const ;
	bool operator<=( const DateTime& dt ) const ;
	
	void Swap( DateTime& dt ) ;
	std::string ToString() const ;
	
private :
	std::time_t		m_sec ;
	unsigned long	m_nsec ;
} ;

std::ostream& operator<<( std::ostream& os, const DateTime& dt ) ;

} // end of namespace
