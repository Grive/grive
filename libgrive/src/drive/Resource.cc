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
#include "CommonUri.hh"
#include "Entry.hh"

#include "http/Agent.hh"
#include "http/Download.hh"
#include "http/Header.hh"
// #include "http/ResponseLog.hh"
#include "http/StringResponse.hh"
#include "http/XmlResponse.hh"
#include "protocol/Json.hh"
#include "util/CArray.hh"
#include "util/Crypt.hh"
#include "util/log/Log.hh"
#include "util/OS.hh"
#include "util/StdioFile.hh"
#include "xml/Node.hh"
#include "xml/NodeSet.hh"
#include "xml/String.hh"

#include <boost/bind.hpp>
#include <boost/exception/all.hpp>

#include <cassert>

// for debugging
#include <iostream>

namespace gr {

// hard coded XML file
const std::string xml_meta =
	"<?xml version='1.0' encoding='UTF-8'?>\n"
	"<entry xmlns=\"http://www.w3.org/2005/Atom\" xmlns:docs=\"http://schemas.google.com/docs/2007\">"
		"<category scheme=\"http://schemas.google.com/g/2005#kind\" "
		"term=\"http://schemas.google.com/docs/2007#%1%\"/>"
		"<title>%2%</title>"
	"</entry>" ;


/// default constructor creates the root folder
Resource::Resource() :
	m_name		( "." ),
	m_kind		( "folder" ),
	m_id		( "folder:root" ),
	m_href		( root_href ),
	m_create	( root_create ),
	m_parent	( 0 ),
	m_state		( sync )
{
}

Resource::Resource( const std::string& name, const std::string& kind ) :
	m_name		( name ),
	m_kind		( kind ),
	m_parent	( 0 ),
	m_state		( unknown )
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

void Resource::FromRemoteFolder( const Entry& remote, const DateTime& last_sync )
{
	fs::path path = Path() ;
	
	if ( remote.CreateLink().empty() )
		Log( "folder %1% is read-only", path, log::verbose ) ;
		
	// already sync
	if ( fs::is_directory( path ) )
	{
		Log( "folder %1% is in sync", path, log::verbose ) ;
		m_state = sync ;
	}
	
	// remote file created after last sync, so remote is newer
	else if ( remote.MTime() > last_sync )
	{
		if ( fs::exists( path ) )
		{
			// TODO: handle type change
			Log( "%1% changed from folder to file", path, log::verbose ) ;
			m_state = sync ;
		}
		else
		{
			// make all children as remote_new, if any
			Log( "folder %1% is created in remote", path, log::verbose ) ;
			SetState( remote_new ) ;
		}
	}
	else
	{
		if ( fs::exists( path ) )
		{
			// TODO: handle type chage
			Log( "%1% changed from file to folder", path, log::verbose ) ;
			m_state = sync ;
		}
		else
		{
			Log( "folder %1% is deleted in local", path, log::verbose ) ;
			SetState( local_deleted ) ;
		}
	}
}

/// Update the state according to information (i.e. Entry) from remote. This function
/// compares the modification time and checksum of both copies and determine which
/// one is newer.
void Resource::FromRemote( const Entry& remote, const DateTime& last_sync )
{
	// sync folder
	if ( remote.Kind() == "folder" && IsFolder() )
		FromRemoteFolder( remote, last_sync ) ;
	else
		FromRemoteFile( remote, last_sync ) ;
	
	AssignIDs( remote ) ;
	
	assert( m_state != unknown ) ;
	
	if ( m_state == remote_new || m_state == remote_changed )
	{
		m_md5	= remote.MD5() ;
		m_mtime	= remote.MTime() ;
	}
}

void Resource::AssignIDs( const Entry& remote )
{
	// the IDs from change feed entries are different
	if ( !remote.IsChange() )
	{
		m_id		= remote.ResourceID() ;
		m_href		= remote.SelfHref() ;
		m_edit		= remote.EditLink() ;
		m_create	= remote.CreateLink() ;
		m_content	= remote.ContentSrc() ;
		m_etag		= remote.ETag() ;
	}
}

void Resource::FromRemoteFile( const Entry& remote, const DateTime& last_sync )
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

