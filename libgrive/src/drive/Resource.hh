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
#include "http/Agent.hh"
#include "util/Exception.hh"
#include "util/FileSystem.hh"

#include <string>
#include <vector>
#include <iosfwd>

namespace gr {

class Resource
{
public :
	explicit Resource( const xml::Node& entry ) ;
	explicit Resource( const Entry& entry, Resource *parent = 0 ) ;
	Resource( const std::string& title, const std::string& href ) ;
	Resource( const std::string& title, Resource *parent ) ;
	
	// default copy ctor & op= are fine
	
	std::string Title() const ;
	std::string SelfHref() const ;
	std::string ResourceID() const ;
	
	const Resource* Parent() const ;
	Resource* Parent() ;
	std::string ParentHref() const ;
	
	fs::path Path() const ;
	bool IsInRootTree() const ;

	bool IsFolder() const ;
	
	void AddChild( Resource *child ) ;
	void AddLeaf( Resource *file ) ;
	
	void Swap( Resource& coll ) ;

	// traversing the tree
	void CreateSubDir( const fs::path& prefix ) ;

	struct Error : virtual Exception {} ;

	Resource* FindChild( const std::string& title ) ;
	void Update( const Entry& e ) ;
	
	void Update( http::Agent *http, const http::Headers& auth ) ;
	void Delete( http::Agent* http, const http::Headers& auth ) ;

private :
	void Download( http::Agent* http, const fs::path& file, const http::Headers& auth ) const ;
	bool Upload( http::Agent* http, std::streambuf *file, const http::Headers& auth ) ;

private :
	Entry						m_entry ;
	
	// not owned
	Resource				*m_parent ;
	std::vector<Resource*>	m_child ;
	std::vector<Resource*>	m_leaf ;
} ;

} // end of namespace

namespace std
{
	void swap( gr::Resource& c1, gr::Resource& c2 ) ;
}
