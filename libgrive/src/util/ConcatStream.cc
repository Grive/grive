/*
	A stream representing a concatenation of several underlying streams
	Copyright (C) 2015 Vitaliy Filippov

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

#include <string.h>
#include "ConcatStream.hh"

namespace gr {

ConcatStream::ConcatStream() :
	m_cur( 0 ), m_size( 0 ), m_pos( 0 )
{
}

std::size_t ConcatStream::Read( char *data, std::size_t size )
{
	std::size_t done = 0, l;
	while ( done < size && m_cur < m_streams.size() )
	{
		l = m_streams[m_cur]->Read( data+done, ( size-done < m_sizes[m_cur]-m_pos ? size-done : m_sizes[m_cur]-m_pos ) );
		done += l;
		m_pos += l;
		if ( !l )
		{
			// current stream was truncated in the meantime, pad output with zeros
			memset( data+done, 0, m_sizes[m_cur]-m_pos );
			done += m_sizes[m_cur]-m_pos;
			m_pos = m_sizes[m_cur];
		}
		if ( m_pos >= m_sizes[m_cur] )
		{
			m_cur++;
			if ( m_cur < m_streams.size() )
				m_streams[m_cur]->Seek( 0, 0 );
		}
	}
	return done ;
}

std::size_t ConcatStream::Write( const char *data, std::size_t size )
{
	return 0 ;
}

off_t ConcatStream::Seek( off_t offset, int whence )
{
	if ( whence == 1 )
		offset += m_pos;
	else if ( whence == 2 )
		offset += Size();
	if ( offset > Size() )
		offset = Size();
	m_cur = 0;
	m_pos = offset;
	if ( m_streams.size() )
	{
		while ( offset > m_sizes[m_cur] )
			m_cur++;
		m_streams[m_cur]->Seek( offset - ( m_cur > 0 ? m_sizes[m_cur-1] : 0 ), 0 );
	}
	return m_pos ;
}

off_t ConcatStream::Tell() const
{
	return m_pos ;
}

u64_t ConcatStream::Size() const
{
	return m_size ;
}

void ConcatStream::Append( SeekStream *stream )
{
	if ( stream )
	{
		off_t size = stream->Size();
		if ( size > 0 )
		{
			// "fix" stream size at the moment of adding so further changes of underlying files
			// don't affect the total size of resulting stream...
			m_size += size;
			m_streams.push_back( stream );
			m_sizes.push_back( m_size );
		}
	}
}

} // end of namespace
