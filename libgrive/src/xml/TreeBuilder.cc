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
#include "Node.hh"

#include <expat.h>

#include <cassert>
#include <iostream>
#include <fstream>

namespace gr { namespace xml {

TreeBuilder::TreeBuilder() : m_stack( 1, Node() )
{
}

Node TreeBuilder::ParseFile( const std::string& file )
{
	::XML_Parser p = ::XML_ParserCreate( 0 ) ;
	::XML_SetElementHandler( p, &TreeBuilder::StartElement, &TreeBuilder::EndElement ) ;
	
	TreeBuilder tb ;
	::XML_SetUserData( p, &tb ) ;
	
	std::ifstream f( file.c_str() ) ;
	
	const std::size_t block_size = 10 ;
	std::size_t count = 0 ;
	while ( (count = f.rdbuf()->sgetn( (char*)::XML_GetBuffer( p, block_size ), block_size ) ) > 0 )
		XML_ParseBuffer( p, count, false ) ;
	XML_ParseBuffer( p, 0, true ) ;
	
	assert( tb.m_stack.size() == 1 ) ;
	return tb.m_stack.front() ;
}

Node TreeBuilder::Parse( const std::string& xml )
{
	::XML_Parser p = ::XML_ParserCreate( 0 ) ;
	::XML_SetElementHandler( p, &TreeBuilder::StartElement, &TreeBuilder::EndElement ) ;
	
	TreeBuilder tb ;
	::XML_SetUserData( p, &tb ) ;

	XML_Parse( p, xml.c_str(), xml.size(), true ) ;
	
	assert( tb.m_stack.size() == 1 ) ;
	return tb.m_stack.front() ;
}

void TreeBuilder::StartElement( void *pvthis, const char *name, const char **attr )
{
	assert( pvthis != 0 ) ;
	assert( name != 0 ) ;
	assert( attr != 0 ) ;

	TreeBuilder *pthis = reinterpret_cast<TreeBuilder*>(pvthis) ;
	
	Node n = pthis->m_stack.back().AddElement( name ) ;
	
	for ( std::size_t i = 0 ; attr[i] != 0 ; i += 2 )
	{
		assert( attr[i+1] != 0 ) ;
		n.AddAttribute( attr[i], attr[i+1] ) ;
	}
	
	pthis->m_stack.push_back( n ) ;
}

void TreeBuilder::EndElement( void* pvthis, const char* name )
{
	TreeBuilder *pthis = reinterpret_cast<TreeBuilder*>(pvthis) ;
	
	assert( pthis->m_stack.back().Name() == name ) ;
	pthis->m_stack.pop_back() ;
}

} } // end of namespace
