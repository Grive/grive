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

Feed::Feed( const std::string& base ) :
	m_base( base )
{
	// Next() will grab this link
	m_content.Add( "nextLink", Val(base) ) ;
}

void Feed::Query( const std::string& field, const std::string& value )
{
	std::string url = m_content["nextLink"].Str() ;
	m_content.Add( "nextLink", Val( url + "?q=" + field + "+%3d+%27" + value + "%27" ) ) ;
}

bool Feed::Next( http::Agent *agent )
{
	Val url ;
	if ( !m_content.Get("nextLink", url) )
		return false ;
	
	http::ValResponse out ;
	try
	{
		agent->Get( url.Str(), &out, http::Header() ) ;
	}
	catch ( Exception& e )
	{
		e << DriveFeed_( m_content ) ;
		throw ;
	}
	m_content = out.Response() ;
	
	return true ;
}

Val Feed::Content() const
{
	return m_content ;
}

} } // end of namespace
