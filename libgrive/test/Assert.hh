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

#pragma once

#include <cppunit/TestAssert.h>

namespace grut {

	template <typename T1, typename T2>
	void AssertEquals(
		const T1&				expected,
		const T2& 				actual,
		CPPUNIT_NS::SourceLine	sourceLine,
		const std::string&		message )
	{
		if ( expected != actual )
		{
			CPPUNIT_NS::Asserter::failNotEqual(
				CPPUNIT_NS::assertion_traits<T1>::toString(expected),
				CPPUNIT_NS::assertion_traits<T2>::toString(actual),
				sourceLine,
				message );
		}
	}

	template <typename FwdIt1, typename FwdIt2>
	void AssertEquals(
		FwdIt1	actualFirst,
		FwdIt1	actualLast,
		FwdIt2	expectFirst,
		CPPUNIT_NS::SourceLine	sourceLine,
		const std::string&		message )
	{
		if ( !std::equal( actualFirst, actualLast, expectFirst ) )
		{
			std::ostringstream exp, act ;
			while ( actualFirst != actualLast )
			{
				act << *actualFirst++ << " " ;
				exp << *expectFirst++ << " " ;
			}
			
			CPPUNIT_NS::Asserter::failNotEqual(
				exp.str(),
				act.str(),
				sourceLine,
				message );
		}
	}
	
	inline void AssertEquals(
		const std::wstring&		expected,
		const std::wstring& 	actual,
		CPPUNIT_NS::SourceLine	sourceLine,
		const std::string&		message )
	{
		AssertEquals(
			std::string(expected.begin(), expected.end() ),
			std::string(actual.begin(), actual.end() ),
			sourceLine,
			message ) ;
	}
	
} // end of namespace

#define GRUT_ASSERT_RANGE_EQUAL(actualFirst, actualLast, expectFirst) \
	  ( grut::AssertEquals( (actualFirst),			\
	                       (actualLast),			\
	                       (expectFirst),			\
	                       CPPUNIT_SOURCELINE(),	\
	                       "["#actualFirst","#actualLast") == "#expectFirst) )

#define GRUT_ASSERT_EQUAL(expected, actual)	\
  ( grut::AssertEquals( (expected),				\
                       (actual),				\
                       CPPUNIT_SOURCELINE(),	\
                       #actual" == "#expected) )

#define GEUT_ASSERT_NULL(actual)				\
  ( grut::AssertEquals( ((void*)0),				\
                       (actual),				\
                       CPPUNIT_SOURCELINE(),	\
                       #actual" != 0" ) )
