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

#include "Feed.hh"

#include "http/Agent.hh"
#include "http/Header.hh"
#include "json/ValResponse.hh"

#include <iostream>

namespace gr { namespace v2 {

Feed::Feed( )
{
}

// for example to find dirs: '?q=mimeType%3d%27' + mime_types::folder + '%27'
void Feed::Start( http::Agent *http, const std::string& url )
{
	http::ValResponse out ;
	
	http->Get( url, &out, http::Header() ) ;
	
	m_content = out.Response() ;
}

bool Feed::GetNext( http::Agent *http )
{
	assert( http != 0 ) ;

	Val url ;
	if ( m_content.Get( "nextLink", url ) )
	{
		Start( http, url ) ;
		return true ;
	}
	else
		return false ;
}

} } // end of namespace
