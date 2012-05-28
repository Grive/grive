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

#include "http/Download.hh"
#include "http/ResponseLog.hh"
#include "http/StringResponse.hh"
#include "http/XmlResponse.hh"
#include "protocol/Json.hh"
#include "util/CArray.hh"
#include "util/Crypt.hh"
#include "util/Log.hh"
#include "util/OS.hh"
#include "util/StdioFile.hh"
#include "xml/Node.hh"
#include "xml/NodeSet.hh"

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
	m_parent( 0 ),
	m_state	( sync )
{
}

/// Construct from previously serialized JSON object. The state of the
/// resource is treated as local_deleted by default. It is because the
/// state will be updated by scanning the local directory. If the state
/// is not updated during scanning, that means the resource is deleted.
Resource::Resource( const Json& json, Resource *parent ) :
	m_entry	(
		json["name"].Str(),
		json["id"].Str(),
		json["href"].Str(),
		json["md5"].Str(),
		json["kind"].Str(),
		DateTime( json["mtime"]["sec"].Int(), json["mtime"]["nsec"].Int() ),
		parent != 0 ? parent->SelfHref() : "" ),
	m_parent( parent ),
	m_state( local_deleted )
{
	// if the file exists in local directory, FromLocal() will mark the
	// state as local_changed
	FromLocal() ;
}

Resource::Resource( const xml::Node& entry ) :
	m_entry	( entry ),
	m_parent( 0 ),
	m_state	( remote_new )
{
}

Resource::Resource( const Entry& entry, Resource *parent ) :
	m_entry	( entry ),
	m_parent( parent ),
	m_state	( remote_new )
{
}

Resource::Resource( const fs::path& path ) :
	m_entry	( path ),
	m_parent( 0 ),
	m_state	( local_new )
{
}

/// Update the state according to information (i.e. Entry) from remote. This function
/// compares the modification time and checksum of both copies and determine which
/// one is newer.
void Resource::FromRemote( const Entry& remote )
{
	// sync folder is easy
	if ( remote.Kind() == "folder" && IsFolder() )
	{
		Log( "folder %1% is in sync", Name(), log::verbose ) ;
		m_state = sync ;
	}
	
	// if checksum is equal, no need to compare the mtime
	else if ( remote.MD5() == m_entry.MD5() )
	{
		Log( "MD5 matches: %1% is already in sync", Name(), log::verbose ) ;
		m_state = sync ;
	}
	
	// use mtime to check which one is more recent
	else
	{
		assert( m_state == local_new || m_state == local_changed || m_state == local_deleted ) ;
				
		m_state = ( remote.MTime() > m_entry.MTime() ? remote_changed : m_state ) ;
		m_state = ( m_state == local_new ? local_changed : m_state ) ;
		
		Log( "%1% state is %2%", Name(), m_state, log::verbose ) ;
	}
	
	m_entry.AssignID( remote ) ;
}

void Resource::FromLocal()
{
	fs::path path = Path() ;

	// root folder is always rsync
	if ( m_parent == 0 )
		m_state = sync ;
	
	else if ( !fs::exists( path ) )
	{
		m_state = local_deleted ;
		Log( "%1% in state but not exist on disk: %2%", Name(), m_state ) ;
	}

	else
	{
		m_state = local_new ;
		
		// no need to compare MD5 or mtime for directories
		if ( !IsFolder() )
		{
			// to save time, compare mtime before checksum
			DateTime mtime = os::FileMTime( path ) ;
			if ( mtime > m_entry.MTime() )
			{
				Log( "%1% mtime newer on disk: %2%", Name(), m_state ) ;
				m_entry.Update( crypt::MD5( path ), mtime ) ;
			}
			else
				Log( "%1% unchanged on disk: %2%", Name(), m_state ) ;
		}
	}
}

std::string Resource::SelfHref() const
{
	return m_entry.SelfHref() ;
}

