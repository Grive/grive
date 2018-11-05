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

#pragma once

#include "DataStream.hh"

#include <string>

namespace gr {

/**	\brief	DataStream base on `std::string`s
	
	StringStream is a DataStream back-end that uses std::string for storage.
	It is useful for unit tests and text parsing, especially when used with
	StreamParser.
*/
class StringStream : public SeekStream
{
public :
	explicit StringStream( const std::string& init = std::string() ) ;

	std::size_t Read( char *data, std::size_t size ) ;
	std::size_t Write( const char *data, std::size_t size ) ;

	off_t Seek( off_t offset, int whence ) ;
	off_t Tell() const ;
	u64_t Size() const ;

	const std::string& Str() const ;
	void Str( const std::string& str ) ;

private :
	std::string	m_str ;
	std::size_t	m_pos ;
} ;

} // end of namespace
