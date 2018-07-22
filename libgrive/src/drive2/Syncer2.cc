/*
	REST API Syncer implementation
	Copyright (C) 2015  Vitaliy Filippov

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

#include "base/Resource.hh"
#include "CommonUri.hh"
#include "Entry2.hh"
#include "Feed2.hh"
#include "Syncer2.hh"

#include "http/Agent.hh"
#include "http/Download.hh"
#include "http/Header.hh"
#include "http/StringResponse.hh"
#include "json/ValResponse.hh"
#include "json/JsonWriter.hh"

#include "util/OS.hh"
#include "util/log/Log.hh"
#include "util/StringStream.hh"
#include "util/ConcatStream.hh"

#include <boost/exception/all.hpp>

#include <cassert>

// for debugging
#include <iostream>

namespace gr { namespace v2 {

Syncer2::Syncer2( http::Agent *http ):
	Syncer( http )
{
	assert( http != 0 ) ;
}

void Syncer2::DeleteRemote( Resource *res )
{
	http::StringResponse str ;
	http::Header hdr ;
	hdr.Add( "If-Match: " + res->ETag() ) ;
	m_http->Post( res->SelfHref() + "/trash", "", &str, hdr ) ;
}

bool Syncer2::EditContent( Resource *res, bool new_rev )
{
	assert( res->Parent() ) ;
	assert( !res->ResourceID().empty() ) ;
	assert( res->Parent()->GetState() == Resource::sync ) ;

	if ( !res->IsEditable() )
	{
		Log( "Cannot upload %1%: file read-only. %2%", res->Name(), res->StateStr(), log::warning ) ;
		return false ;
	}

	return Upload( res, new_rev ) ;
}

bool Syncer2::Create( Resource *res )
{
	assert( res->Parent() ) ;
	assert( res->Parent()->IsFolder() ) ;
	assert( res->Parent()->GetState() == Resource::sync ) ;
	assert( res->ResourceID().empty() ) ;
	
	if ( !res->Parent()->IsEditable() )
	{
		Log( "Cannot upload %1%: parent directory read-only. %2%", res->Name(), res->StateStr(), log::warning ) ;
		return false ;
	}
	
	return Upload( res, false );
}

bool Syncer2::Move( Resource* res, Resource* newParentRes, std::string newFilename )
{
	if ( res->ResourceID().empty() )
	{
		Log("Can't rename file %1%, no server id found", res->Name());
		return false;
	}

	Val meta;
	meta.Add( "title", Val(newFilename) );
	if ( res->IsFolder() )
	{
		meta.Add( "mimeType", Val( mime_types::folder ) );
	}
	std::string json_meta = WriteJson( meta );

	Val valr ;

	// Issue metadata update request
	{
		std::string addRemoveParents("");
		if (res->Parent()->IsRoot() )
			addRemoveParents += "&removeParents=root";
		else
			addRemoveParents += "&removeParents=" + res->Parent()->ResourceID();
		if ( newParentRes->IsRoot() )
			addRemoveParents += "&addParents=root";
		else
			addRemoveParents += "&addParents=" + newParentRes->ResourceID();
		http::Header hdr2 ;
		hdr2.Add( "Content-Type: application/json" );
		http::ValResponse vrsp ;
		// Don't change modified date because we're only moving
		long http_code = m_http->Put(
			feeds::files + "/" + res->ResourceID() + "?modifiedDateBehavior=noChange" + addRemoveParents,
			json_meta, &vrsp, hdr2
		) ;
		valr = vrsp.Response();
		assert( http_code == 200 && !( valr["id"].Str().empty() ) );
	}

	return true;
}

std::string to_string( uint64_t n )
{
	std::ostringstream s;
	s << n;
	return s.str();
}

bool Syncer2::Upload( Resource *res, bool new_rev )
{
	Val meta;
	meta.Add( "title", Val( res->Name() ) );
	if ( res->IsFolder() )
		meta.Add( "mimeType", Val( mime_types::folder ) );
	if ( !res->Parent()->IsRoot() )
	{
		Val parent;
		parent.Add( "id", Val( res->Parent()->ResourceID() ) );
		Val parents( Val::array_type );
		parents.Add( parent );
		meta.Add( "parents", parents );
	}
	std::string json_meta = WriteJson( meta );

	Val valr ;

	if ( res->IsFolder() )
	{
		// Only issue metadata update request
		http::Header hdr2 ;
		hdr2.Add( "Content-Type: application/json" );
		http::ValResponse vrsp ;
		long http_code = 0;
		if ( res->ResourceID().empty() )
			http_code = m_http->Post( feeds::files, json_meta, &vrsp, hdr2 ) ;
		else
			http_code = m_http->Put( feeds::files + "/" + res->ResourceID(), json_meta, &vrsp, hdr2 ) ;
		valr = vrsp.Response();
		assert( http_code == 200 && !( valr["id"].Str().empty() ) );
	}
	else
	{
		File file( res->Path() ) ;
		uint64_t size = file.Size() ;
		ConcatStream multipart ;
		StringStream p1(
			"--file_contents\r\nContent-Type: application/json; charset=utf-8\r\n\r\n" + json_meta +
			"\r\n--file_contents\r\nContent-Type: application/octet-stream\r\nContent-Length: " + to_string( size ) +
			"\r\n\r\n"
		);
		StringStream p2("\r\n--file_contents--\r\n");
		multipart.Append( &p1 );
		multipart.Append( &file );
		multipart.Append( &p2 );

		http::Header hdr ;
		if ( !res->ETag().empty() )
			hdr.Add( "If-Match: " + res->ETag() ) ;
		hdr.Add( "Content-Type: multipart/related; boundary=\"file_contents\"" );
		hdr.Add( "Content-Length: " + to_string( multipart.Size() ) );

		http::ValResponse vrsp;
		m_http->Request(
			res->ResourceID().empty() ? "POST" : "PUT",
			upload_base + ( res->ResourceID().empty() ? "" : "/" + res->ResourceID() ) +
			"?uploadType=multipart&newRevision=" + ( new_rev ? "true" : "false" ),
			&multipart, &vrsp, hdr
		) ;
		valr = vrsp.Response() ;
		assert( !( valr["id"].Str().empty() ) );
	}

	Entry2 responseEntry = Entry2( valr ) ;
	AssignIDs( res, responseEntry ) ;
	res->SetServerTime( responseEntry.MTime() );

	return true ;
}

std::unique_ptr<Feed> Syncer2::GetFolders()
{
	return std::unique_ptr<Feed>( new Feed2( feeds::files + "?maxResults=100000&q=trashed%3dfalse+and+mimeType%3d%27" + mime_types::folder + "%27" ) );
}

std::unique_ptr<Feed> Syncer2::GetAll()
{
	return std::unique_ptr<Feed>( new Feed2( feeds::files + "?maxResults=999999999&q=trashed%3dfalse" ) );
}

std::string ChangesFeed( long changestamp, int maxResults = 1000 )
{
	boost::format feed( feeds::changes + "?maxResults=%1%&includeSubscribed=false" + ( changestamp > 0 ? "&startChangeId=%2%" : "" ) ) ;
	return ( changestamp > 0 ? feed % maxResults % changestamp : feed % maxResults ).str() ;
}

std::unique_ptr<Feed> Syncer2::GetChanges( long min_cstamp )
{
	return std::unique_ptr<Feed>( new Feed2( ChangesFeed( min_cstamp ) ) );
}

long Syncer2::GetChangeStamp( long min_cstamp )
{
	http::ValResponse res ;
	m_http->Get( ChangesFeed( min_cstamp, 1 ), &res, http::Header(), 0 ) ;

	return std::atoi( res.Response()["largestChangeId"].Str().c_str() );
}

} } // end of namespace gr::v1
