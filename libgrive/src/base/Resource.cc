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

#include "Resource.hh"
#include "ResourceTree.hh"
#include "Entry.hh"
#include "Syncer.hh"

#include "json/Val.hh"
#include "util/CArray.hh"
#include "util/Crypt.hh"
#include "util/log/Log.hh"
#include "util/OS.hh"
#include "util/File.hh"
#include "http/Error.hh"

#include <boost/exception/all.hpp>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>

#include <errno.h>

#include <cassert>

// for debugging
#include <iostream>

namespace gr {

/// default constructor creates the root folder
Resource::Resource( const fs::path& root_folder ) :
	m_name		( root_folder.string() ),
	m_kind		( "folder" ),
	m_size		( 0 ),
	m_id		( "folder:root" ),
	m_href		( "root" ),
	m_is_editable( true ),
	m_parent	( 0 ),
	m_state		( sync ),
	m_json		( NULL ),
	m_local_exists( true )
{
}

Resource::Resource( const std::string& name, const std::string& kind ) :
	m_name		( name ),
	m_kind		( kind ),
	m_size		( 0 ),
	m_is_editable( true ),
	m_parent	( 0 ),
	m_state		( unknown ),
	m_json		( NULL ),
	m_local_exists( false )
{
}

void Resource::SetState( State new_state )
{
	// only the new and delete states need to be set recursively
	assert(
		new_state == remote_new || new_state == remote_deleted ||
		new_state == local_new  || new_state == local_deleted
	) ;
	
	m_state = new_state ;
	std::for_each( m_child.begin(), m_child.end(),
		boost::bind( &Resource::SetState, _1, new_state ) ) ;
}

void Resource::FromRemoteFolder( const Entry& remote )
{
	fs::path path = Path() ;
	
	if ( !remote.IsEditable() )
		Log( "folder %1% is read-only", path, log::verbose ) ;
	
	// already sync
	if ( m_local_exists && m_kind == "folder" )
	{
		Log( "folder %1% is in sync", path, log::verbose ) ;
		m_state = sync ;
	}
	else if ( m_local_exists && m_kind == "file" )
	{
		// TODO: handle type change
		Log( "%1% changed from folder to file", path, log::verbose ) ;
		m_state = sync ;
	}
	else if ( m_local_exists && m_kind == "bad" )
	{
		Log( "%1% inaccessible", path, log::verbose ) ;
		m_state = sync ;
	}
	else if ( remote.MTime().Sec() > m_mtime.Sec() ) // FIXME only seconds are stored in local index
	{
		// remote folder created after last sync, so remote is newer
		Log( "folder %1% is created in remote", path, log::verbose ) ;
		SetState( remote_new ) ;
	}
	else
	{
		Log( "folder %1% is deleted in local", path, log::verbose ) ;
		SetState( local_deleted ) ;
	}
}

/// Update the state according to information (i.e. Entry) from remote. This function
/// compares the modification time and checksum of both copies and determine which
/// one is newer.
void Resource::FromRemote( const Entry& remote )
{
	// sync folder
	if ( remote.IsDir() && IsFolder() )
		FromRemoteFolder( remote ) ;
	else
		FromRemoteFile( remote ) ;
	
	AssignIDs( remote ) ;
	
	assert( m_state != unknown ) ;
	
	if ( m_state == remote_new || m_state == remote_changed )
		m_md5 = remote.MD5() ;
	
	m_mtime = remote.MTime() ;
}

void Resource::AssignIDs( const Entry& remote )
{
	// the IDs from change feed entries are different
	if ( !remote.IsChange() )
	{
		m_id		= remote.ResourceID() ;
		m_href		= remote.SelfHref() ;
		m_content	= remote.ContentSrc() ;
		m_is_editable = remote.IsEditable() ;
		m_etag		= remote.ETag() ;
		m_md5		= remote.MD5() ;
	}
}

void Resource::FromRemoteFile( const Entry& remote )
{
	assert( m_parent != 0 ) ;
	
	fs::path path = Path() ;

	// recursively create/delete folder
	if ( m_parent->m_state == remote_new || m_parent->m_state == remote_deleted ||
		 m_parent->m_state == local_new  || m_parent->m_state == local_deleted )
	{
		Log( "file %1% parent %2% recursively in %3% (%4%)", path,
			( m_parent->m_state == remote_new || m_parent->m_state == local_new )      ? "created" : "deleted",
			( m_parent->m_state == remote_new || m_parent->m_state == remote_deleted ) ? "remote"  : "local",
			m_parent->m_state, log::verbose ) ;
		
		m_state = m_parent->m_state ;
	}

	else if ( m_kind == "bad" )
	{
		m_state = sync;
	}

	// local not exists
	else if ( !m_local_exists )
	{
		Trace( "file %1% change stamp = %2%", Path(), remote.ChangeStamp() ) ;
		
		if ( remote.MTime().Sec() > m_mtime.Sec() || remote.MD5() != m_md5 || remote.ChangeStamp() > 0 )
		{
			Log( "file %1% is created in remote (change %2%)", path,
				remote.ChangeStamp(), log::verbose ) ;
			m_size = remote.Size();
			m_state = remote_new ;
		}
		else
		{
			Log( "file %1% is deleted in local", path, log::verbose ) ;
			m_state = local_deleted ;
		}
	}

	// remote checksum unknown, assume the file is not changed in remote
	else if ( remote.MD5().empty() )
	{
		Log( "file %1% has unknown checksum in remote. assumed in sync",
			Path(), log::verbose ) ;
		m_state = sync ;
	}

	// use mtime to check which one is more recent
	else if ( remote.Size() != m_size || remote.MD5() != GetMD5() )
	{
		assert( m_state != unknown ) ;

		// if remote is modified
		if ( remote.MTime().Sec() > m_mtime.Sec() )
		{
			Log( "file %1% is changed in remote", path, log::verbose ) ;
			m_size = remote.Size();
			m_state = remote_changed ;
		}
		
		// remote also has the file, so it's not new in local
		else if ( m_state == local_new || m_state == remote_deleted )
		{
			Log( "file %1% is changed in local", path, log::verbose ) ;
			m_state = local_changed ;
		}
		else
			Trace( "file %1% state is %2%", m_name, m_state ) ;
	}

	// if checksum is equal, no need to compare the mtime
	else
	{
		Log( "file %1% is already in sync", Path(), log::verbose ) ;
		m_state = sync ;
	}
}

void Resource::FromDeleted( Val& state )
{
	assert( !m_json );
	m_json = &state;
	if ( state.Has( "ctime" ) )
		m_ctime.Assign( state["ctime"].U64(), 0 );
	if ( state.Has( "md5" ) )
		m_md5 = state["md5"];
	if ( state.Has( "srv_time" ) )
		m_mtime.Assign( state[ "srv_time" ].U64(), 0 ) ;
	if ( state.Has( "size" ) )
		m_size = state[ "size" ].U64();
	m_state = both_deleted;
}

/// Update the resource with the attributes of local file or directory. This
/// function will propulate the fields in m_entry.
void Resource::FromLocal( Val& state )
{
	assert( !m_json );
	m_json = &state;

	// root folder is always in sync
	if ( !IsRoot() )
	{
		fs::path path = Path() ;
		FileType ft ;
		try
		{
			os::Stat( path, &m_ctime, (off64_t*)&m_size, &ft ) ;
		}
		catch ( os::Error &e )
		{
			// invalid symlink, unreadable file or something else
			int const* eno = boost::get_error_info< boost::errinfo_errno >(e);
			Log( "Error accessing %1%: %2%; skipping file", path.string(), strerror( *eno ), log::warning );
			m_state = sync;
			m_kind = "bad";
			return;
		}
		if ( ft == FT_UNKNOWN )
		{
			// Skip sockets/FIFOs/etc
			Log( "File %1% is not a regular file or directory; skipping file", path.string(), log::warning );
			m_state = sync;
			m_kind = "bad";
			return;
		}

		m_name = path.filename().string() ;
		m_kind = ft == FT_DIR ? "folder" : "file";
		m_local_exists = true;

		bool is_changed;
		if ( state.Has( "ctime" ) && (u64_t) m_ctime.Sec() <= state["ctime"].U64() &&
			( ft == FT_DIR || state.Has( "md5" ) ) )
		{
			if ( ft != FT_DIR )
				m_md5 = state["md5"];
			is_changed = false;
		}
		else
		{
			if ( ft != FT_DIR )
			{
				// File is changed locally. TODO: Detect conflicts
				is_changed = ( state.Has( "size" ) && m_size != state["size"].U64() ) ||
					!state.Has( "md5" ) || GetMD5() != state["md5"].Str();
			}
			else
				is_changed = true;
		}
		if ( state.Has( "srv_time" ) )
			m_mtime.Assign( state[ "srv_time" ].U64(), 0 ) ;

		// Upload file if it is changed and remove if not.
		// State will be updated to sync/remote_changed in FromRemote()
		m_state = is_changed ? local_new : remote_deleted;
		if ( m_state == local_new )
		{
			// local_new means this file is changed in local.
			// this means we can't delete any of its parents.
			// make sure their state is also set to local_new.
			Resource *p = m_parent;
			while ( p && p->m_state == remote_deleted )
			{
				p->m_state = local_new;
				p = p->m_parent;
			}
		}
	}
	
	assert( m_state != unknown ) ;
}

std::string Resource::SelfHref() const
{
	return m_href ;
}

std::string Resource::ContentSrc() const
{
	return m_content ;
}

std::string Resource::ETag() const
{
	return m_etag ;
}

std::string Resource::Name() const
{
	return m_name ;
}

std::string Resource::Kind() const
{
	return m_kind ;
}

DateTime Resource::ServerTime() const
{
	return m_mtime ;
}

std::string Resource::ResourceID() const
{
	return m_id ;
}

Resource::State Resource::GetState() const
{
	return m_state ;
}

const Resource* Resource::Parent() const
{
	assert( m_parent == 0 || m_parent->IsFolder() ) ;
	return m_parent ;
}

Resource* Resource::Parent()
{
	assert( m_parent == 0 || m_parent->IsFolder() ) ;
	return m_parent ;
}

void Resource::AddChild( Resource *child )
{
	assert( child != 0 ) ;
	assert( child->m_parent == 0 || child->m_parent == this ) ;
	assert( child != this ) ;

	child->m_parent = this ;
	m_child.push_back( child ) ;
}

bool Resource::IsFolder() const
{
	return m_kind == "folder" ;
}

bool Resource::IsEditable() const
{
	return m_is_editable ;
}

fs::path Resource::Path() const
{
	assert( m_parent != this ) ;
	assert( m_parent == 0 || m_parent->IsFolder() ) ;

	return m_parent != 0 ? (m_parent->Path() / m_name) : m_name ;
}

// Path relative to the root directory
fs::path Resource::RelPath() const
{
	assert( m_parent != this ) ;
	assert( m_parent == 0 || m_parent->IsFolder() ) ;

	return m_parent != 0 && !m_parent->IsRoot() ? (m_parent->RelPath() / m_name) : m_name ;
}

bool Resource::IsInRootTree() const
{
	assert( m_parent == 0 || m_parent->IsFolder() ) ;
	return m_parent == 0 ? IsRoot() : m_parent->IsInRootTree() ;
}

Resource* Resource::FindChild( const std::string& name )
{
	for ( std::vector<Resource*>::iterator i = m_child.begin() ; i != m_child.end() ; ++i )
	{
		assert( (*i)->m_parent == this ) ;
		if ( (*i)->m_name == name )
			return *i ;
	}
	return 0 ;
}

// try to change the state to "sync"
void Resource::Sync( Syncer *syncer, ResourceTree *res_tree, const Val& options )
{
	assert( m_state != unknown ) ;
	assert( !IsRoot() || m_state == sync ) ;	// root folder is already synced
	
	try
	{
		SyncSelf( syncer, res_tree, options ) ;
	}
	catch ( File::Error &e )
	{
		int *en = boost::get_error_info< boost::errinfo_errno > ( e ) ;
		Log( "Error syncing %1%: %2%", Path(), en ? strerror( *en ) : "", log::error );
		return;
	}
	catch ( boost::filesystem::filesystem_error &e )
	{
		Log( "Error syncing %1%: %2%", Path(), e.what(), log::error );
		return;
	}
	catch ( http::Error &e )
	{
		int *curlcode = boost::get_error_info< http::CurlCode > ( e ) ;
		int *httpcode = boost::get_error_info< http::HttpResponseCode > ( e ) ;
		std::string msg;
		if ( curlcode )
			msg = *( boost::get_error_info< http::CurlErrMsg > ( e ) );
		else if ( httpcode )
			msg = "HTTP " + boost::to_string( *httpcode );
		else
			msg = e.what();
		Log( "Error syncing %1%: %2%", Path(), msg, log::error );
		std::string *url = boost::get_error_info< http::Url > ( e );
		std::string *resp_hdr = boost::get_error_info< http::HttpResponseHeaders > ( e );
		std::string *resp_txt = boost::get_error_info< http::HttpResponseText > ( e );
		http::Header *req_hdr = boost::get_error_info< http::HttpRequestHeaders > ( e );
		if ( url )
			Log( "Request URL: %1%", *url, log::verbose );
		if ( req_hdr )
			Log( "Request headers: %1%", req_hdr->Str(), log::verbose );
		if ( resp_hdr )
			Log( "Response headers: %1%", *resp_hdr, log::verbose );
		if ( resp_txt )
			Log( "Response text: %1%", *resp_txt, log::verbose );
		return;
	}
	
	// if myself is deleted, no need to do the childrens
	if ( m_state != local_deleted && m_state != remote_deleted )
	{
		std::for_each( m_child.begin(), m_child.end(),
			boost::bind( &Resource::Sync, _1, syncer, res_tree, options ) ) ;
	}
}

bool Resource::CheckRename( Syncer* syncer, ResourceTree *res_tree )
{
	if ( !IsFolder() && ( m_state == local_new || m_state == remote_new ) )
	{
		bool is_local = m_state == local_new;
		State other = is_local ? local_deleted : remote_deleted;
		if ( is_local )
		{
			// First check size index for locally added files
			details::SizeRange moved = res_tree->FindBySize( m_size );
			bool found = false;
			for ( details::SizeMap::iterator i = moved.first ; i != moved.second; i++ )
			{
				Resource *m = *i;
				if ( m->m_state == other )
				{
					found = true;
					break;
				}
			}
			if ( !found )
			{
				// Don't check md5 sums if there are no deleted files with same size
				return false;
			}
		}
		details::MD5Range moved = res_tree->FindByMD5( GetMD5() );
		for ( details::MD5Map::iterator i = moved.first ; i != moved.second; i++ )
		{
			Resource *m = *i;
			if ( m->m_state == other )
			{
				Resource* from = m_state == local_new || m_state == remote_new ? m : this;
				Resource* to = m_state == local_new || m_state == remote_new ? this : m;
				Log( "sync %1% moved to %2%. moving %3%", from->Path(), to->Path(),
					is_local ? "remote" : "local", log::info );
				if ( syncer )
				{
					if ( is_local )
					{
						syncer->Move( from, to->Parent(), to->Name() );
						to->SetIndex( false );
					}
					else
					{
						fs::rename( from->Path(), to->Path() );
						to->SetIndex( true );
					}
					to->m_mtime = from->m_mtime;
					to->m_json->Set( "srv_time", Val( from->m_mtime.Sec() ) );
					from->DeleteIndex();
				}
				from->m_state = both_deleted;
				to->m_state = sync;
				return true;
			}
		}
	}
	return false;
}

void Resource::SyncSelf( Syncer* syncer, ResourceTree *res_tree, const Val& options )
{
	assert( !IsRoot() || m_state == sync ) ;	// root is always sync
	assert( IsRoot() || !syncer || m_parent->IsFolder() ) ;
	assert( IsRoot() || m_parent->m_state != remote_deleted ) ;
	assert( IsRoot() || m_parent->m_state != local_deleted ) ;

	const fs::path path = Path() ;

	// Detect renames
	if ( CheckRename( syncer, res_tree ) )
		return;

	switch ( m_state )
	{
	case local_new :
		Log( "sync %1% doesn't exist in server, uploading", path, log::info ) ;
		
		if ( syncer && syncer->Create( this ) )
		{
			m_state = sync ;
			SetIndex( false );
		}
		break ;
	
	case local_deleted :
		Log( "sync %1% deleted in local. deleting remote", path, log::info ) ;
		if ( syncer && !options["no-delete-remote"].Bool() )
		{
			syncer->DeleteRemote( this ) ;
			DeleteIndex() ;
		}
		break ;
	
	case local_changed :
		Log( "sync %1% changed in local. uploading", path, log::info ) ;
		if ( syncer && syncer->EditContent( this, options["new-rev"].Bool() ) )
		{
			m_state = sync ;
			SetIndex( false );
		}
		break ;
	
	case remote_new :
		if ( options["no-remote-new"].Bool() )
			Log( "sync %1% created in remote. skipping", path, log::info ) ;
		else
		{
			Log( "sync %1% created in remote. creating local", path, log::info ) ;
			if ( syncer )
			{
				if ( IsFolder() )
					fs::create_directories( path ) ;
				else
					syncer->Download( this, path ) ;
				SetIndex( true ) ;
				m_state = sync ;
			}
		}
		break ;
	
	case remote_changed :
		assert( !IsFolder() ) ;
		if ( options["upload-only"].Bool() )
			Log( "sync %1% changed in remote. skipping", path, log::info ) ;
		else
		{
			Log( "sync %1% changed in remote. downloading", path, log::info ) ;
			if ( syncer )
			{
				syncer->Download( this, path ) ;
				SetIndex( true ) ;
				m_state = sync ;
			}
		}
		break ;
	
	case remote_deleted :
		Log( "sync %1% deleted in remote. deleting local", path, log::info ) ;
		if ( syncer )
		{
			DeleteLocal() ;
			DeleteIndex() ;
		}
		break ;
	
	case both_deleted :
		if ( syncer )
			DeleteIndex() ;
		break ;
	
	case sync :
		Log( "sync %1% already in sync", path, log::verbose ) ;
		if ( !IsRoot() )
			SetIndex( false ) ;
		break ;
	
	// shouldn't go here
	case unknown :
	default :
		assert( false ) ;
		break ;
	}
	
	if ( syncer && m_json )
	{
		// Update server time of this file
		m_json->Set( "srv_time", Val( m_mtime.Sec() ) );
	}
}

void Resource::SetServerTime( const DateTime& time )
{
	m_mtime = time ;
}

/// this function doesn't really remove the local file. it renames it.
void Resource::DeleteLocal()
{
	static const boost::format trash_file( "%1%-%2%" ) ;

	assert( m_parent != NULL ) ;
	Resource* p = m_parent;
	fs::path destdir;
	while ( !p->IsRoot() )
	{
		destdir = p->Name() / destdir;
		p = p->Parent();
	}
	destdir = p->Path() / ".trash" / destdir;

	fs::path dest = destdir / Name();
	std::size_t idx = 1 ;
	while ( fs::exists( dest ) && idx != 0 )
		dest = destdir / (boost::format(trash_file) % Name() % idx++).str() ;

	// wrap around! just remove the file
	if ( idx == 0 )
		fs::remove_all( Path() ) ;
	else
	{
		fs::create_directories( dest.parent_path() ) ;
		fs::rename( Path(), dest ) ;
	}
}

void Resource::DeleteIndex()
{
	(*m_parent->m_json)["tree"].Del( Name() );
	m_json = NULL;
}

void Resource::SetIndex( bool re_stat )
{
	assert( m_parent && m_parent->m_json != NULL );
	if ( !m_json )
		m_json = &((*m_parent->m_json)["tree"]).Item( Name() );
	FileType ft;
	if ( re_stat )
		os::Stat( Path(), &m_ctime, NULL, &ft );
	else
		ft = IsFolder() ? FT_DIR : FT_FILE;
	m_json->Set( "ctime", Val( m_ctime.Sec() ) );
	if ( ft != FT_DIR )
	{
		m_json->Set( "md5", Val( m_md5 ) );
		m_json->Set( "size", Val( m_size ) );
		m_json->Del( "tree" );
	}
	else
	{
		// add tree item if it does not exist
		m_json->Item( "tree" );
		m_json->Del( "md5" );
		m_json->Del( "size" );
	}
}

Resource::iterator Resource::begin() const
{
	return m_child.begin() ;
}

Resource::iterator Resource::end() const
{
	return m_child.end() ;
}

std::size_t Resource::size() const
{
	return m_child.size() ;
}

std::ostream& operator<<( std::ostream& os, Resource::State s )
{
	static const char *state[] =
	{
		"sync",	"local_new", "local_changed", "local_deleted", "remote_new",
		"remote_changed", "remote_deleted", "both_deleted"
	} ;
	assert( s >= 0 && s < Count(state) ) ;
	return os << state[s] ;
}

std::string Resource::StateStr() const
{
	std::ostringstream ss ;
	ss << m_state ;
	return ss.str() ;
}

u64_t Resource::Size() const
{
	return m_size ;
}

std::string Resource::MD5() const
{
	return m_md5 ;
}

std::string Resource::GetMD5()
{
	if ( m_md5.empty() && !IsFolder() && m_local_exists )
	{
		// MD5 checksum is calculated lazily and only when really needed:
		// 1) when a local rename is supposed (when there are a new file and a deleted file of the same size)
		// 2) when local ctime is changed, but file size isn't
		m_md5 = crypt::MD5::Get( Path() );
	}
	return m_md5 ;
}

bool Resource::IsRoot() const
{
	// Root entry does not show up in file feeds, so we check for empty parent (and self-href)
	return !m_parent ;
}

bool Resource::HasID() const
{
	return !m_href.empty() && !m_id.empty() ;
}

} // end of namespace
