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

#include "NodeSet.hh"

#include <algorithm>
#include <stdexcept>

namespace gr { namespace xml {

NodeSet::NodeSet( iterator first, iterator last ) :
	m_first( first ),
	m_last( last )
{
}
	
NodeSet::iterator NodeSet::begin() const
{
	return m_first ;
}

NodeSet::iterator NodeSet::end() const
{
	return m_last ;
}
	
Node NodeSet::Find( const std::string& attr, const std::string& value ) const
{
	for ( iterator i = m_first ; i != m_last ; ++i )
	{
		if ( i->Attr( attr ) == value )
			return *i ;
	}
	throw std::runtime_error( "can't find element with " + attr + " is " + value ) ;
}

NodeSet::operator Node() const
{
	return m_first != m_last ? *m_first : Node() ;
}

bool NodeSet::empty() const
{
	return m_first == m_last ;
}

std::size_t NodeSet::size() const
{
	return m_last - m_first ;
}

std::ostream& operator<<( std::ostream& os, const NodeSet& node )
{
	std::copy( node.begin(), node.end(), std::ostream_iterator<Node>(os, " ") ) ;
}

} } // end of namespace
