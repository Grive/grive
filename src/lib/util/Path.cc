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

#include "Path.hh"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <sstream>

#include <iostream>

namespace gr {

Path::Path( const std::string& p ) :
	m_paths( Separate(p) )
{
}

Path::Path( const Path& path ) :
	m_paths( path.m_paths )
{
}

std::string Path::Str() const
{
	// this is not the fastest way to implement it, but the easiest
	std::ostringstream ss ;
	ss << *this ;
	return ss.str() ;
}

Path::DirVec Path::Separate( std::string str )
{
	std::vector<std::string> result ;

	std::size_t n = str.find( '/' ) ;
	while ( n != str.npos )
	{
		std::string dir = str.substr( 0, n ) ;
		if ( !dir.empty() )
			result.push_back( dir ) ;
		
		str.erase( 0, n+1 ) ;
		n = str.find( '/' ) ;
	}
	result.push_back( str ) ;
	
	return result ;
}

std::ostream& operator<<( std::ostream& os, const Path& path )
{
	assert( !path.m_paths.empty() ) ;
	
	if ( path.m_paths.front() != "." )
		os << '/' ;
	
	std::copy( path.Begin(), path.End()-1, std::ostream_iterator<std::string>( os, "/" ) ) ;
	return os << path.m_paths.back() ;
}

Path& Path::operator/=( const Path& path )
{
	std::copy( path.Begin(), path.End(), std::back_inserter( m_paths ) ) ;
	return *this ;
}

Path& Path::operator/=( const std::string& path )
{
	m_paths.push_back( path ) ;
	return *this ;
}

Path Path::operator/( const Path& path ) const
{
	Path tmp(*this) ;
	tmp /= path ;
	return tmp ;
}

Path Path::operator/( const std::string& str ) const
{
	Path tmp(*this) ;
	tmp /= str ;
	return tmp ;
}

bool operator==( const Path& p1, const Path& p2 )
{
	return p1.m_paths == p2.m_paths ;
}

bool operator!=( const Path& p1, const Path& p2 )
{
	return !( p1 == p2 ) ;
}

Path::iterator Path::Begin()
{
	return m_paths.begin() ;
}

Path::iterator Path::End()
{
	return m_paths.end() ;
}

Path::const_iterator Path::Begin() const
{
	return m_paths.begin() ;
}

Path::const_iterator Path::End() const
{
	return m_paths.end() ;
}

} // end of namespace
