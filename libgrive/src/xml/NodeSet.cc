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

#include <iostream>

namespace gr { namespace xml {

NodeSet::NodeSet() :
	m_first( m_tmp.begin() ),
	m_last( m_tmp.end() )
{
}

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

/*!	This function search the members in the node set. If any members in the node
	set has a children named \a name , with value equal to \a value , it will
	be returned.
	\param	name	name to be found. prefix with '@' for attributes
	\param	value	value to be matched.
	\return	the node set contained all children nodes that matches \a name and \a value
*/
NodeSet NodeSet::Find( const std::string& name, const std::string& value ) const
{
	NodeSet result ;
	for ( iterator i = m_first ; i != m_last ; ++i )
	{
		NodeSet cand = (*i)[name] ;
		for ( iterator j = cand.m_first ; j != cand.m_last ; ++j )
		{
			if ( j->Value() == value )
			{
				result.Add( *i ) ;
				break ;
			}
		}
	}
	return result ;
}

void NodeSet::Add( const Node& n )
{
	// the tmp node is not used, that means the first,last iterators points to elsewhere
	if ( m_tmp.size() == 0 )
	{
		m_tmp.AddNode( m_first, m_last ) ;
	}

	m_tmp.AddNode( n ) ;
	
	// the iterators may be invalidated after adding the node
	m_first = m_tmp.begin() ;
	m_last  = m_tmp.end() ;
}

NodeSet NodeSet::operator[]( const std::string& name ) const
{
	for ( iterator i = m_first ; i != m_last ; ++i )
	{
		NodeSet r = (*i)[name] ;
		if ( !r.empty() )
			return r ;
	}
	throw std::runtime_error( "can't find node " + name ) ;
}

Node NodeSet::front() const
{
	if ( empty() )
		throw std::runtime_error( "empty node set" ) ;
		
	return *m_first ;
}

NodeSet::operator std::string() const
{
	return empty() ? "" : front().Value() ;
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
