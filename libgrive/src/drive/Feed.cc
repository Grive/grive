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
#include "http/ResponseLog.hh"
#include "http/XmlResponse.hh"
#include "xml/NodeSet.hh"

#include <boost/format.hpp>

#include <cassert>

namespace gr {

Feed::Feed( )
{
}

Feed::Feed( const xml::Node& root ) :
	m_root		( root ),
	m_entries	( m_root["entry"] )
{
}

void Feed::Assign( const xml::Node& root )
{
	m_root		= root ;
	m_entries	= m_root["entry"] ;
}

Feed::iterator Feed::begin() const
{
	return iterator( m_entries.begin() ) ;
}

Feed::iterator Feed::end() const
{
	return iterator( m_entries.end() ) ;
}

std::string Feed::Next() const
{
	xml::NodeSet nss = m_root["link"].Find( "@rel", "next" ) ;
	return nss.empty() ? "" : std::string(nss["@href"]) ;
}

void Feed::Start( http::Agent *http, const http::Header& auth, const std::string& url )
{
	http::XmlResponse xrsp ;
	http::ResponseLog log( &xrsp ) ;
	
	if ( m_log.get() != 0 )
		log.Reset(
			(boost::format( "%1%-%2%." ) % m_log->prefix % m_log->sequence++).str(),
			m_log->suffix, &xrsp ) ;
	
	http->Get( url, &log, auth ) ;
	
	m_root		= xrsp.Response() ;
	m_entries	= m_root["entry"] ;
}

bool Feed::GetNext( http::Agent *http, const http::Header& auth )
{
	assert( http != 0 ) ;

	xml::NodeSet nss = m_root["link"].Find( "@rel", "next" ) ;
	if ( !nss.empty() )
	{
		Start( http, auth, nss["@href"] ) ;
		return true ;
	}
	else
		return false ;
}

Feed::iterator::iterator( )
{
}

Feed::iterator::iterator( xml::Node::iterator i )
{
	// for some reason, gcc 4.4.4 doesn't allow me to initialize the base class
	// in the initializer. I have no choice but to initialize here.
	base_reference() = i ;
}

Feed::iterator::reference Feed::iterator::dereference() const
{
	return Entry( *base_reference() ) ;
}

void Feed::EnableLog( const std::string& prefix, const std::string& suffix )
{
	m_log.reset( new LogInfo ) ;
	m_log->prefix	= prefix ;
	m_log->suffix	= suffix ;
	m_log->sequence	= 0 ;
}

const xml::Node& Feed::Root() const
{
	return m_root ;
}

} // end of namespace
