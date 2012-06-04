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

#include "Entry.hh"

#include "xml/Node.hh"
#include "xml/NodeSet.hh"

#include <boost/iterator_adaptors.hpp>

#include <string>

namespace gr {

namespace http
{
	class Agent ;
	class Header ;
}

class Feed
{
public :
	class iterator ;

public :
	explicit Feed( const xml::Node& root ) ;
	void Assign( const xml::Node& root ) ;
	
	iterator begin() const ;
	iterator end() const ;
	
	std::string Next() const ;
	bool GetNext( http::Agent *http, const http::Header& auth ) ;
		
private :
	xml::Node		m_root ;
	xml::NodeSet	m_entries ;
} ;

class Feed::iterator : public boost::iterator_adaptor<
	Feed::iterator,
	xml::Node::iterator,
	Entry,
	boost::random_access_traversal_tag,
	Entry
>
{
public :
	iterator() ;
	explicit iterator( xml::Node::iterator i ) ;		

private :
	friend class boost::iterator_core_access;
	
	reference dereference() const ;
} ;

} // end of namespace
