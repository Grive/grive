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

#include "Entry.hh"
#include "Resource.hh"
#include "CommonUri.hh"

#include "http/Agent.hh"
#include "http/Header.hh"
#include "util/Crypt.hh"
#include "util/log/Log.hh"
#include "protocol/Json.hh"

#include <fstream>

namespace gr {

State::State( const fs::path& filename, const Json& options ) :
	m_cstamp( -1 )
{
	Read( filename ) ;
	
	// the "-f" option will make grive always thinks remote is newer
	Json force ;
	if ( options.Get("force", force) && force.Bool() )
		m_last_sync = DateTime() ;
	
	Log( "last sync time: %1%", m_last_sync, log::verbose ) ;
}

State::~State()
{
}

/// Synchronize local directory. Build up the resource tree from files and folders
/// of local directory.
void State::FromLocal( const fs::path& p )
{
	FromLocal( p, m_res.Root() ) ;
}

bool State::IsIgnore( const std::string& filename )
{
	return filename[0] == '.' ;
}

void State::FromLocal( const fs::path& p, gr::Resource* folder )
{
	assert( folder != 0 ) ;
	assert( folder->IsFolder() ) ;
	
	// sync the folder itself
	folder->FromLocal( m_last_sync ) ;

	for ( fs::directory_iterator i( p ) ; i != fs::directory_iterator() ; ++i )
	{
		std::string fname = Path2Str(i->path().filename()) ;
// Trace( "found file %1%", i->path() ) ;
	
		if ( IsIgnore(fname) )
			Log( "file %1% is ignored by grive", fname, log::verbose ) ;
		
		// check for broken symblic links
		else if ( !fs::exists( i->path() ) )
			Log( "file %1% doesn't exist (broken link?), ignored", i->path(), log::verbose ) ;
		
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
	std::string fn = e.Filename() ;				

	if ( IsIgnore( e.Name() ) )
		Log( "%1% %2% is ignored by grive", e.Kind(), e.Name(), log::verbose ) ;

	// common checkings
	else if ( e.Kind() != "folder" && (fn.empty() || e.ContentSrc().empty()) )
		Log( "%1% \"%2%\" is a google document, ignored", e.Kind(), e.Name(), log::verbose ) ;
	
	else if ( fn.find('/') != fn.npos )
		Log( "%1% \"%2%\" contains a slash in its name, ignored", e.Kind(), e.Name(), log::verbose ) ;
	
	else if ( !e.IsChange() && e.ParentHrefs().size() != 1 )
		Log( "%1% \"%2%\" has multiple parents, ignored", e.Kind(), e.Name(), log::verbose ) ;

	else if ( e.IsChange() )
		FromChange( e ) ;

	else if ( !Update( e ) )
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

void State::FromChange( const Entry& e )
{
	assert( e.IsChange() ) ;
	assert( !IsIgnore( e.Name() ) ) ;
	
	// entries in the change feed is always treated as newer in remote,
	// so we override the last sync time to 0
	if ( Resource *res = m_res.FindByHref( e.AltSelf() ) )
		m_res.Update( res, e, DateTime() ) ;
}

bool State::Update( const Entry& e )
{
	assert( !e.IsChange() ) ;
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
		std::string name = e.Name() ;
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
		
		m_cstamp = json["change_stamp"].Int() ;
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
	result.Add( "change_stamp", Json(m_cstamp) ) ;
	
	std::ofstream fs( filename.string().c_str() ) ;
	fs << result ;
}

void State::Sync( http::Agent *http, const http::Header& auth )
{
	m_res.Root()->Sync( http, auth ) ;
	m_last_sync = DateTime::Now() ;
}

long State::ChangeStamp() const
{
	return m_cstamp ;
}

void State::ChangeStamp( long cstamp )
{
	Log( "change stamp is set to %1%", cstamp, log::verbose ) ;
	m_cstamp = cstamp ;
}

} // end of namespace
