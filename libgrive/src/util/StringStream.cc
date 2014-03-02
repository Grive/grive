/*
	webwrite: an GPL program to sync a local directory with Google Drive
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

#include "StringStream.hh"

#include <algorithm>

namespace gr {

namespace
{
	// the max size of the cached string. this is to prevent allocating
	// too much memory if client sends a line too long (i.e. DOS attack)
	const std::size_t capacity = 1024 ;
}

StringStream::StringStream( const std::string& init ) :
	m_str( init )
{
}

/// Read `size` bytes from the stream. Those bytes will be removed from
/// the underlying string by calling `std::string::erase()`. Therefore, it is
/// not a good idea to call Read() to read byte-by-byte.
std::size_t StringStream::Read( char *data, std::size_t size )
{
	// wow! no need to count count == 0
	std::size_t count = std::min( m_str.size(), size ) ;
	std::copy( m_str.begin(), m_str.begin() + count, data ) ;
	m_str.erase( 0, count ) ;
	return count ;
}

std::size_t StringStream::Write( const char *data, std::size_t size )
{
	std::size_t count = std::min( size, capacity - m_str.size() ) ;
	m_str.insert( m_str.end(), data, data+count ) ;
	return count ;
}

const std::string& StringStream::Str() const
{
	return m_str ;
}

void StringStream::Str( const std::string& str )
{
	m_str = str ;
}

} // end of namespace
