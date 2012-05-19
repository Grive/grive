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

#include "Entry.hh"
#include "util/Exception.hh"
#include "util/FileSystem.hh"

#include <string>
#include <vector>

namespace gr {

class File ;

class Resource
{
public :
	explicit Resource( const xml::Node& entry ) ;
	explicit Resource( const Entry& entry ) ;
	Resource( const std::string& title, const std::string& href ) ;
	Resource( const std::string& title, Resource *parent ) ;
	
	// default copy ctor & op= are fine
	
	std::string Title() const ;
	std::string SelfHref() const ;
	const Resource* Parent() const ;
	Resource* Parent() ;
	std::string ParentHref() const ;
	fs::path Dir() const ;
	bool IsInRootTree() const ;
	std::string ResourceID() const ;

	void AddChild( Resource *child ) ;
	void AddLeaf( File *file ) ;
	
	void Swap( Resource& coll ) ;

	// traversing the tree
	void CreateSubDir( const fs::path& prefix ) ;

	struct Error : virtual Exception {} ;

	Resource* FindChild( const std::string& title ) ;
	void Update( const Entry& e ) ;
	
private :
	Entry						m_entry ;
	
	// not owned
	Resource				*m_parent ;
	std::vector<Resource*>	m_child ;
	std::vector<File*>		m_leaf ;
} ;

} // end of namespace

namespace std
{
	void swap( gr::Resource& c1, gr::Resource& c2 ) ;
}
