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

#include "FunctionTest.hh"

#include "util/Function.hh"

namespace grut {

using namespace gr ;

FunctionTest::FunctionTest( )
{
}

int TestFunction( int v )
{
	return v ;
}

void FunctionTest::TestRun( )
{
	Function<int(int)> f = &TestFunction ;
	Function<std::string()> f2 ;
	
	CPPUNIT_ASSERT_EQUAL( 3, f(3) ) ;
	CPPUNIT_ASSERT_EQUAL( std::string(), f2() ) ;
}

} // end of namespace grut
