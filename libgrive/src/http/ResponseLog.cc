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

#include "ResponseLog.hh"

#include "util/DateTime.hh"

#include <cassert>

namespace gr { namespace http {

ResponseLog::ResponseLog(
	const std::string&	prefix,
	const std::string&	suffix,
	Receivable			*next ) :
	m_enabled	( true ),
	m_log		( Filename(prefix, suffix).c_str() ),
	m_next		( next )
{
	assert( m_next != 0 ) ;
}

ResponseLog::ResponseLog( Receivable *next ) :
	m_enabled	( false ),
	m_next		( next )
{
	assert( m_next != 0 ) ;
}

std::size_t ResponseLog::OnData( void *data, std::size_t count )
{
	if ( m_enabled )
	{
		assert( m_log.rdbuf() != 0 ) ;
		m_log.rdbuf()->sputn( reinterpret_cast<char*>(data), count ) ;
	}
	
	return m_next->OnData( data, count ) ;
}

void ResponseLog::Clear()
{
	assert( m_next != 0 ) ;
	m_next->Clear() ;
}

std::string ResponseLog::Filename( const std::string& prefix, const std::string& suffix )
{
	return prefix + DateTime::Now().Format( "%H%M%S" ) + suffix ;
}

void ResponseLog::Enable( bool enable )
{
	m_enabled = enable ;
}

void ResponseLog::Reset( const std::string& prefix, const std::string& suffix, Receivable *next )
{
	assert( next != 0 ) ;
	
	if ( m_log.is_open() )
		m_log.close() ;
	
	m_log.open( Filename(prefix, suffix).c_str() ) ;
	m_next		= next ;
	m_enabled	= true ;
}

}} // end of namespace
