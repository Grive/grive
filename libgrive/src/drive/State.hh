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

#include "ResourceTree.hh"

#include "http/Agent.hh"
#include "util/DateTime.hh"
#include "util/FileSystem.hh"

#include <memory>

namespace gr {

class Json ;
class Resource ;
class Entry ;

class State
{
public :
	typedef ResourceTree::iterator iterator ;

public :
	explicit State( const fs::path& filename, const Json& options ) ;
	
	void FromLocal( const fs::path& p ) ;
	void FromRemote( const Entry& e ) ;
	void ResolveEntry() ;
	
	void Read( const fs::path& filename ) ;
	void Write( const fs::path& filename ) const ;

	Resource* FindByHref( const std::string& href ) ;
	Resource* FindByID( const std::string& id ) ;
	Resource* Find( const fs::path& path ) ;

	void Sync( http::Agent *http, const http::Headers& auth ) ;
	
	iterator begin() ;
	iterator end() ;
	
private :
	void FromLocal( const fs::path& p, Resource *folder ) ;
	bool Update( const Entry& e ) ;
	std::size_t TryResolveEntry() ;

private :
	ResourceTree		m_res ;
	DateTime			m_last_sync ;
	
	std::vector<Entry>	m_unresolved ;
} ;

} // end of namespace
