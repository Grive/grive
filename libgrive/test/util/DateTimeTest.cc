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

#include "DateTimeTest.hh"

#include "util/DateTime.hh"

#include <cstring>

namespace grut {

using namespace gr ;

DateTimeTest::DateTimeTest( )
{
}

void DateTimeTest::TestParseIso( )
{
	DateTime	subject( "2009-07-29T20:31:39.804Z" ) ;
	struct tm tp = subject.Tm() ;
	CPPUNIT_ASSERT( tp.tm_year == 109 ) ;
	CPPUNIT_ASSERT( tp.tm_sec  == 39 ) ;
	CPPUNIT_ASSERT_EQUAL( 804000000UL, subject.NanoSec() ) ;
}

void DateTimeTest::TestParseNoMillisec( )
{
	DateTime	subject( "2009-07-29T20:31:39Z" ) ;
	CPPUNIT_ASSERT_EQUAL( 0UL, subject.NanoSec() ) ;
}

void DateTimeTest::TestParseInvalid( )
{
	DateTime	subject( "abcdefg" ) ;
	CPPUNIT_ASSERT_EQUAL( static_cast<time_t>(0), subject.Sec() ) ;
	CPPUNIT_ASSERT_EQUAL( 0UL, subject.NanoSec() ) ;
}

void DateTimeTest::TestOffByOne( )
{
	DateTime	subject( "2008-12-21T02:48:53.940Z" ) ;
	struct tm tp = subject.Tm() ;
	CPPUNIT_ASSERT_EQUAL( 21, tp.tm_mday ) ;
}

void DateTimeTest::TestCompare( )
{
	DateTime	s1( 1000, 2000 ), s2( 1001, 2000 ), s3( 1000, 2001 ), s4( 1001, 2000 ) ;
	
	CPPUNIT_ASSERT( s1 <  s3 ) ;
	CPPUNIT_ASSERT( s1 <= s3 ) ;
	CPPUNIT_ASSERT( s3 >  s1 ) ;
	CPPUNIT_ASSERT( s3 >= s1 ) ;
	
	CPPUNIT_ASSERT( s1 <  s2 ) ;
	CPPUNIT_ASSERT( s1 <= s2 ) ;
	CPPUNIT_ASSERT( s2 >  s1 ) ;
	CPPUNIT_ASSERT( s2 >= s1 ) ;
	
	CPPUNIT_ASSERT( s2 == s4 ) ;
	CPPUNIT_ASSERT( s2 >= s4 ) ;
	CPPUNIT_ASSERT( s2 <= s4 ) ;
	CPPUNIT_ASSERT( s4 == s2 ) ;
	CPPUNIT_ASSERT( s4 >= s2 ) ;
	CPPUNIT_ASSERT( s4 <= s2 ) ;
}

} // end of namespace grut