std::string Resource::Name() const
{
	return IsFolder() ? m_entry.Title() : m_entry.Filename() ;
}

std::string Resource::ResourceID() const
{
	return m_entry.ResourceID() ;
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

std::string Resource::ParentHref() const
{
	return m_entry.ParentHref() ;
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
	m_entry.Swap( coll.m_entry ) ;
	std::swap( m_parent, coll.m_parent ) ;
	m_child.swap( coll.m_child ) ;
	std::swap( m_state, coll.m_state ) ;
}

bool Resource::IsFolder() const
{
	return m_entry.Kind() == "folder" ;
}

fs::path Resource::Path() const
{
	assert( m_parent != this ) ;
	assert( m_parent == 0 || m_parent->IsFolder() ) ;

	return m_parent != 0 ? (m_parent->Path() / Name()) : "." ;
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
		if ( (*i)->Name() == name )
			return *i ;
	}
	return 0 ;
}

// try to change the state to "sync"
void Resource::Sync( http::Agent *http, const http::Headers& auth )
{
	// root folder is already synced
	if ( IsRoot() )
	{
		m_state = sync ;
		return ;
	}
	
	switch ( m_state )
	{
	case local_new :
		Log( "sync %1% doesn't exist in server. upload \"%2%\"?",
			Name(), m_parent->m_entry.CreateLink(), log::verbose ) ;
		
		if ( Create( http, auth ) )
			m_state = sync ;
		break ;
	
	case local_deleted :
		Log( "sync %1% deleted in local. delete?", m_entry.Filename(), log::verbose ) ;
		break ;
	
	case local_changed :
		Log( "sync %1% changed in local", m_entry.Filename(), log::verbose ) ;
		if ( EditContent( http, auth ) )
			m_state = sync ;
		break ;
	
	case remote_new :
	case remote_changed :
		Log( "sync %1% changed in remote. download?", m_entry.Filename(), log::verbose ) ;
		Download( http, Path(), auth ) ;
		m_state = sync ;
		break ;
	
	case sync :
		Log( "sync %1% already in sync", m_entry.Filename(), log::verbose ) ;
		break ;
		
	default :
		break ;
	}
}

void Resource::Delete( http::Agent *http, const http::Headers& auth )
{
	http::Headers hdr( auth ) ;
	hdr.push_back( "If-Match: " + m_entry.ETag() ) ;
	
	http::StringResponse str ;
	http->Custom( "DELETE", feed_base + "/" + m_entry.ResourceID() + "?delete=true", &str, hdr ) ;
}


void Resource::Download( http::Agent* http, const fs::path& file, const http::Headers& auth ) const
{
	Log( "Downloading %1%", file ) ;
	http::Download dl( file.string(), http::Download::NoChecksum() ) ;
	long r = http->Get( m_entry.ContentSrc(), &dl, auth ) ;
	if ( r <= 400 )
		os::SetFileTime( file, m_entry.MTime() ) ;
}

bool Resource::EditContent( http::Agent* http, const http::Headers& auth )
{
	assert( m_parent != 0 ) ;

	// sync parent first. make sure the parent folder exists in remote
	if ( m_parent->m_state != sync )
		m_parent->Sync( http, auth ) ;

	// upload link missing means that file is read only
	if ( m_entry.EditLink().empty() )
	{
		Log( "Cannot upload %1%: file read-only. %2%", m_entry.Title(), m_state, log::warning ) ;
		return false ;
	}
	
	return Upload( http, m_entry.EditLink(), auth, false ) ;
}

