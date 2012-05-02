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

#include "PathTest.hh"

#include "util/Path.hh"

namespace grut {

using namespace gr ;

PathTest::PathTest( )
{
}

void PathTest::TestStr( )
{
	Path relative = Path() / "dir1" / "dir2" ;
	CPPUNIT_ASSERT_EQUAL( std::string( "./dir1/dir2" ), relative.Str() ) ;

	Path absolute = Path( "usr" ) / "X11R6" / "lib" ;
	CPPUNIT_ASSERT_EQUAL( std::string( "/usr/X11R6/lib" ), absolute.Str() ) ;

}

void PathTest::TestSeparate( )
{
	Path relative( "./matchman/green///grive/dir" ) ;
	Path relative2 = Path() / "matchman" / "green" / "grive" / "dir" ;
	CPPUNIT_ASSERT_EQUAL( relative2, relative ) ;

	Path absolute( "////usr//local/grive/////dir" ) ;
	Path absolute2 = Path("usr") / "local" / "grive" / "dir" ;
	CPPUNIT_ASSERT_EQUAL( absolute2, absolute ) ;

}

} // end of namespace grut
