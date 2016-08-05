/*
	REST API item list ("Feed") implementation
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

#include "CommonUri.hh"
#include "Feed2.hh"
#include "Entry2.hh"

#include "http/Agent.hh"
#include "http/Header.hh"
#include "json/Val.hh"
#include "json/ValResponse.hh"

#include <iostream>
#include <boost/format.hpp>

namespace gr { namespace v2 {

Feed2::Feed2( const std::string& url ):
	Feed( url )
{
}

bool Feed2::GetNext( http::Agent *http )
{
	if ( m_next.empty() )
		return false ;
	
	http::ValResponse out ;
	http->Get( m_next, &out, http::Header(), 0 ) ;
	Val m_content = out.Response() ;
	
	Val::Array items = m_content["items"].AsArray() ;
	m_entries.clear() ;
	for ( Val::Array::iterator i = items.begin() ; i != items.end() ; ++i )
		m_entries.push_back( Entry2( *i ) );
	
	Val url ;
	m_next = m_content.Get( "nextLink", url ) ? url : std::string( "" ) ;
	return true ;
}

} } // end of namespace gr::v2