bool Resource::Create( http::Agent* http, const http::Headers& auth )
{
	assert( m_parent != 0 ) ;
	assert( m_parent->IsFolder() ) ;
	
	// sync parent first. make sure the parent folder exists in remote
	if ( m_parent->m_state != sync )
		m_parent->Sync( http, auth ) ;
	
	if ( IsFolder() )
	{
		std::string uri = feed_base ;
		if ( !m_parent->IsRoot() )
			uri += ("/" + http->Escape(m_parent->ResourceID()) + "/contents") ;
		
		std::string meta = (boost::format(xml_meta) % "folder" % Name() ).str() ;
		
		http::Headers hdr( auth ) ;
		hdr.push_back( "Content-Type: application/atom+xml" ) ;
		
		http::XmlResponse xml ;
// 		http::ResponseLog log( "create", ".xml", &xml ) ;
		http->Post( uri, meta, &xml, hdr ) ;
		m_entry.Update( xml.Response() ) ;

		return true ;
	}
	else if ( !m_parent->m_entry.CreateLink().empty() )
	{
		return Upload( http, m_parent->m_entry.CreateLink() + "?convert=false", auth, true ) ;
	}
	else
	{
		Log( "parent of %1% does not exist: cannot upload", Name() ) ;
		return false ;
	}
}

bool Resource::Upload( http::Agent* http, const std::string& link, const http::Headers& auth, bool post )
{
	Log( "Uploading %1%", m_entry.Title() ) ;
	
	StdioFile file( Path() ) ;
	
	// TODO: upload in chunks
	std::string data ;
	char buf[4096] ;
	std::size_t count = 0 ;
	while ( (count = file.Read( buf, sizeof(buf) )) > 0 )
		data.append( buf, count ) ;

	std::ostringstream xcontent_len ;
	xcontent_len << "X-Upload-Content-Length: " << data.size() ;
	
	http::Headers hdr( auth ) ;
	hdr.push_back( "Content-Type: application/atom+xml" ) ;
	hdr.push_back( "X-Upload-Content-Type: application/octet-stream" ) ;
	hdr.push_back( xcontent_len.str() ) ;
  	hdr.push_back( "If-Match: " + m_entry.ETag() ) ;
	hdr.push_back( "Expect:" ) ;
	
	std::string meta = (boost::format( xml_meta ) % m_entry.Kind() % Name()).str() ;
	
	http::StringResponse str ;
	if ( post )
		http->Post( link, meta, &str, hdr ) ;
	else
		http->Put( link, meta, &str, hdr ) ;
	
	http::Headers uphdr ;
	uphdr.push_back( "Expect:" ) ;
	uphdr.push_back( "Accept:" ) ;

	// the content upload URL is in the "Location" HTTP header
	std::string uplink = http->RedirLocation() ;
	http::XmlResponse xml ;
	
	http->Put( uplink, data, &xml, uphdr ) ;
	m_entry.Update( xml.Response() ) ;
	
	return true ;
}

Json Resource::Serialize() const
{
	Json result ;
	result.Add( "name",	Json(Name()) ) ;
	result.Add( "id",	Json(ResourceID()) ) ;
	result.Add( "href",	Json(SelfHref()) ) ;
	result.Add( "md5",	Json(m_entry.MD5()) ) ;
	result.Add( "kind",	Json(m_entry.Kind()) ) ;
	
	Json mtime ;
	mtime.Add( "sec",	Json(m_entry.MTime().Sec() ) );
	mtime.Add( "nsec",	Json(m_entry.MTime().NanoSec() ) );
	
	result.Add( "mtime",mtime ) ;
	
	std::vector<Json> array ;
	for ( std::vector<Resource*>::const_iterator i = m_child.begin() ; i != m_child.end() ; ++i )
		array.push_back( (*i)->Serialize() ) ;
	
	result.Add( "child", Json(array) ) ;
		
	return result ;

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
		"sync",	"local_new", "local_changed", "local_deleted", "remote_new", "remote_changed"
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

bool Resource::IsRoot() const
{
	return m_parent == 0 ;
}

} // end of namespace

namespace std
{
	void swap( gr::Resource& c1, gr::Resource& c2 )
	{
		c1.Swap( c2 ) ;
	}
}
