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

#include "Config.hh"

#include "util/StdioFile.hh"

#include <boost/program_options.hpp>

#include <iostream>
#include <iterator>

namespace gr {

Config::Config(const fs::path& configFile) :
	m_configFile(configFile),
	m_cfg( Read() )
{
	if (configFile.empty())
	{
		throw Error() << expt::ErrMsg("Config cannot be initalised with an empty string.");
	}
}

const fs::path& Config::ConfigFile() const
{
	return m_configFile ;
}

void Config::Save( )
{
	StdioFile file( m_configFile.string(), 0600 ) ;
	m_cfg.Write( file ) ;
}

Json& Config::Get()
{
	return m_cfg ;
}

Json Config::Read()
{
	try
	{
		return Json::ParseFile( m_configFile.string() ) ;
	}
	catch ( Exception& e )
	{
		return Json() ;
	}
}

} // end of namespace
