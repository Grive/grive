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

#include <cstdlib>
#include <iostream>

#include <time.h>

namespace gr {

DateTime::DateTime( ) :
	m_sec	( 0 ),
	m_usec	( 0 )
{
}

DateTime::DateTime( const std::string& iso ) :
	m_sec	( 0 ),
	m_usec	( 0 )
{
	struct tm tp ;
	const char *r = ::strptime( iso.c_str(), "%Y-%m-%dT%H:%M:%S.", &tp ) ;
	
	m_sec	= ::mktime( &tp ) ;
	if ( r != 0 )
		m_usec	= std::atoi( r ) * 1000 ;
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

unsigned long DateTime::USec( ) const
{
	return m_usec ;
}

} // end of namespace
