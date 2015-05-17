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

#include "CommonUri.hh"
#include "Feed1.hh"

#include "Entry1.hh"

#include "http/Agent.hh"
#include "http/Header.hh"
#include "http/ResponseLog.hh"
#include "http/XmlResponse.hh"
#include "xml/NodeSet.hh"

#include <boost/format.hpp>

#include <cassert>

namespace gr { namespace v1 {

Feed1::Feed1( const std::string &url ):
	Feed( url )
{
}

bool Feed1::GetNext( http::Agent *http )
{
	http::XmlResponse xrsp ;
	http::ResponseLog log( &xrsp ) ;
	
	if ( m_next.empty() )
		return false;
	
	if ( m_log.get() != 0 )
		log.Reset(
			m_log->prefix,
			(boost::format( "-#%1%%2%" ) % m_log->sequence++ % m_log->suffix ).str(),
			&xrsp ) ;
	
	http->Get( m_next, &log, http::Header() ) ;
	
	xml::Node m_root = xrsp.Response() ;
	xml::NodeSet xe = m_root["entry"] ;
	m_entries.clear() ;
	for ( xml::NodeSet::iterator i = xe.begin() ; i != xe.end() ; ++i )
	{
		m_entries.push_back( Entry1( *i ) );
	}
	
	xml::NodeSet nss = m_root["link"].Find( "@rel", "next" ) ;
	m_next = nss.empty() ? std::string( "" ) : nss["@href"];
	
	return true;
}

} } // end of namespace gr::v1
