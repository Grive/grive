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

#include "ResourceTest.hh"

#include "Assert.hh"

#include "base/Resource.hh"

#include "drive/Entry1.hh"
#include "xml/Node.hh"
#include "json/Val.hh"

#include <iostream>

namespace grut {

using namespace gr ;
using namespace gr::v1 ;

ResourceTest::ResourceTest( )
{
}

void ResourceTest::TestRootPath()
{
  std::string rootFolder = "/home/usr/grive/grive";
	Resource root(rootFolder) ;
	CPPUNIT_ASSERT( root.IsRoot() ) ;
	GRUT_ASSERT_EQUAL( root.Path(), fs::path( rootFolder ) ) ;
}

void ResourceTest::TestNormal( )
{
	Resource root( TEST_DATA, "folder" ) ;
	Resource subject( "entry.xml", "file" ) ;
	root.AddChild( &subject ) ;
	
	GRUT_ASSERT_EQUAL( subject.Path(), fs::path( TEST_DATA ) / "entry.xml" ) ;
	
	Val st;
	subject.FromLocal( DateTime(), st ) ;
	GRUT_ASSERT_EQUAL( subject.MD5(), "c0742c0a32b2c909b6f176d17a6992d0" ) ;
	GRUT_ASSERT_EQUAL( subject.StateStr(), "local_new" ) ;
	
	xml::Node entry = xml::Node::Element( "entry" ) ;
	entry.AddElement( "updated" ).AddText( "2012-05-09T16:13:22.401Z" ) ;
	
	Entry1 remote( entry ) ;
	subject.FromRemote( remote ) ;
	GRUT_ASSERT_EQUAL( "local_changed", subject.StateStr() ) ;
}


} // end of namespace grut
