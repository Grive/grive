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
//#include "http/ResponseLog.hh"
#include "json/ValResponse.hh"
#include "json/JsonWriter.hh"

#include "util/OS.hh"
#include "util/log/Log.hh"

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

	return Upload( res ) ;
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
	
	return Upload( res );
}

bool Syncer2::Upload( Resource *res )
{
	Val meta;
	meta.Add( "title", Val( res->Name() ) );
	if ( res->IsFolder() )
	{
		meta.Add( "mimeType", Val( mime_types::folder ) );
	}
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

	// Issue metadata update request
	{
		http::Header hdr2 ;
		hdr2.Add( "Content-Type: application/json" );
		http::ValResponse vrsp ;
		long http_code = 0;
		if ( res->ResourceID().empty() )
			http_code = m_http->Post( feeds::files, json_meta, &vrsp, hdr2 ) ;
		else
			http_code = m_http->Put( feeds::files + "/" + res->ResourceID(), json_meta, &vrsp, hdr2 ) ;
		valr = vrsp.Response();
		assert( !( valr["id"].Str().empty() ) );
	}

	if ( !res->IsFolder() )
	{
		while ( true )
		{
			File file( res->Path() ) ;
			std::ostringstream xcontent_len ;
			xcontent_len << "Content-Length: " << file.Size() ;

			http::Header hdr ;
			hdr.Add( "Content-Type: application/octet-stream" ) ;
			hdr.Add( xcontent_len.str() ) ;
			if ( valr.Has( "etag" ) )
				hdr.Add( "If-Match: " + valr["etag"].Str() ) ;

			http::ValResponse vrsp;
			long http_code = m_http->Put( upload_base + "/" + valr["id"].Str() + "?uploadType=media", &file, &vrsp, hdr ) ;
			if ( http_code == 410 || http_code == 412 )
			{
				Log( "request failed with %1%, retrying whole upload in 5s", http_code, log::warning ) ;
				os::Sleep( 5 );
			}
			else
			{
				valr = vrsp.Response() ;
				assert( !( valr["id"].Str().empty() ) );
				break ;
			}
		}
	}

	Entry2 responseEntry = Entry2( valr ) ;
	AssignIDs( res, responseEntry ) ;
	AssignMTime( res, responseEntry.MTime() ) ;

	return true ;
}

std::auto_ptr<Feed> Syncer2::GetFolders()
{
	return std::auto_ptr<Feed>( new Feed2( feeds::files + "?maxResults=1000&q=%27me%27+in+readers+and+trashed%3dfalse+and+mimeType%3d%27" + mime_types::folder + "%27" ) );
}

std::auto_ptr<Feed> Syncer2::GetAll()
{
	return std::auto_ptr<Feed>( new Feed2( feeds::files + "?maxResults=1000&q=%27me%27+in+readers+and+trashed%3dfalse" ) );
}

std::string ChangesFeed( long changestamp, int maxResults = 1000 )
{
	boost::format feed( feeds::changes + "?maxResults=%1%&includeSubscribed=false" + ( changestamp > 0 ? "&startChangeId=%2%" : "" ) ) ;
	return ( changestamp > 0 ? feed % maxResults % changestamp : feed % maxResults ).str() ;
}

std::auto_ptr<Feed> Syncer2::GetChanges( long min_cstamp )
{
	return std::auto_ptr<Feed>( new Feed2( ChangesFeed( min_cstamp ) ) );
}

long Syncer2::GetChangeStamp( long min_cstamp )
{
	http::ValResponse res ;
	m_http->Get( ChangesFeed( min_cstamp, 1 ), &res, http::Header() ) ;

	return std::atoi( res.Response()["largestChangeId"].Str().c_str() );
}

} } // end of namespace gr::v1
