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
	const std::size_t capacity = 4096 ;
}

StringStream::StringStream( const std::string& init ) :
	// FIXME avoid copy
	m_str( init ), m_pos( 0 )
{
}

/// Read `size` bytes from the stream.
std::size_t StringStream::Read( char *data, std::size_t size )
{
	// wow! no need to count count == 0
	std::size_t count = std::min( m_str.size()-m_pos, size ) ;
	std::copy( m_str.begin() + m_pos, m_str.begin() + m_pos + count, data ) ;
	m_pos += count ;
	return count ;
}

std::size_t StringStream::Write( const char *data, std::size_t size )
{
	std::size_t count = std::min( size, capacity - m_str.size() ) ;
	m_str.replace( m_str.begin() + m_pos, m_str.begin() + m_pos + count, data, data+count ) ;
	m_pos += count ;
	return count ;
}

off_t StringStream::Seek( off_t offset, int whence )
{
	if ( whence == 1 )
		offset += m_pos;
	else if ( whence == 2 )
		offset += Size();
	if ( offset > Size() )
		offset = Size();
	m_pos = (size_t)offset;
	return m_pos;
}

off_t StringStream::Tell() const
{
	return m_pos;
}

u64_t StringStream::Size() const
{
	return m_str.size();
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
