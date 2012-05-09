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

#include "XmlResponse.hh"

#include "xml/Node.hh"
#include "xml/TreeBuilder.hh"

namespace gr { namespace http {

XmlResponse::XmlResponse() : m_tb( new xml::TreeBuilder )
{
}

std::size_t XmlResponse::OnData( void *data, std::size_t count )
{
	m_tb->ParseData( reinterpret_cast<char*>(data), count ) ;
	return count ;
}

void XmlResponse::Clear()
{
	m_tb.reset( new xml::TreeBuilder ) ;
}

void XmlResponse::Finish()
{
	m_tb->ParseData( 0, 0, true ) ;
}

xml::Node XmlResponse::Response() const
{
	return m_tb->Result() ;
}

} } // end of namespace
