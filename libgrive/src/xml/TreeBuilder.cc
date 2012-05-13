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

#include "TreeBuilder.hh"

#include "Error.hh"
#include "Node.hh"

#include <expat.h>

#include <cassert>
#include <iostream>
#include <fstream>

namespace gr { namespace xml {

struct TreeBuilder::Impl
{
	std::vector<Node>	stack ;
	::XML_Parser		psr ;
} ;

TreeBuilder::TreeBuilder() : m_impl( new Impl )
{
	m_impl->stack.push_back( Node() ) ;
	m_impl->psr = ::XML_ParserCreate( 0 ) ;
	
	::XML_SetElementHandler( m_impl->psr, &TreeBuilder::StartElement, &TreeBuilder::EndElement ) ;
	::XML_SetCharacterDataHandler( m_impl->psr, &TreeBuilder::OnCharData ) ;
	::XML_SetUserData( m_impl->psr , this ) ;
}

TreeBuilder::~TreeBuilder()
{
}

Node TreeBuilder::ParseFile( const std::string& file )
{
	TreeBuilder tb ;
	::XML_Parser p = tb.m_impl->psr ;
	
	std::ifstream f( file.c_str() ) ;
	
	const std::size_t block_size = 10 ;
	std::size_t count = 0 ;
	while ( (count = f.rdbuf()->sgetn( (char*)::XML_GetBuffer( p, block_size ), block_size ) ) > 0 )
		XML_ParseBuffer( p, count, false ) ;
	XML_ParseBuffer( p, 0, true ) ;
	
	return tb.Result() ;
}

void TreeBuilder::ParseData( const char *data, std::size_t count, bool last )
{
	if ( ::XML_Parse( m_impl->psr, data, count, last ) == 0 )
		throw Error() << expt::ErrMsg( "XML parse error" ) ;
}

Node TreeBuilder::Parse( const std::string& xml )
{
	TreeBuilder tb ;
	tb.ParseData( xml.c_str(), xml.size(), true ) ;
	return tb.Result() ;
}

Node TreeBuilder::Result() const
{
	// the node on the stack should be the dummy node with only one child
	assert( m_impl->stack.size() == 1 ) ;
	
	if ( m_impl->stack.front().size() != 1 )
		throw Error() << expt::ErrMsg( "invalid node" ) ;
		
	return *m_impl->stack.front().begin() ;
}

void TreeBuilder::StartElement( void *pvthis, const char *name, const char **attr )
{
	assert( pvthis != 0 ) ;
	assert( name != 0 ) ;
	assert( attr != 0 ) ;

	TreeBuilder *pthis = reinterpret_cast<TreeBuilder*>(pvthis) ;
	
	Node n = pthis->m_impl->stack.back().AddElement( name ) ;
	
	for ( std::size_t i = 0 ; attr[i] != 0 ; i += 2 )
	{
		assert( attr[i+1] != 0 ) ;
		n.AddAttribute( attr[i], attr[i+1] ) ;
	}
	
	pthis->m_impl->stack.push_back( n ) ;
}

void TreeBuilder::EndElement( void* pvthis, const char* name )
{
	TreeBuilder *pthis = reinterpret_cast<TreeBuilder*>(pvthis) ;
	
	assert( pthis->m_impl->stack.back().Name() == name ) ;
	pthis->m_impl->stack.pop_back() ;
}

void TreeBuilder::OnCharData( void *pvthis, const char *s, int len )
{
	TreeBuilder *pthis = reinterpret_cast<TreeBuilder*>(pvthis) ;
	pthis->m_impl->stack.back().AddText( std::string( s, len ) ) ;
}

} } // end of namespace
