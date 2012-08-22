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

#include "Exception.hh"
#include "FileSystem.hh"
#include "Config.hh"

#include "protocol/Json.hh"

#include "Config.hh"


namespace boost
{
	namespace program_options
	{
		class variables_map ;
	}
}

namespace gr {

class Ignore
{
public :
	//struct Error : virtual Exception {} ;
	//typedef boost::error_info<struct FileTag, std::string>	File ;

	Ignore( const Config& cfg ) ;

	void Add( const std::string& filename );
	void Remove( const std::string& filename );
	
	bool Contains( const std::string& filename );
	
	void Save( ) ;

private :
	//Json Read( ) ;
	//static fs::path GetPath( const fs::path& root_path ) ;

private :
	Config m_cfg;
	Json ignoreList;
} ;
	
} // end of namespace
