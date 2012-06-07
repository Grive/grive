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

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

namespace gr
{
	namespace fs = boost::filesystem ;
	
	// these two functions are for ancient distro which does not have boost v1.44 or later
	// will be removed once people upgrade
	
	inline std::string Path2Str( const fs::path& p )
	{
		return p.string() ;
	}
	
	inline std::string Path2Str( const std::string& s )
	{
		return s ;
	}
}
