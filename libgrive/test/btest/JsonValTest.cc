/*
	webwrite: an GPL wiki-like website with in-place editing
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

#include "json/JsonParser.hh"
#include "json/Val.hh"
#include "json/ValBuilder.hh"
#include "json/ValWriter.hh"
#include "util/StringStream.hh"

#include <boost/test/unit_test.hpp>

using namespace gr ;

namespace
{
	struct F
	{
	} ;
}

BOOST_FIXTURE_TEST_SUITE( JsonValTest, F )

BOOST_AUTO_TEST_CASE( Test )
{
	ValBuilder b ;
	JsonParser::Parse( "{\"key\": 100 }", &b ) ;
	Val json = b.Result() ;
	
	BOOST_CHECK( json.Is<Val::Object>() ) ;
	BOOST_CHECK_EQUAL( json["key"].As<long long>(), 100 ) ;
	
	StringStream ss ;
	ValWriter wr( &ss ) ;
	json.Visit( &wr ) ;
	
	BOOST_CHECK_EQUAL( ss.Str(), "{\"key\":100}" ) ;
	
//	std::cout << ss.Str() << std::endl ;
}

BOOST_AUTO_TEST_SUITE_END()
