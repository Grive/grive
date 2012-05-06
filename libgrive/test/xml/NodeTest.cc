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

#include "xml/Node.hh"
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
	CPPUNIT_ASSERT_EQUAL( 1UL, node.RefCount() ) ;
	CPPUNIT_ASSERT_EQUAL( Node::element, node.GetType() ) ;
	
	Node c1 = node.AddElement( "child1" ) ;
	c1.AddText( "this is a line" ) ;
	Node c11 = c1.AddElement( "b" ) ;
	CPPUNIT_ASSERT_EQUAL( 2UL, c1.RefCount() ) ;
	
	Node c2 = node.AddElement( "child2" ) ;
	Node c0 = node.AddElement( "child0" ) ;
	
	Node c1_ = node["child1"] ;
	Node c11_ = node["child1"]["b"] ;
	
	CPPUNIT_ASSERT_EQUAL( 3UL, c1_.RefCount() ) ;
	CPPUNIT_ASSERT_EQUAL( std::string("child1"), c1_.Name() ) ;
}

void NodeTest::TestParseFile( )
{
	Node n = TreeBuilder::Parse( "<entry><link href=\"q\"><href>abc</href></link></entry>" ) ;
	CPPUNIT_ASSERT_EQUAL( std::string("entry"), n["entry"].Name() ) ;
	CPPUNIT_ASSERT_EQUAL( std::string("link"), n["entry"]["link"].Name() ) ;
	CPPUNIT_ASSERT_EQUAL( std::string("q"), n["entry"]["link"]["@href"].Value() ) ;
	CPPUNIT_ASSERT_EQUAL( Node::element, n["entry"]["link"]["href"].GetType() ) ;
	CPPUNIT_ASSERT_EQUAL( std::string("abc"), n["entry"]["link"]["href"].Value() ) ;

	Node el = n["entry"]["link"] ;
	Node::iterator i = el.begin() ;
	while ( i != el.end() )
	{
		CPPUNIT_ASSERT_EQUAL( std::string("href"), (*i).Name() ) ;
		++i ;
	}
}

} // end of namespace grut
