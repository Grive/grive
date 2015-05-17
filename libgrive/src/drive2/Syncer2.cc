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

#include "base/Resource.hh"
#include "CommonUri.hh"
#include "Entry2.hh"
#include "Feed2.hh"
#include "Syncer2.hh"

#include "http/Agent.hh"
#include "http/Download.hh"
#include "http/Header.hh"
//#include "http/ResponseLog.hh"
#include "json/ValResponse.hh"

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
}

void Syncer2::Download( Resource *res, const fs::path& file )
{
}

bool Syncer2::EditContent( Resource *res, bool new_rev )
{
	return false ;
}

bool Syncer2::Create( Resource *res )
{
	return false ;
}

std::auto_ptr<Feed> Syncer2::GetFolders()
{
	return std::auto_ptr<Feed>( new Feed2( feeds::files + "?maxResults=1000&q=%27me%27+in+readers+and+mimeType%3d%27" + mime_types::folder + "%27" ) );
}

std::auto_ptr<Feed> Syncer2::GetAll()
{
	return std::auto_ptr<Feed>( new Feed2( feeds::files + "?maxResults=1000&q=%27me%27+in+readers" ) );
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
