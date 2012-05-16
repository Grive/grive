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

#include "util/Gdbm.hh"

#include <boost/filesystem.hpp>

namespace gr {

class Json ;

class State
{
public :
	explicit State( const std::string& filename ) ;
	
	void Sync( const boost::filesystem::path& p ) ;

private :
	static Json FileInfo( const boost::filesystem::path& p ) ;
	
private :
	Gdbm	m_db ;
} ;

} // end of namespace
