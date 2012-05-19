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
#include "http/StringResponse.hh"
#include "http/XmlResponse.hh"
#include "util/Crypt.hh"
#include "util/Log.hh"
#include "util/OS.hh"
#include "xml/Node.hh"
#include "xml/NodeSet.hh"

#include <cassert>
#include <fstream>

// for debugging
#include <iostream>

namespace gr {

Resource::Resource( const xml::Node& entry ) :
	m_entry		( entry ),
	m_parent	( 0 )
{
}

Resource::Resource( const Entry& entry, Resource *parent ) :
	m_entry		( entry ),
	m_parent	( parent )
{
}

Resource::Resource(
	const std::string& name,
	const std::string& kind,
	const std::string& href ) :
	m_entry	( name, kind, href ),
	m_parent( 0 )
{
}

void Resource::Update( const Entry& e )
{
	m_entry = e ;
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
	return m_parent ;
}

Resource* Resource::Parent()
{
	return m_parent ;
}

std::string Resource::ParentHref() const
{
	return m_entry.ParentHref() ;
}

void Resource::AddChild( Resource *child )
{
	assert( child != 0 ) ;
	assert( child->m_parent == 0 ) ;
	assert( child != this ) ;

	child->m_parent = this ;
	m_child.push_back( child ) ;
}

void Resource::Swap( Resource& coll )
{
	m_entry.Swap( coll.m_entry ) ;
	std::swap( m_parent, coll.m_parent ) ;
	m_child.swap( coll.m_child ) ;
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

void Resource::Update( http::Agent *http, const http::Headers& auth )
{
	// no need to update for folders
	if ( IsFolder() )
		return ;

	assert( m_parent != 0 ) ;

	bool changed = true ;
	fs::path path = Path() ;

	Trace( "updating %1%", path ) ;
	
	// compare checksum first if file exists
	std::ifstream ifile( path.string().c_str(), std::ios::binary | std::ios::in ) ;
	if ( ifile && m_entry.ServerMD5() == crypt::MD5(ifile.rdbuf()) )
		changed = false ;

	// if the checksum is different, file is changed and we need to update
	if ( changed )
	{
		DateTime remote	= m_entry.ServerModified() ;
		DateTime local	= ifile ? os::FileMTime( path ) : DateTime() ;
		
		// remote file is newer, download file
		if ( !ifile || remote > local )
			Download( http, path, auth ) ;
		
		else
		{
			// re-reading the file
			ifile.seekg(0) ;
			Upload( http, ifile.rdbuf(), auth ) ;
		}
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
		os::SetFileTime( file, m_entry.ServerModified() ) ;
}

bool Resource::Upload( http::Agent* http, std::streambuf *file, const http::Headers& auth )
{
	// upload link missing means that file is read only
	if ( m_entry.UploadLink().empty() )
	{
		Log( "Cannot upload %1%: file read-only.", m_entry.Title(), log::warning ) ;
		return false ;
	}
	
	Log( "Uploading %1%", m_entry.Title() ) ;

	std::string meta =
	"<?xml version='1.0' encoding='UTF-8'?>\n"
	"<entry xmlns=\"http://www.w3.org/2005/Atom\" xmlns:docs=\"http://schemas.google.com/docs/2007\">"
		"<category scheme=\"http://schemas.google.com/g/2005#kind\" "
		"term=\"http://schemas.google.com/docs/2007#file\"/>"
		"<title>" + m_entry.Filename() + "</title>"
	"</entry>" ;

	std::string data(
		(std::istreambuf_iterator<char>(file)),
		(std::istreambuf_iterator<char>()) ) ;
	
	std::ostringstream xcontent_len ;
	xcontent_len << "X-Upload-Content-Length: " << data.size() ;
	
	http::Headers hdr( auth ) ;
	hdr.push_back( "Content-Type: application/atom+xml" ) ;
	hdr.push_back( "X-Upload-Content-Type: application/octet-stream" ) ;
	hdr.push_back( xcontent_len.str() ) ;
  	hdr.push_back( "If-Match: " + m_entry.ETag() ) ;
	hdr.push_back( "Expect:" ) ;
	
	http::StringResponse str ;
	http->Put( m_entry.UploadLink(), meta, &str, hdr ) ;
	
	std::string uplink = http->RedirLocation() ;
	
	// parse the header and find "Location"
	http::Headers uphdr ;
	uphdr.push_back( "Expect:" ) ;
	uphdr.push_back( "Accept:" ) ;
	
	http::XmlResponse xml ;
	http->Put( uplink, data, &xml, uphdr ) ;

	Trace( "Receipted response = %1%", xml.Response() ) ;
	
	m_entry.Update( xml.Response() ) ;
	
	return true ;
}

} // end of namespace

namespace std
{
	void swap( gr::Resource& c1, gr::Resource& c2 )
	{
		c1.Swap( c2 ) ;
	}
}
