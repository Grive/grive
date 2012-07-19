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

#include "AuthAgent.hh"
#include "http/Header.hh"

#include <cassert>

namespace gr {

using namespace http ;

AuthAgent::AuthAgent( const OAuth2& auth, std::auto_ptr<Agent> real_agent ) :
	m_auth	( auth ),
	m_agent	( real_agent )
{
	assert( m_agent.get() != 0 ) ;
}

Header AuthAgent::AppendHeader( const Header& hdr ) const
{
	Header h(hdr) ;
	h.Add( "Authorization: Bearer " + m_auth.AccessToken() ) ;
	h.Add( "GData-Version: 3.0" ) ;
	return h ;
}

long AuthAgent::Put(
	const std::string&	url,
	const std::string&	data,
	Receivable			*dest,
	const Header&		hdr )
{
	return m_agent->Put( url, data, dest, AppendHeader(hdr) ) ;
}

long AuthAgent::Put(
	const std::string&	url,
	StdioFile&			file,
	Receivable			*dest,
	const Header&		hdr )
{
	return m_agent->Put( url, file, dest, AppendHeader(hdr) ) ;
}

long AuthAgent::Get(
	const std::string& 	url,
	Receivable			*dest,
	const Header&		hdr )
{
	return m_agent->Get( url, dest, AppendHeader(hdr) ) ;
}

long AuthAgent::Post(
	const std::string& 	url,
	const std::string&	data,
	Receivable			*dest,
	const Header&		hdr )
{
	return m_agent->Post( url, data, dest, AppendHeader(hdr) ) ;
}

long AuthAgent::Custom(
	const std::string&	method,
	const std::string&	url,
	Receivable			*dest,
	const Header&		hdr )
{
	return m_agent->Custom( method, url, dest, AppendHeader(hdr) ) ;
}

std::string AuthAgent::RedirLocation() const
{
	return m_agent->RedirLocation() ;
}

std::string AuthAgent::Escape( const std::string& str )
{
	return m_agent->Escape( str ) ;
}

std::string AuthAgent::Unescape( const std::string& str )
{
	return m_agent->Unescape( str ) ;
}

} // end of namespace
