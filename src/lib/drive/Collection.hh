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

#include "util/Function.hh"

#include <string>
#include <vector>

namespace gr {

class Json ;
class Path ;

class Collection
{
public :
	explicit Collection( const Json& entry ) ;
	Collection( const std::string& title, const std::string& href ) ;
	
	// default copy ctor & op= are fine
	
	static bool IsCollection( const Json& entry ) ;
	static std::string ParentHref( const Json& entry ) ;
	
	const std::string& Title() const ;
	const std::string& Href() const ;
	const Collection* Parent() const ;
	Collection* Parent() ;
	Path Dir() const ;

	void AddChild( Collection *child ) ;
	void AddLeaf( const std::string& filename ) ;
	
	void Swap( Collection& coll ) ;

	// traversing the tree
	void CreateSubDir( const Path& prefix ) ;
	void ForEachFile(
		Function<void(const std::string&)>	callback,
		const std::string& 					prefix = "." ) ;
	
private :
	std::string					m_title ;
	std::string					m_href ;
	
	// not owned
	Collection					*m_parent ;
	std::vector<Collection*>	m_child ;
	
	std::vector<std::string>	m_leaves ;
} ;

} // end of namespace

namespace std
{
	void swap( gr::Collection& c1, gr::Collection& c2 ) ;
}
