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

State::State( const fs::path& filename, const Json& options )
{
	Read( filename ) ;
	
	// the "-f" option will make grive always thinks remote is newer
	Json force ;
	if ( options.Get("force", force) && force.Bool() )
		m_last_sync = DateTime() ;
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
	folder->FromLocal( m_last_sync ) ;

	for ( fs::directory_iterator i( p ) ; i != fs::directory_iterator() ; ++i )
	{
		std::string fname = i->path().filename().string() ;
	
		if ( fname[0] == '.' )
			Log( "file %1% is ignored by grive", fname, log::verbose ) ;
		
		else
		{
			// if the Resource object of the child already exists, it should
			// have been so no need to do anything here
			Resource *c = folder->FindChild( fname ) ;
			if ( c == 0 )
			{
				c = new Resource( fname, fs::is_directory(i->path()) ? "folder" : "file" ) ;
				folder->AddChild( c ) ;
				m_res.Insert( c ) ;
			}
			
			c->FromLocal( m_last_sync ) ;			
			
			if ( fs::is_directory( i->path() ) )
				FromLocal( *i, c ) ;
		}
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
		m_res.Update( res, e, m_last_sync ) ;
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
			m_res.Update( child, e, m_last_sync ) ;
		}
		
		// folder entry exist in google drive, but not local. we should create
		// the directory
		else if ( e.Kind() == "folder" || !e.Filename().empty() )
		{
			// first create a dummy resource and update it later
			child = new Resource( name, e.Kind() ) ;
			parent->AddChild( child ) ;
			m_res.Insert( child ) ;
			
			// update the state of the resource
			m_res.Update( child, e, m_last_sync ) ;
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
	try
	{
		Json json = Json::ParseFile( filename.string() ) ;
		
		Json last_sync = json["last_sync"] ;
		m_last_sync.Assign(
			last_sync["sec"].Int(),
			last_sync["nsec"].Int() ) ;
	}
	catch ( Exception& )
	{
		m_last_sync.Assign(0) ;
	}
}

void State::Write( const fs::path& filename ) const
{
	Json last_sync ;
	last_sync.Add( "sec",	Json(m_last_sync.Sec() ) );
	last_sync.Add( "nsec",	Json(m_last_sync.NanoSec() ) );
	
	Json result ;
	result.Add( "last_sync", last_sync ) ;
	
	std::ofstream fs( filename.string().c_str() ) ;
	fs << result ;
}

void State::Sync( http::Agent *http, const http::Headers& auth )
{
	std::for_each( m_res.begin(), m_res.end(),
		boost::bind( &Resource::Sync, _1, http, auth ) ) ;
	
	m_last_sync = DateTime::Now() ;
}

} // end of namespace
