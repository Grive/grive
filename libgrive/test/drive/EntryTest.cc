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

#include "EntryTest.hh"

#include "Assert.hh"

#include "drive/Entry.hh"
#include "xml/Node.hh"
#include "xml/NodeSet.hh"
#include "xml/TreeBuilder.hh"

#include <iostream>

namespace grut {

using namespace gr ;

EntryTest::EntryTest( )
{
}

void EntryTest::TestXml( )
{
	xml::Node root = xml::TreeBuilder::ParseFile( TEST_DATA "entry.xml" )  ;
	
	CPPUNIT_ASSERT( !root["entry"].empty() ) ;
	
	Entry subject( root["entry"].front() ) ;
	GRUT_ASSERT_EQUAL( "snes", subject.Title() ) ;
	GRUT_ASSERT_EQUAL( "\"WxYPGE8CDyt7ImBk\"", subject.ETag() ) ;
	GRUT_ASSERT_EQUAL( "https://docs.google.com/feeds/default/private/full/folder%3A0B5KhdsbryVeGMl83OEV1ZVc3cUE",
		subject.SelfHref() ) ;
	
	GRUT_ASSERT_EQUAL( 1U, subject.ParentHrefs().size() ) ;
	GRUT_ASSERT_EQUAL( "https://docs.google.com/feeds/default/private/full/folder%3A0B5KhdsbryVeGNEZjdUxzZHl3Sjg",
		subject.ParentHrefs().front() ) ;
	
	GRUT_ASSERT_EQUAL( "folder", subject.Kind() ) ;
}

} // end of namespace grut
