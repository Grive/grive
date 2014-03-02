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

#include "NodeTest.hh"

#include "Assert.hh"

#include "xml/Node.hh"
#include "xml/NodeSet.hh"
#include "xml/TreeBuilder.hh"

#include <iostream>

namespace grut {

using namespace gr::xml ;

NodeTest::NodeTest( )
{
}

void NodeTest::TestTree( )
{
	Node node = Node::Element( "root" ) ;
	GRUT_ASSERT_EQUAL( 1UL, node.RefCount() ) ;
	GRUT_ASSERT_EQUAL( Node::element, node.GetType() ) ;
	
	Node c1 = node.AddElement( "child1" ) ;
	c1.AddText( "this is a line" ) ;
	Node c11 = c1.AddElement( "b" ) ;
	GRUT_ASSERT_EQUAL( 2UL, c1.RefCount() ) ;
	
	Node c2 = node.AddElement( "child2" ) ;
	Node c0 = node.AddElement( "child0" ) ;
	
	Node c1_ = node["child1"].front() ;
	Node c11_ = node["child1"]["b"].front() ;
	
	GRUT_ASSERT_EQUAL( 3UL, 			c1_.RefCount() ) ;
	GRUT_ASSERT_EQUAL( "child1",		c1_.Name() ) ;
}

void NodeTest::TestParseFile( )
{
	Node n ;
	n.AddNode( TreeBuilder::Parse( "<entry><link href=\"q\"><href>abc</href></link><link></link></entry>" ) ) ;
	
	
	GRUT_ASSERT_EQUAL( "entry",			n["entry"].front().Name() ) ;
	GRUT_ASSERT_EQUAL( "link",			n["entry"]["link"].front().Name() ) ;
	GRUT_ASSERT_EQUAL( "q",				n["entry"]["link"]["@href"].front().Value() ) ;
	GRUT_ASSERT_EQUAL( Node::element,	n["entry"]["link"]["href"].front().GetType() ) ;
	
	GRUT_ASSERT_EQUAL( "abc", n["entry"]["link"]["href"].front().Value() ) ;

	Node el = n["entry"]["link"].front() ;
	Node::iterator i = el.begin() ;
	while ( i != el.end() )
	{
		CPPUNIT_ASSERT_EQUAL( std::string("href"), (*i).Name() ) ;
		++i ;
	}
	
	NodeSet r = n["entry"]["link"] ;
	GRUT_ASSERT_EQUAL( 2U, r.size() ) ;
}

} // end of namespace grut
