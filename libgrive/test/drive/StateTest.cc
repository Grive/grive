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

#include "StateTest.hh"

#include "Assert.hh"

#include "drive/State.hh"
#include "protocol/Json.hh"
#include "util/Log.hh"

#include <iostream>

namespace grut {

using namespace gr ;

StateTest::StateTest( )
{
}

void StateTest::TestSync( )
{
	State s( TEST_DATA "/test_dir1.state", Json() ) ;
	Resource *r = s.Find( "./folder1/abc.txt" ) ;
	CPPUNIT_ASSERT( r != 0 ) ;
	GRUT_ASSERT_EQUAL( r->Name(), "abc.txt" ) ;
	
	Trace( "state %1% = %2%", r->Name(), r->StateStr() ) ;

	// load directory
	s.FromLocal( TEST_DATA "/test_dir1" ) ;
}

} // end of namespace grut
