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

#include "Node.hh"

#include <algorithm>
#include <cassert>
#include <functional>
#include <stdexcept>

namespace gr { namespace xml {

class Node::Impl
{
private :
	typedef std::vector<Impl*> ImplVec ;

public :
	typedef ImplVec::iterator iterator ;

public :
	Impl() : m_ref(1)
	{
	}
	
	Impl( const std::string& str, Type type ) :
		m_ref(1),
		m_type( type ),
		m_str( str )
	{
	}
	
	~Impl()
	{
		std::for_each( m_children.begin(), m_children.end(), std::mem_fun( &Impl::Release ) ) ;
	}

	Impl* AddRef()
	{
		++m_ref ;
		return this ;
	}
	
	void Release()
	{
		if ( --m_ref == 0 )
			delete this ;
	}
	
	std::size_t RefCount() const
	{
		assert( m_ref > 0 ) ;
		return m_ref ;
	}

	void Add( Impl *child )
	{
		assert( child != 0 ) ;
	
		if ( child->m_type != text )
			m_map.insert( std::lower_bound( m_map.begin(), m_map.end(), child, Comp() ), child ) ;
		
		m_children.push_back( child ) ;
	}

	Impl* Find( const std::string& str )
	{
		Impl tmp( str, element ) ;
		iterator i = std::lower_bound( m_map.begin(), m_map.end(), &tmp, Comp() ) ;
		return i != m_map.end() && (*i)->m_str == str ? *i : 0 ;
	}
	
	iterator Begin()
	{
		return m_children.begin() ;
	}
	
	iterator End()
	{
		return m_children.end() ;
	}
	
	const std::string& Str() const
	{
		return m_str ;
	}
	
	void Str( const std::string& str )
	{
		m_str = str ;
	}
	
	Type GetType() const
	{
		return m_type ;
	}

	struct Comp
	{
		bool operator()( Impl *p1, Impl *p2 ) const
		{
			return p1->Str() < p2->Str() ;
		}
	} ;

private :
	std::size_t		m_ref ;
	
	Type			m_type ;
	std::string		m_str ;
	ImplVec			m_map ;
	ImplVec			m_children ;
} ;

Node::Node() : m_ptr( new Impl )
{
}

Node::Node( const Node& node ) : m_ptr( node.m_ptr->AddRef() )
{
}

Node::Node( Impl *impl ) : m_ptr( impl )
{
}

Node Node::Element( const std::string& name )
{
	return Node( new Impl( name, element ) ) ;
}

Node Node::Text( const std::string& name )
{
	return Node( new Impl( name, text ) ) ;
}

Node::~Node()
{
	assert( m_ptr != 0 ) ;
	m_ptr->Release() ;
}

Node& Node::operator=( const Node& node )
{
	Node tmp( node ) ;
	std::swap( tmp.m_ptr, m_ptr ) ;
	return *this ;
}

Node Node::AddElement( const std::string& name )
{
	assert( m_ptr != 0 ) ;
	
	Impl *child = new Impl( name, element ) ;
	m_ptr->Add( child->AddRef() ) ;
	return Node( child ) ;
}

Node Node::AddText( const std::string& str )
{
	assert( m_ptr != 0 ) ;

	Impl *child = new Impl( str, text ) ;
	m_ptr->Add( child->AddRef() ) ;
	return Node( child ) ;
}

Node Node::operator[]( const std::string& name ) const
{
	assert( m_ptr != 0 ) ;
	Impl *i = m_ptr->Find( name ) ;
	if ( i != 0 )
		return Node( i->AddRef() ) ;

	throw std::runtime_error( "node " + name + " can't be found" ) ;
}

std::size_t Node::RefCount() const
{
	assert( m_ptr != 0 ) ;
	return m_ptr->RefCount() ;
}

Node::Type Node::GetType() const
{
	assert( m_ptr != 0 ) ;
	return m_ptr->GetType() ;
}

const std::string& Node::Str() const
{
	assert( m_ptr != 0 ) ;
	return m_ptr->Str() ;
}

} } // end namespace