	// local not exists
	else if ( !fs::exists( path ) )
	{
		Trace( "file %1% change stamp = %2%", Path(), remote.ChangeStamp() ) ;
		
		if ( remote.MTime() > last_sync || remote.ChangeStamp() > 0 )
		{
			Log( "file %1% is created in remote (change %2%)", path,
				remote.ChangeStamp(), log::verbose ) ;
			
			m_state = remote_new ;
		}
		else
		{
			Log( "file %1% is deleted in local", path, log::verbose ) ;
			m_state = local_deleted ;
		}
	}
	
	// if checksum is equal, no need to compare the mtime
	else if ( remote.MD5() == m_md5 )
	{
		Log( "file %1% is already in sync", Path(), log::verbose ) ;
		m_state = sync ;
	}

	// use mtime to check which one is more recent
	else
	{
		assert( m_state != unknown ) ;

		// if remote is modified
		if ( remote.MTime() > m_mtime )
		{
			Log( "file %1% is changed in remote", path, log::verbose ) ;
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
}

/// Update the resource with the attributes of local file or directory. This
/// function will propulate the fields in m_entry.
void Resource::FromLocal( const DateTime& last_sync )
{
	fs::path path = Path() ;
	assert( fs::exists( path ) ) ;

	// root folder is always in sync
	if ( !IsRoot() )
	{
		m_mtime = os::FileCTime( path ) ;

		// follow parent recursively
		if ( m_parent->m_state == local_new || m_parent->m_state == local_deleted )
			m_state = local_new ;
		
		// if the file is not created after last sync, assume file is
		// remote_deleted first, it will be updated to sync/remote_changed
		// in FromRemote()
		else
			m_state = ( m_mtime > last_sync ? local_new : remote_deleted ) ;
		
		m_name		= Path2Str( path.filename() ) ;
		m_kind		= fs::is_directory(path) ? "folder"	: "file" ;
		m_md5		= fs::is_directory(path) ? ""		: crypt::MD5::Get( path ) ;
	}
	
	assert( m_state != unknown ) ;
}

std::string Resource::SelfHref() const
{
	return m_href ;
}

std::string Resource::Name() const
{
	return m_name ;
}

std::string Resource::ResourceID() const
{
	return m_id ;
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

void Resource::Swap( Resource& coll )
{
	m_name.swap( coll.m_name ) ;
	m_kind.swap( coll.m_kind ) ;
	m_md5.swap( coll.m_md5 ) ;
	m_etag.swap( coll.m_etag ) ;
	m_id.swap( coll.m_id ) ;

	m_href.swap( coll.m_href ) ;
	m_content.swap( coll.m_content ) ;	
	m_edit.swap( coll.m_edit ) ;
	m_create.swap( coll.m_create ) ;
	
	m_mtime.Swap( coll.m_mtime ) ;
	
	std::swap( m_parent, coll.m_parent ) ;
	m_child.swap( coll.m_child ) ;
	std::swap( m_state, coll.m_state ) ;
}

bool Resource::IsFolder() const
{
	return m_kind == "folder" ;
}

fs::path Resource::Path() const
{
	assert( m_parent != this ) ;
	assert( m_parent == 0 || m_parent->IsFolder() ) ;

	return m_parent != 0 ? (m_parent->Path() / m_name) : m_name ;
}

bool Resource::IsInRootTree() const
{
	assert( m_parent == 0 || m_parent->IsFolder() ) ;
	return m_parent == 0 ? (SelfHref() == root_href) : m_parent->IsInRootTree() ;
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
void Resource::Sync( http::Agent *http, const http::Header& auth )
{
	assert( m_state != unknown ) ;
	assert( !IsRoot() || m_state == sync ) ;	// root folder is already synced
	
	SyncSelf( http, auth ) ;
	
	// if myself is deleted, no need to do the childrens
	if ( m_state != local_deleted && m_state != remote_deleted )
		std::for_each( m_child.begin(), m_child.end(),
			boost::bind( &Resource::Sync, _1, http, auth ) ) ;
}

void Resource::SyncSelf( http::Agent* http, const http::Header& auth )
{
	assert( !IsRoot() || m_state == sync ) ;	// root is always sync
	assert( IsRoot() || http == 0 || fs::is_directory( m_parent->Path() ) ) ;
	assert( IsRoot() || m_parent->m_state != remote_deleted ) ;
	assert( IsRoot() || m_parent->m_state != local_deleted ) ;

	const fs::path path = Path() ;

	switch ( m_state )
	{
	case local_new :
		Log( "sync %1% doesn't exist in server, uploading", path, log::info ) ;
		
		if ( http != 0 && Create( http, auth ) )
			m_state = sync ;
		break ;
	
	case local_deleted :
		Log( "sync %1% deleted in local. deleting remote", path, log::info ) ;
		if ( http != 0 )
			DeleteRemote( http, auth ) ;
		break ;
	
	case local_changed :
		Log( "sync %1% changed in local. uploading", path, log::info ) ;
		if ( http != 0 && EditContent( http, auth ) )
			m_state = sync ;
		break ;
	
	case remote_new :
		Log( "sync %1% created in remote. creating local", path, log::info ) ;
		if ( http != 0 )
		{
			if ( IsFolder() )
				fs::create_directories( path ) ;
			else
				Download( http, path, auth ) ;
			
			m_state = sync ;
		}
		break ;
	
	case remote_changed :
		assert( !IsFolder() ) ;
		Log( "sync %1% changed in remote. downloading", path, log::info ) ;
		if ( http != 0 )
		{
			Download( http, path, auth ) ;
			m_state = sync ;
		}
		break ;
	
	case remote_deleted :
		Log( "sync %1% deleted in remote. deleting local", path, log::info ) ;
		if ( http != 0 )
			DeleteLocal() ;
		break ;
	
	case sync :
		Log( "sync %1% already in sync", path, log::verbose ) ;
		break ;

	// shouldn't go here
	case unknown :
		assert( false ) ;
		break ;
		
	default :
		break ;
	}
}

/// this function doesn't really remove the local file. it renames it.
void Resource::DeleteLocal()
{
	static const boost::format trash_file( "%1%-%2%" ) ;

	assert( m_parent != 0 ) ;
	fs::path parent = m_parent->Path() ;
	fs::path dest	= ".trash" / parent / Name() ;
	
	std::size_t idx = 1 ;
	while ( fs::exists( dest ) && idx != 0 )
		dest = ".trash" / parent / (boost::format(trash_file) % Name() % idx++).str() ;
	
	// wrap around! just remove the file
	if ( idx == 0 )
		fs::remove_all( Path() ) ;
	else
	{
		fs::create_directories( dest.parent_path() ) ;
		fs::rename( Path(), dest ) ;
	}
}

void Resource::DeleteRemote( http::Agent *http, const http::Header& auth )
{
	assert( http != 0 ) ;
	http::StringResponse str ;
	
	try
	{
		http::Header hdr( auth ) ;
		hdr.Add( "If-Match: " + m_etag ) ;
		
		// doesn't know why, but an update before deleting seems to work always
		http::XmlResponse xml ;
		http->Get( m_href, &xml, hdr ) ;
		AssignIDs( Entry( xml.Response() ) ) ;
	
		http->Custom( "DELETE", m_href, &str, hdr ) ;
	}
	catch ( Exception& e )
	{
		// don't rethrow here. there are some cases that I don't know why
		// the delete will fail.
		Trace( "Exception %1% %2%",
			boost::diagnostic_information(e),
			str.Response() ) ;
	}
}


void Resource::Download( http::Agent* http, const fs::path& file, const http::Header& auth ) const
{
	assert( http != 0 ) ;
	
	http::Download dl( file.string(), http::Download::NoChecksum() ) ;
	long r = http->Get( m_content, &dl, auth ) ;
	if ( r <= 400 )
	{
		if ( m_mtime != DateTime() )
			os::SetFileTime( file, m_mtime ) ;
		else
			Log( "encountered zero date time after downloading %1%", file, log::warning ) ;
	}
}

bool Resource::EditContent( http::Agent* http, const http::Header& auth )
{
	assert( http != 0 ) ;
	assert( m_parent != 0 ) ;
	assert( m_parent->m_state == sync ) ;

	// upload link missing means that file is read only
	if ( m_edit.empty() )
	{
		Log( "Cannot upload %1%: file read-only. %2%", m_name, m_state, log::warning ) ;
		return false ;
	}
	
	return Upload( http, m_edit, auth, false ) ;
}

bool Resource::Create( http::Agent* http, const http::Header& auth )
{
	assert( http != 0 ) ;
	assert( m_parent != 0 ) ;
	assert( m_parent->IsFolder() ) ;
	assert( m_parent->m_state == sync ) ;
	
	if ( IsFolder() )
	{
		std::string uri = feed_base ;
		if ( !m_parent->IsRoot() )
			uri += ("/" + http->Escape(m_parent->m_id) + "/contents") ;
		
		std::string meta = (boost::format( xml_meta )
			% "folder"
			% xml::Escape(m_name)
		).str() ;

		http::Header hdr( auth ) ;
		hdr.Add( "Content-Type: application/atom+xml" ) ;
		
		http::XmlResponse xml ;
// 		http::ResponseLog log( "create", ".xml", &xml ) ;
		http->Post( uri, meta, &xml, hdr ) ;
		AssignIDs( Entry( xml.Response() ) ) ;

		return true ;
	}
	else if ( !m_parent->m_create.empty() )
	{
		return Upload( http, m_parent->m_create + "?convert=false", auth, true ) ;
	}
	else
	{
		Log( "parent of %1% does not exist: cannot upload", Name(), log::warning ) ;
		return false ;
	}
}

bool Resource::Upload( http::Agent* http, const std::string& link, const http::Header& auth, bool post )
{
	assert( http != 0 ) ;
	
	StdioFile file( Path() ) ;
	
	// TODO: upload in chunks
	std::string data ;
	char buf[4096] ;
	std::size_t count = 0 ;
	while ( (count = file.Read( buf, sizeof(buf) )) > 0 )
		data.append( buf, count ) ;

	std::ostringstream xcontent_len ;
	xcontent_len << "X-Upload-Content-Length: " << data.size() ;
	
	http::Header hdr( auth ) ;
	hdr.Add( "Content-Type: application/atom+xml" ) ;
	hdr.Add( "X-Upload-Content-Type: application/octet-stream" ) ;
	hdr.Add( xcontent_len.str() ) ;
  	hdr.Add( "If-Match: " + m_etag ) ;
	hdr.Add( "Expect:" ) ;
	
	std::string meta = (boost::format( xml_meta )
		% m_kind
		% xml::Escape(m_name)
	).str() ;
	
	http::StringResponse str ;
	if ( post )
		http->Post( link, meta, &str, hdr ) ;
	else
		http->Put( link, meta, &str, hdr ) ;
	
	http::Header uphdr ;
	uphdr.Add( "Expect:" ) ;
	uphdr.Add( "Accept:" ) ;

	// the content upload URL is in the "Location" HTTP header
	std::string uplink = http->RedirLocation() ;
	http::XmlResponse xml ;
	
	http->Put( uplink, data, &xml, uphdr ) ;
	AssignIDs( Entry( xml.Response() ) ) ;
	
	return true ;
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
		"remote_changed", "remote_deleted"
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

std::string Resource::MD5() const
{
	return m_md5 ;
}

bool Resource::IsRoot() const
{
	return m_parent == 0 ;
}

bool Resource::HasID() const
{
	return !m_href.empty() && !m_id.empty() ;
}

} // end of namespace

namespace std
{
	void swap( gr::Resource& c1, gr::Resource& c2 )
	{
		c1.Swap( c2 ) ;
	}
}
