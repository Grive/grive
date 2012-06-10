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

#include "drive/Resource.hh"

#include "drive/Entry.hh"
#include "xml/Node.hh"

#include <iostream>

namespace grut {

using namespace gr ;

ResourceTest::ResourceTest( )
{
}

void ResourceTest::TestRootPath()
{
	Resource root ;
	CPPUNIT_ASSERT( root.IsRoot() ) ;
	GRUT_ASSERT_EQUAL( root.Path(), fs::path( "." ) ) ;
}

void ResourceTest::TestNormal( )
{
	Resource root( TEST_DATA, "folder" ) ;
	Resource subject( "entry.xml", "file" ) ;
	root.AddChild( &subject ) ;
	
	GRUT_ASSERT_EQUAL( subject.Path(), fs::path( TEST_DATA ) / "entry.xml" ) ;
	
	subject.FromLocal( DateTime() ) ;
	GRUT_ASSERT_EQUAL( subject.MD5(), "c0742c0a32b2c909b6f176d17a6992d0" ) ;
	GRUT_ASSERT_EQUAL( subject.StateStr(), "local_new" ) ;
	
	xml::Node entry = xml::Node::Element( "entry" ) ;
	entry.AddElement( "updated" ).AddText( "2012-05-09T16:13:22.401Z" ) ;
	
	Entry remote( entry ) ;
	subject.FromRemote( remote, DateTime() ) ;
	GRUT_ASSERT_EQUAL( subject.StateStr(), "local_changed" ) ;
}


} // end of namespace grut
