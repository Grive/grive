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
#include "json/Val.hh"

namespace boost
{
	namespace program_options
	{
		class variables_map ;
	}
}

namespace gr {

class Config
{
public :
	struct Error : virtual Exception {} ;
	typedef boost::error_info<struct FileTag, std::string>	File ;

	Config( const boost::program_options::variables_map& vm ) ;

	const fs::path Filename() const ;
	
	void Set( const std::string& key, const Val& value ) ;
	Val Get( const std::string& key ) const ;

	Val GetAll() const ;
	void Save() ;

private :
	Val Read( ) ;
	static fs::path GetPath( const fs::path& root_path ) ;

private :
	//! config file path
	fs::path	m_path;
	
	//! config values loaded from config file
	Val		m_file ;
	
	//! config values from command line
	Val		m_cmd ;
} ;
	
} // end of namespace
