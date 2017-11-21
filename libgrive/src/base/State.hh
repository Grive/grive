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

#include "util/DateTime.hh"
#include "util/FileSystem.hh"
#include "json/Val.hh"

#include <memory>
#include <boost/regex.hpp>

namespace gr {

class Entry ;

class Syncer ;

class Resource ;

class State
{
public :
	typedef ResourceTree::iterator iterator ;

public :
	explicit State( const fs::path& root, const Val& options ) ;
	~State() ;
	
	void FromLocal( const fs::path& p ) ;
	void FromRemote( const Entry& e ) ;
	void ResolveEntry() ;
	
	void Read() ;
	void Write() ;

	Resource* FindByHref( const std::string& href ) ;
	Resource* FindByID( const std::string& id ) ;

	void Sync( Syncer *syncer, const Val& options ) ;
	
	iterator begin() ;
	iterator end() ;
	
	long ChangeStamp() const ;
	void ChangeStamp( long cstamp ) ;

private :
	bool ParseIgnoreFile( const char* buffer, int size ) ;
	void FromLocal( const fs::path& p, Resource *folder, Val& tree ) ;
	void FromChange( const Entry& e ) ;
	bool Update( const Entry& e ) ;
	std::size_t TryResolveEntry() ;

	bool IsIgnore( const std::string& filename ) ;
	
private :
	fs::path			m_root ;
	ResourceTree		m_res ;
	int					m_cstamp ;
	std::string			m_ign ;
	boost::regex		m_ign_re ;
	Val					m_st ;
	bool				m_force ;
	bool				m_ign_changed ;
	
	std::list<Entry>	m_unresolved ;
} ;

} // end of namespace gr
