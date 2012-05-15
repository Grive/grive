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

#include <fstream>
#include <iterator>

namespace gr {

const std::string& Config::Filename()
{
	static const char *env_cfg = ::getenv( "GR_CONFIG" ) ;
	static const std::string filename =
		(env_cfg != 0) ? env_cfg : std::string( ::getenv( "HOME") ) + "/.grive" ;

	return filename ;
}

Config::Config()
{
	std::ifstream ifile( Filename().c_str() ) ;
	if ( ifile )
	{
		try
		{
			std::string cfg_str(
				(std::istreambuf_iterator<char>( ifile )),
				(std::istreambuf_iterator<char>()) ) ;
			
			m_cfg = Json::Parse( cfg_str ) ;
		}
		catch ( Exception& e )
		{
			throw Error()
				<< File( Filename() )
				<< expt::ErrMsg("Cannot open config file ")
				<< expt::Nested(e) ;
		}
	}
}

void Config::Save( )
{
	std::ofstream ofile( Filename().c_str() ) ;
	ofile << m_cfg ;
}

Json& Config::Get()
{
	return m_cfg ;
}

} // end of namespace
