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

#include "StringResponse.hh"

namespace gr { namespace http {

StringResponse::StringResponse()
{
}

void StringResponse::Clear()
{
	m_resp.clear() ;
}

std::size_t StringResponse::OnData( void *data, std::size_t count )
{
	m_resp.append( reinterpret_cast<char*>(data), count ) ;
	return count ;
}

const std::string& StringResponse::Response() const
{
	return m_resp ;
}

} } // end of namespace
