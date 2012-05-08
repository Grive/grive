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

#include "JsonResponse.hh"

#include "protocol/Json.hh"

namespace gr { namespace http {

JsonResponse::JsonResponse()
{
}

void JsonResponse::Clear()
{
	m_resp.Clear() ;
}

std::size_t JsonResponse::OnData( void *data, std::size_t count )
{
	return m_resp.OnData( data, count ) ;
}

Json JsonResponse::Response() const
{
	return Json::Parse( m_resp.Response() ) ;
}

} } // end of namespace
