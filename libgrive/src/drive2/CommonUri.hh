/*
	Common URIs for REST API
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

#pragma once

#include <string>

namespace gr { namespace v2 {

const std::string upload_base = "https://www.googleapis.com/upload/drive/v2/files" ;

namespace feeds
{
	const std::string files		= "https://www.googleapis.com/drive/v2/files" ;
	const std::string changes	= "https://www.googleapis.com/drive/v2/changes" ;
}

namespace mime_types
{
	const std::string folder	= "application/vnd.google-apps.folder" ;
}

} } // end of namespace gr::v2
