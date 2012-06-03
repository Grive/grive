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

#include "Header.hh"

#include <algorithm>
#include <iterator>
#include <ostream>

namespace gr { namespace http {

Header::Header()
{
}

void Header::Add( const std::string& str )
{
	m_vec.push_back( str ) ;
}

Header::iterator Header::begin() const
{
	return m_vec.begin() ;
}

Header::iterator Header::end() const
{
	return m_vec.end() ;
}

std::ostream& operator<<( std::ostream& os, const Header& h )
{
	std::copy( h.begin(), h.end(), std::ostream_iterator<std::string>( os, "\n" ) ) ;
	return os ;
}

} } // end of namespace
