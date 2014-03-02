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

#include "ConfigTest.hh"

#include "Assert.hh"

#include "util/Config.hh"
#include "protocol/Json.hh"
#include "util/log/Log.hh"

#include <boost/program_options.hpp>
#include <iostream>

using namespace grut;
using namespace gr ;
namespace po = boost::program_options;

ConfigTest::ConfigTest( )
{
}

void ConfigTest::TestInitialiseWithNoPath( )
{
	po::variables_map vm;
	po::notify(vm);

	Config config(vm);
	GRUT_ASSERT_EQUAL( "./.grive", config.Filename().string()) ;
}

void ConfigTest::TestInitialiseWithPath( )
{
	char const *argv[] = { "Program", "-p", "/home/grive" };
	int argc = 3;

	po::options_description desc( "Grive options" );
	desc.add_options()
		( "path,p",		po::value<std::string>(), "Path to sync")
		;

	po::variables_map vm;
	po::store(po::parse_command_line( argc, argv, desc), vm );
	po::notify(vm);

	Config config(vm);
	GRUT_ASSERT_EQUAL( "/home/grive/.grive", config.Filename().string()) ;
}

