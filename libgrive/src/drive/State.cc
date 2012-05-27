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

#include "State.hh"

#include "Resource.hh"
#include "CommonUri.hh"

#include "util/Crypt.hh"
#include "util/Log.hh"
#include "protocol/Json.hh"

#include <boost/bind.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>

#include <fstream>

namespace gr {

State::State( const fs::path& filename )
{
	Read( filename ) ;
}

std::string State::ChangeStamp() const
{
	return m_change_stamp ;
}

void State::ChangeStamp( const std::string& cs )
{
	m_change_stamp = cs ;
}

/// Synchronize local directory. Build up the resource tree from files and folders
/// of local directory.
void State::FromLocal( const fs::path& p )
{
	FromLocal( p, m_res.Root() ) ;
}

void State::FromLocal( const fs::path& p, gr::Resource* folder )
{
	assert( folder != 0 ) ;
	assert( folder->IsFolder() ) ;
	
	// sync the folder itself
	folder->FromLocal() ;

	for ( fs::directory_iterator i( p ) ; i != fs::directory_iterator() ; ++i )
	{
		std::string fname = i->path().filename().string() ;
	
		if ( fname[0] == '.' )
			Log( "file %1% is ignored by grive", fname, log::info ) ;
		
		else
		{
			// if the Resource object of the child already exists, it should
			// have been so no need to do anything here
			Resource *c = folder->FindChild( fname ) ;
			if ( c == 0 )
			{
				Log( "detected new file %1% in local", fname, log::verbose ) ;
				c = new Resource( i->path() ) ;
				folder->AddChild( c ) ;
				m_res.Insert( c ) ;
			}
			
			if ( fs::is_directory( i->path() ) )
				FromLocal( *i, c ) ;
		}
	}
}

void State::ShowFolders()
{
	for ( ResourceTree::iterator i = m_res.begin() ; i != m_res.end() ; ++i )
	{
		Resource *r = *i ;
		if ( r->IsFolder() )
			Trace( "%1%: %2%", r->Name(), r->StateStr() ) ;
	}
}

void State::FromRemote( const Entry& e )
{
	if ( !Update( e ) )
	{
		m_unresolved.push_back( e ) ;
	}
}

void State::ResolveEntry()
{
	while ( !m_unresolved.empty() )
	{
		if ( TryResolveEntry() == 0 )
			break ;
	}
}

std::size_t State::TryResolveEntry()
{
	assert( !m_unresolved.empty() ) ;

	std::size_t count = 0 ;
	std::vector<Entry>& en = m_unresolved ;
	
	for ( std::vector<Entry>::iterator i = en.begin() ; i != en.end() ; )
	{
		if ( Update( *i ) )
		{
			i = en.erase( i ) ;
			count++ ;
		}
		else
			++i ;
	}
	return count ;
}

bool State::Update( const Entry& e )
{
	assert( !e.ParentHref().empty() ) ;
	
	if ( Resource *res = m_res.FindByHref( e.SelfHref() ) )
	{
		m_res.Update( res, e ) ;
		return true ;
	}
	else if ( Resource *parent = m_res.FindByHref( e.ParentHref() ) )
	{
		assert( parent->IsFolder() ) ;

		// see if the entry already exist in local
		std::string name = ( e.Kind() == "folder" ? e.Title() : e.Filename() ) ;
		Resource *child = parent->FindChild( name ) ;
		if ( child != 0 )
		{
			// since we are updating the ID and Href, we need to remove it and re-add it.
			m_res.Update( child, e ) ;
		}
		
		// folder entry exist in google drive, but not local. we should create
		// the directory
		else if ( e.Kind() == "folder" || !e.Filename().empty() )
		{
			child = new Resource( e ) ;
			parent->AddChild( child ) ;
			m_res.Insert( child ) ;
			
			fs::path child_path = child->Path() ;
			if ( child->IsFolder() && !fs::is_directory( child_path ) )
			{
				Log( "creating %1% directory", child_path, log::info ) ;
				fs::create_directories( child_path ) ;
			}
		}
		else
		{
			Trace( "what here? %1%", e.Title() ) ;
		}
		
		return true ;
	}
	else
		return false ;
}

Resource* State::FindByHref( const std::string& href )
{
	return m_res.FindByHref( href ) ;
}

Resource* State::Find( const fs::path& path )
{
	return m_res.FindByPath( path ) ;
}


State::iterator State::begin()
{
	return m_res.begin() ;
}

State::iterator State::end()
{
	return m_res.end() ;
}

void State::Read( const fs::path& filename )
{
	if ( fs::exists( filename ) )
	{
		Json json = Json::ParseFile( filename.string() ) ;
		
		m_change_stamp = json["change_stamp"].Str() ;
		m_res.Read( json["rtree"] ) ;
	}
}

void State::Write( const fs::path& filename ) const
{
	Json result ;
	result.Add( "change_stamp", Json( m_change_stamp ) ) ;
	result.Add( "rtree",		m_res.Serialize() ) ;	
	
	std::ofstream fs( filename.string().c_str() ) ;
	fs << result ;
}

} // end of namespace
