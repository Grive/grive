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

class Json ;

class Collection
{
public :
	explicit Collection( const Json& entry ) ;
	Collection( const std::string& title, const std::string& href, const std::string& parent ) ;
	
	// default copy ctor & op= are fine
	
	static bool IsCollection( const Json& entry ) ;
	
	const std::string& Title() const ;
	const std::string& Href() const ;
	const std::string& Parent() const ;

	void AddChild( Collection *child ) ;
	
	void Swap( Collection& coll ) ;

	void CreateSubDir( const std::string& prefix ) ;
	
private :
	static std::string Parent( const Json& entry ) ;
	
private :
	std::string					m_title ;
	std::string					m_href ;
	std::string					m_parent ;
	
	// not owned
	std::vector<Collection*>	m_child ;
} ;

} // end of namespace

namespace std
{
	void swap( gr::Collection& c1, gr::Collection& c2 ) ;
}
