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

#include <cppunit/ui/text/TestRunner.h>

#include "util/DateTimeTest.hh"
#include "util/FunctionTest.hh"
#include "util/PathTest.hh"
#include "util/SignalHandlerTest.hh"

int main( int argc, char **argv )
{
	using namespace grut ;

	CppUnit::TextUi::TestRunner runner;
	runner.addTest( DateTimeTest::suite( ) ) ;
	runner.addTest( FunctionTest::suite( ) ) ;
	runner.addTest( PathTest::suite( ) ) ;
	runner.addTest( SignalHandlerTest::suite( ) ) ;
	runner.run();
  
	return 0 ;
}
