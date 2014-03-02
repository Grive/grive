/*
	grive: an GPL program to sync a local directory with Google Drive
	Copyright (C) 2013 Wan Wai Ho

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
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
	MA  02110-1301, USA.
*/

#include "ValResponse.hh"

#include "Val.hh"

namespace gr { namespace http {

ValResponse::ValResponse( ) :
	m_parser( &m_val )
{
}

std::size_t ValResponse::Write( const char *data, std::size_t count )
{
	m_parser.Parse( data, count ) ;
	return count ;
}

std::size_t ValResponse::Read( char *data, std::size_t count )
{
	return count ;
}

Val ValResponse::Response() const
{
	return m_val.Result() ;
}

void ValResponse::Finish()
{
	m_parser.Finish() ;
}

} } // end of namespace gr::http
