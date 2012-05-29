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

#include "DateTime.hh"

#include "Exception.hh"

// boost headers
#include <boost/throw_exception.hpp>
#include <boost/exception/errinfo_api_function.hpp>
#include <boost/exception/errinfo_at_line.hpp>
#include <boost/exception/errinfo_errno.hpp>
#include <boost/exception/info.hpp>

#include <sstream>

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>

#include <time.h>

namespace gr {

DateTime::DateTime( ) :
	m_sec	( 0 ),
	m_nsec	( 0 )
{
}

DateTime::DateTime( const std::string& iso ) :
	m_sec	( 0 ),
	m_nsec	( 0 )
{
	struct tm tp = {} ;
	const char *r = ::strptime( iso.c_str(), "%Y-%m-%dT%H:%M:%S", &tp ) ;

	// should be '.' followed by 3 digits and 'Z' (e.g. .123Z)
	if ( r != 0 && r - iso.c_str() == 19 )
	{
		m_sec	= ::timegm( &tp ) ;

		// at least 3 digits is OK. we don't care the Z
		if ( *r == '.' && ::strlen( r+1 ) >= 3 )
			m_nsec	= std::atoi( r+1 ) * 1000 * 1000 ;
	}
}

DateTime::DateTime( std::time_t sec, unsigned long nsec )
{
	Assign( sec, nsec ) ;
}

void DateTime::Assign( std::time_t sec, unsigned long nsec )
{
	m_sec	= sec + nsec / 1000000000 ;
	m_nsec	= nsec % 1000000000 ;
}

DateTime DateTime::Now()
{
	struct timeval tv = {} ;
	if ( ::gettimeofday( &tv, 0 ) != 0 )
	{
		BOOST_THROW_EXCEPTION(
			Exception()
				<< boost::errinfo_api_function("gettimeofday")
				<< boost::errinfo_errno(errno)
		) ;
	}
	
	return DateTime( tv.tv_sec, tv.tv_usec * 1000 ) ;
}

std::string DateTime::Format( const std::string& format ) const
{
	struct tm tp = Tm() ;

	char tmp[1024] ;
	std::size_t count = ::strftime( tmp, sizeof(tmp), format.c_str(), &tp ) ;
	return count > 0 ? std::string( tmp, count ) : "" ;	
}

struct tm DateTime::Tm() const
{
	struct tm tp ;
	gmtime_r( &m_sec, &tp ) ;
	return tp ;
}

std::time_t DateTime::Sec( ) const
{
	return m_sec ;
}

unsigned long DateTime::NanoSec( ) const
{
	assert( m_nsec < 1000000000 ) ;
	return m_nsec ;
}

std::ostream& operator<<( std::ostream& os, const DateTime& dt )
{
	struct tm tp = dt.Tm() ;

	char buf[40] ;
	strftime( buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &tp ) ;
	return os << buf << '.' << std::setw( 3 ) << std::setfill('0') << dt.NanoSec()/1000000 << 'Z' ;
}

struct timeval DateTime::Tv() const
{
	assert( m_nsec < 1000000000 ) ;
	
	timeval result ;
	result.tv_sec	= m_sec ;
	result.tv_usec	= m_nsec / 1000 ;
	return result ;
}

bool DateTime::operator==( const DateTime& dt ) const
{
	assert( m_nsec < 1000000000 ) ;
	return m_sec == dt.m_sec && m_nsec == dt.m_nsec ;
}

bool DateTime::operator!=( const DateTime& dt ) const
{
	return !( *this == dt ) ;
}

bool DateTime::operator>( const DateTime& dt ) const
{
	assert( m_nsec < 1000000000 ) ;
	assert( dt.m_nsec < 1000000000 ) ;
	return m_sec == dt.m_sec ? m_nsec > dt.m_nsec : m_sec > dt.m_sec ;
}

bool DateTime::operator>=( const DateTime& dt ) const
{
	return ( *this > dt ) || ( *this == dt ) ;
}

bool DateTime::operator<( const DateTime& dt ) const
{
	return !( *this >= dt ) ;
}

bool DateTime::operator<=( const DateTime& dt ) const
{
	return !( *this > dt ) ;
}

void DateTime::Swap( DateTime& dt )
{
	std::swap( m_sec, dt.m_sec ) ;
	std::swap( m_nsec, dt.m_nsec ) ;
}

std::string DateTime::ToString() const
{
	std::ostringstream ss ;
	ss << *this ;
	return ss.str() ;
}

} // end of namespace
