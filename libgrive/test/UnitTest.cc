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

#include "util/log/DefaultLog.hh"

#include "drive/EntryTest.hh"
#include "drive/ResourceTest.hh"
#include "drive/ResourceTreeTest.hh"
#include "drive/StateTest.hh"
#include "util/DateTimeTest.hh"
#include "util/FunctionTest.hh"
#include "util/SignalHandlerTest.hh"
#include "xml/NodeTest.hh"

int main( int argc, char **argv )
{
	using namespace grut ;
	
	gr::LogBase::Inst( std::auto_ptr<gr::LogBase>(new gr::log::DefaultLog) ) ;
	
	CppUnit::TextUi::TestRunner runner;
	runner.addTest( EntryTest::suite( ) ) ;
	runner.addTest( StateTest::suite( ) ) ;
	runner.addTest( ResourceTest::suite( ) ) ;
	runner.addTest( ResourceTreeTest::suite( ) ) ;
	runner.addTest( DateTimeTest::suite( ) ) ;
	runner.addTest( FunctionTest::suite( ) ) ;
	runner.addTest( SignalHandlerTest::suite( ) ) ;
	runner.addTest( NodeTest::suite( ) ) ;
	runner.run();
  
	return 0 ;
}
