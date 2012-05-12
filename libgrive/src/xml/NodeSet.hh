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

#pragma once

#include "Node.hh"

#include <cstddef>
#include <iosfwd>
#include <string>

namespace gr { namespace xml {

class NodeSet
{
public :
	typedef Node::iterator iterator ;

public :
	NodeSet( iterator first, iterator last ) ;
	
	iterator begin() const ;
	iterator end() const ;
	bool empty() const ;
	std::size_t size() const ;
	Node front() const ;
	
	Node Find( const std::string& attr, const std::string& value ) const ;

	// forwarding common Node operations to Node
	operator std::string() const ;
	NodeSet operator[]( const std::string& name ) const ;
	
private :
	iterator	m_first ;
	iterator	m_last ;
} ;

std::ostream& operator<<( std::ostream& os, const NodeSet& node ) ;

} } // end of namespace
