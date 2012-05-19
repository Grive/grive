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

#include "File.hh"

#include "Resource.hh"
#include "CommonUri.hh"

#include "http/Download.hh"
#include "http/StringResponse.hh"
#include "http/XmlResponse.hh"
#include "protocol/OAuth2.hh"
#include "util/Crypt.hh"
#include "util/DateTime.hh"
#include "util/OS.hh"
#include "util/Log.hh"
#include "xml/Node.hh"

#include <cassert>
#include <fstream>

namespace gr {

File::File( const Entry& entry, const Resource *parent ) :
	m_entry		( entry ),
	m_parent	( parent )
{
}

void File::Update( http::Agent *http, const http::Headers& auth )
{
	assert( m_parent != 0 ) ;

	bool changed = true ;
	fs::path path = m_parent->Dir() / m_entry.Filename() ;

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

void File::Delete( http::Agent *http, const http::Headers& auth )
{
	http::Headers hdr( auth ) ;
	hdr.push_back( "If-Match: " + m_entry.ETag() ) ;
	
	http::StringResponse str ;
	http->Custom( "DELETE", feed_base + "/" + m_entry.ResourceID() + "?delete=true", &str, hdr ) ;
}


void File::Download( http::Agent* http, const fs::path& file, const http::Headers& auth ) const
{
	Log( "Downloading %1%", file ) ;
	http::Download dl( file.string(), http::Download::NoChecksum() ) ;
	long r = http->Get( m_entry.ContentSrc(), &dl, auth ) ;
	if ( r <= 400 )
		os::SetFileTime( file, m_entry.ServerModified() ) ;
}

bool File::Upload( http::Agent* http, std::streambuf *file, const http::Headers& auth )
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

fs::path File::Path() const
{
	assert( m_parent != 0 ) ;
	return m_parent->Dir() / m_entry.Filename() ;
}

std::string File::ResourceID() const
{
	return m_entry.ResourceID() ;
}

} // end of namespace
