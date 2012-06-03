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

#include "Error.hh"
#include "NodeSet.hh"

#include <algorithm>
#include <cassert>
#include <functional>
#include <iterator>

// debugging
#include <iostream>

namespace gr { namespace xml {

class Node::Impl
{
private :
	typedef std::vector<Impl*> ImplVec ;

public :
	typedef ImplVec::iterator iterator ;
	typedef ImplVec::const_iterator const_iterator ;

public :
	Impl() : m_ref(1), m_type( element )
	{
	}
	
	Impl( const std::string& str, Type type, const std::string& value = "" ) :
		m_ref(1),
		m_type( type ),
		m_name( str ),
		m_value( value )
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
		assert( child->m_type >= element && child->m_type <= text ) ;
	
		ImplVec *map[] = { &m_element, &m_attr, 0 } ;
	
		if ( map[child->m_type] != 0 )
		{
			ImplVec& vec = *map[child->m_type] ;
			std::pair<iterator,iterator> p =
				std::equal_range( vec.begin(), vec.end(), child, Comp() ) ;

			// cannot allow duplicate attribute nodes
			if ( child->m_type	== attr && p.first != p.second )
				BOOST_THROW_EXCEPTION( Error() << expt::ErrMsg( "duplicate attribute " + child->m_name ) ) ;
			
			vec.insert( p.second, child ) ;
		}
		
		m_children.push_back( child ) ;
	}

	Range Find( const std::string& name )
	{
		assert( !name.empty() ) ;

		return name[0] == '@'
			? Find( m_attr, name.substr(1) ) 
			: Find( m_element, name ) ;
	}
	
	Impl* FindAttr( const std::string& name )
	{
		std::pair<iterator,iterator> r = Find( m_attr, name ) ;
		return r.first != r.second ? *r.first : 0 ;
	}
	
	iterator Begin()
	{
		return m_children.begin() ;
	}
	
	iterator End()
	{
		return m_children.end() ;
	}
	
	const_iterator Begin() const
	{
		return m_children.begin() ;
	}
	
	const_iterator End() const
	{
		return m_children.end() ;
	}
	
	std::size_t Size() const
	{
		return m_children.size() ;
	}
	
	Range Attr()
	{
		return std::make_pair( m_attr.begin(), m_attr.end() ) ;
	}
	
	const std::string& Name() const
	{
		return m_name ;
	}
	
	std::string Value() const
	{
		assert( m_type != element || m_value.empty() ) ;
	
		std::string value = m_value ;
		for ( const_iterator i = Begin() ; i != End() ; ++i )
			value += (*i)->Value() ;

		return value ;
	}
	
	void Value( const std::string& val )
	{
		m_value = val ;
	}
	
	Type GetType() const
	{
		return m_type ;
	}

	struct Comp
	{
		bool operator()( Impl *p1, Impl *p2 ) const
		{
			return p1->Name() < p2->Name() ;
		}
	} ;

private :
	Range Find( ImplVec& map, const std::string& name )
	{
		Impl tmp( name , element ) ;
		return std::equal_range( map.begin(), map.end(), &tmp, Comp() ) ;
	}

private :
	std::size_t		m_ref ;
	
	Type			m_type ;
	std::string		m_name ;
	std::string		m_value ;
	ImplVec			m_element, m_attr ;
	ImplVec			m_children ;
} ;

Node::iterator::iterator( )
{
}

Node::iterator::iterator( ImplVec::iterator i )
{
	// for some reason, gcc 4.4.4 doesn't allow me to initialize the base class
	// in the initializer. I have no choice but to initialize here.
	base_reference() = i ;
}

Node::iterator::reference Node::iterator::dereference() const
{
	Impl *p = *base_reference() ;
	assert( p != 0 ) ;
	
	return Node( p->AddRef() ) ;
}

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
	Swap( tmp ) ;
	return *this ;
}

void Node::Swap( Node& node )
{
	std::swap( node.m_ptr, m_ptr ) ;
}

bool Node::IsCompatible( Type parent, Type child )
{
	static const bool map[][3] =
	{
		// element,	attr,	text
		{ true,		true,	true },		// element
		{ false,	false,	true },		// attribute
		{ false,	false,	false }		// text
	} ;
	
	assert( parent >= element && parent <= text ) ;
	assert( child  >= element && child  <= text ) ;
	return map[parent][child] ;
}

