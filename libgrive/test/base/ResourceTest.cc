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

#include "drive2/Entry2.hh"
#include "json/Val.hh"

#include <iostream>

namespace grut {

using namespace gr ;
using namespace gr::v2 ;

ResourceTest::ResourceTest( )
{
}

void ResourceTest::TestRootPath()
{
	std::string rootFolder = "/home/usr/grive/grive";
	Resource root( rootFolder ) ;
	CPPUNIT_ASSERT( root.IsRoot() ) ;
	GRUT_ASSERT_EQUAL( root.Path(), fs::path( rootFolder ) ) ;
}

void ResourceTest::TestNormal( )
{
	Resource root( TEST_DATA, "folder" ) ;
	Resource subject( "entry.xml", "file" ) ;
	root.AddChild( &subject ) ;
	
	GRUT_ASSERT_EQUAL( subject.IsRoot(), false ) ;
	GRUT_ASSERT_EQUAL( subject.Path(), fs::path( TEST_DATA ) / "entry.xml" ) ;
	
	Val st;
	st.Add( "srv_time", Val( DateTime( "2012-05-09T16:13:22.401Z" ).Sec() ) );
	subject.FromLocal( st ) ;
	GRUT_ASSERT_EQUAL( subject.MD5(), "c0742c0a32b2c909b6f176d17a6992d0" ) ;
	GRUT_ASSERT_EQUAL( subject.StateStr(), "local_new" ) ;
	
	Val entry;
	entry.Set( "modifiedDate", Val( std::string( "2012-05-09T16:13:22.401Z" ) ) );
	entry.Set( "md5Checksum", Val( std::string( "DIFFERENT" ) ) );
	
	Entry2 remote( entry ) ;
	GRUT_ASSERT_EQUAL( "different", remote.MD5() ) ;
	subject.FromRemote( remote ) ;
	GRUT_ASSERT_EQUAL( "local_changed", subject.StateStr() ) ;
}

} // end of namespace grut
