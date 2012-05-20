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
	explicit State( const fs::path& filename ) ;
	
	void FromLocal( const fs::path& p ) ;
	void FromRemote( const Entry& e ) ;
	void ResolveEntry() ;
	
	void Read( const fs::path& filename ) ;
	void Write( const fs::path& filename ) const ;

	std::string ChangeStamp() const ;
	void ChangeStamp( const std::string& cs ) ;
	
	Resource* FindFolderByHref( const std::string& href ) ;
	Resource* FindFolderByID( const std::string& id ) ;

	iterator begin() ;
	iterator end() ;

private :
	void FromLocal( const fs::path& p, Resource *folder ) ;
	bool Update( const Entry& e ) ;
	std::size_t TryResolveEntry() ;

private :
	ResourceTree		m_res ; ;
	std::string			m_change_stamp ;
	
	std::vector<Entry>	m_unresolved ;
} ;

} // end of namespace