Node Node::AddElement( const std::string& name )
{
	assert( m_ptr != 0 ) ;
	assert( IsCompatible( GetType(), element) ) ;
	
	Impl *child = new Impl( name, element ) ;
	m_ptr->Add( child->AddRef() ) ;
	return Node( child ) ;
}

Node Node::AddText( const std::string& str )
{
	assert( m_ptr != 0 ) ;
	assert( IsCompatible( GetType(), text ) ) ;

	Impl *child = new Impl( "#text", text, str ) ;
	m_ptr->Add( child->AddRef() ) ;
	return Node( child ) ;
}

void Node::AddAttribute( const std::string& name, const std::string& val )
{
	assert( m_ptr != 0 ) ;
	assert( GetType() == element ) ;
	m_ptr->Add( new Impl( name, attr, val ) ) ;
}

void Node::AddNode( const Node& node )
{
	assert( m_ptr != 0 ) ;
	assert( node.m_ptr != 0 ) ;
	assert( IsCompatible( GetType(), node.GetType() ) ) ;
	
	m_ptr->Add( node.m_ptr->AddRef() ) ;
}

void Node::AddNode( iterator first, iterator last )
{
	for ( iterator i = first ; i != last ; ++i )
		AddNode( *i ) ;
}

NodeSet Node::operator[]( const std::string& name ) const
{
	assert( m_ptr != 0 ) ;
	assert( !name.empty() ) ;
	
	Range is = m_ptr->Find( name ) ;
	return NodeSet( iterator(is.first), iterator(is.second) ) ;
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

const std::string& Node::Name() const
{
	assert( m_ptr != 0 ) ;
	return m_ptr->Name() ;
}

std::string Node::Value() const
{
	assert( m_ptr != 0 ) ;
	
	return m_ptr->Value() ;
}

Node::operator std::string() const
{
	return Value() ;
}

bool Node::operator==( const std::string& value ) const
{
	return Value() == value ;
}

std::ostream& operator<<( std::ostream& os, const Node& node )
{
	if ( node.GetType() == Node::element )
	{
		os << '<' << node.Name() ;
		
		// print attributes
		NodeSet attrs = node.Attr() ;
		if ( !attrs.empty() )
			os << ' ' << attrs ;
		os << '>' ;
		
		// recursively print children
		for ( Node::iterator i = node.begin() ; i != node.end() ; ++i )
		{
			if ( (*i).GetType() != Node::attr )
				os << *i ;
		}
		os << "</" << node.Name() << '>' ;
	}
	else if ( node.GetType() == Node::attr )
	{
		os << node.Name() << "=\"" ;
		Node::PrintString( os, node.Value() ) ;
		os << "\"" ;
	}
	else
	{
		Node::PrintString( os, node.Value() ) ;
	}
	
	return os ;
}

std::ostream& Node::PrintString( std::ostream& os, const std::string& s )
{
	for ( std::string::const_iterator i = s.begin() ; i != s.end() ; ++i )
		Node::PrintChar( os, *i ) ;
	return os ;
}

std::ostream& Node::PrintChar( std::ostream& os, char c )
{
	switch ( c )
	{
		case '\"': 	os << "&quot;" ; break ;
		case '\'':	os << "&apos;" ; break ;
		case '&':	os << "&amp;" ; break ;
		case '<':	os << "&lt;" ; break ;
		case '>':	os << "&gt;" ; break ;
		default :	os << c ; break ;
	}
	return os ;
}

Node::iterator Node::begin() const
{
	assert( m_ptr != 0 ) ;
	return iterator( m_ptr->Begin() ) ;
}

Node::iterator Node::end() const
{
	assert( m_ptr != 0 ) ;
	return iterator( m_ptr->End() ) ;
}

std::size_t Node::size() const
{
	assert( m_ptr != 0 ) ;
	return m_ptr->Size() ;
}

NodeSet Node::Children() const
{
	assert( m_ptr != 0 ) ;
	return NodeSet( begin(), end() ) ;
}

NodeSet Node::Attr() const
{
	assert( m_ptr != 0 ) ;
	Range is = m_ptr->Attr() ;

	return NodeSet( iterator(is.first), iterator(is.second) ) ;
}

std::string Node::Attr( const std::string& attr ) const
{
	assert( m_ptr != 0 ) ;
	Impl *imp = m_ptr->FindAttr( attr ) ;
	return imp != 0 ? imp->Value() : "" ;
}

} } // end namespace
