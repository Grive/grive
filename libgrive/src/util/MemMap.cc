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

#include "MemMap.hh"
#include "File.hh"

namespace gr {

MemMap::MemMap( File& file, off_t offset, std::size_t length ) :
	m_addr	( file.Map( offset, length ) ),
	m_length( length )
{
}

MemMap::~MemMap()
{
	File::UnMap( m_addr, m_length ) ;
}
	
void* MemMap::Addr() const
{
	return m_addr ;
}

std::size_t MemMap::Length() const
{
	return m_length ;
}

} // end of namespace
