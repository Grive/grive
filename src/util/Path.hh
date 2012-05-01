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

#include <string>
#include <vector>

namespace gr {

class Path
{
private :
	typedef std::vector<std::string> DirVec ;

public :
	typedef DirVec::iterator		iterator ;
	typedef DirVec::const_iterator	const_iterator ;

public :
	// it will less troublesome to use if we can convert them automatically
	Path( const std::string& p = "." ) ;
	Path( const Path& path ) ;
	
	std::string Str() const ;
	Path operator/( const Path& path ) const ;
	Path operator/( const std::string& str ) const ;
	Path& operator/=( const Path& path ) ;
	Path& operator/=( const std::string& str ) ;

	iterator Begin() ;
	iterator End() ;
	const_iterator Begin() const ;
	const_iterator End() const ;

	friend std::ostream& operator<<( std::ostream& os, const Path& path ) ;
	
	// support std::string for both arguments also
	friend bool operator==( const Path& p1, const Path& p2 ) ;

private :
	static DirVec	Separate( std::string str ) ;
	
private :
	DirVec	m_paths ;
} ;

// this doesn't need to be friend. just call op== and ! it
bool operator!=( const Path& p1, const Path& p2 ) ;

} // end of namespace
