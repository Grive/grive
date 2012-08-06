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

namespace po = boost::program_options;

namespace gr {

const std::string	default_filename	= ".grive";
const char			*env_name			= "GR_CONFIG";
const std::string	default_root_folder = ".";

Config::Config( const po::variables_map& vm )
{
	m_cmd.Add( "log-xml",	Json(vm.count("log-xml") > 0) ) ;
	m_cmd.Add( "new-rev",	Json(vm.count("new-rev") > 0) ) ;
	m_cmd.Add( "force",		Json(vm.count("force") > 0 ) ) ;
	m_cmd.Add( "path",		Json(vm.count("path") > 0
		? vm["path"].as<std::string>()
		: default_root_folder ) ) ;
	
	m_path	= GetPath( fs::path(m_cmd["path"].Str()) ) ;
	m_file	= Read( ) ;
}

fs::path Config::GetPath( const fs::path& root_path )
{
	// config file will be (in order of preference)
	// value specified in environment string
	// value specified in defaultConfigFileName in path from commandline --path
	// value specified in defaultConfigFileName in current directory
	const char *env = ::getenv( env_name ) ;
	return root_path / (env ? env : default_filename) ;
}

const fs::path Config::Filename() const
{
	return m_path ;
}

void Config::Save( )
{
	StdioFile file( m_path.string(), 0600 ) ;
	m_file.Write( file ) ;
}

void Config::Set( const std::string& key, const Json& value )
{
	m_file.Add( key, value ) ;
}

Json Config::Get( const std::string& key ) const
{
	return m_cmd.Has(key) ? m_cmd[key] : m_file[key] ;
}

Json Config::GetAll() const
{
	Json::Object obj		= m_file.AsObject() ;
	Json::Object cmd_obj	= m_cmd.AsObject() ;
	
	for ( Json::Object::iterator i = cmd_obj.begin() ; i != cmd_obj.end() ; ++i )
		obj[i->first] = i->second ;
	
	return Json( obj ) ;
}

Json Config::Read()
{
	try
	{
		return Json::ParseFile( m_path.string() ) ;
	}
	catch ( Exception& e )
	{
		return Json() ;
	}
}

} // end of namespace
