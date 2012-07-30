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

#include <iostream>

using namespace grut;
using namespace gr ;

ConfigTest::ConfigTest( )
{
}

void ConfigTest::TestInitialiseWithEmptyString( )
{
	Config config("");
	GRUT_ASSERT_EQUAL( "/home/.grive", config.Filename().string()) ;
}

void ConfigTest::TestInitialiseWithString( )
{
	Config config("/home/.grive");
	GRUT_ASSERT_EQUAL( "/home/.grive", config.Filename().string()) ;
}

void ConfigTest::TestInitialiseWithFileSystemPath( )
{
	fs::path path("/home");
	fs::path file(".grive");
	Config config(path / file);
	GRUT_ASSERT_EQUAL( "/home/.grive", config.Filename().string());
}


