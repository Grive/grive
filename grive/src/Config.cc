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

#include <iterator>

namespace gr {

const std::string& Config::Filename()
{
    static std::string cfg_path;
    char *cfg_base;
    bool fallback;
    
    fallback = false;
    cfg_base = getenv("XDG_CONFIG_HOME");
    if (!cfg_base)
    {
        fallback = true;
        cfg_base = getenv("HOME");
        /* Clutter the current directory if forced to */
        if (!cfg_base) 
            cfg_base = (char *)".";
    }

    cfg_path  = cfg_base;
    cfg_path += "/";
    if (fallback) /* Hidden folder */
        cfg_path += ".";
    cfg_path += "grive";
    cfg_path += "/";
    cfg_path += "config.json";

	return cfg_path ;
}

Config::Config() :
	m_cfg( Read( Filename() ) )
{
}

void Config::Save( )
{
	StdioFile file( Filename(), 0600 ) ;
	m_cfg.Write( file ) ;
}

Json& Config::Get()
{
	return m_cfg ;
}

Json Config::Read( const std::string& filename )
{
	try
	{
		return Json::ParseFile( filename ) ;
	}
	catch ( Exception& e )
	{
		return Json() ;
	}
}

} // end of namespace
