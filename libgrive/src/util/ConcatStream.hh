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

#pragma once

#include "DataStream.hh"

#include <vector>

namespace gr {

class ConcatStream : public SeekStream
{
public :
	ConcatStream() ;

	std::size_t Read( char *data, std::size_t size ) ;
	std::size_t Write( const char *data, std::size_t size ) ;

	off_t Seek( off_t offset, int whence ) ;
	off_t Tell() const ;
	u64_t Size() const ;

	void Append( SeekStream *stream ) ;

private :
	std::vector<SeekStream*> m_streams ;
	off_t m_size, m_pos ;
	int m_cur ;
} ;

} // end of namespace
