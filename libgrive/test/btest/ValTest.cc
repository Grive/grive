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

#include "json/Val.hh"
#include <boost/test/unit_test.hpp>

using namespace gr ;

namespace
{
	struct Fixture
	{
	} ;
}

BOOST_FIXTURE_TEST_SUITE( ValTest, Fixture )

BOOST_AUTO_TEST_CASE( TestSimpleTypes )
{
	Val null ;
	BOOST_CHECK_EQUAL( null.Type(), Val::null_type ) ;
	BOOST_CHECK( null.Is<void>() ) ;
	
	Val i( 100 ) ;
	BOOST_CHECK_EQUAL( i.As<long long>(), 100 ) ;
	BOOST_CHECK_EQUAL( i.Type(), Val::int_type ) ;
}

BOOST_AUTO_TEST_CASE( TestMap )
{
	Val obj(( Val::Object() )) ;
	obj.Add( "key", Val( std::string("value") ) ) ;
	BOOST_CHECK_EQUAL( obj["key"].Type(), Val::string_type ) ;
	BOOST_CHECK_EQUAL( obj["key"].As<std::string>(), "value" ) ;
}

BOOST_AUTO_TEST_SUITE_END()
