/*
	Common URIs for the old "Document List" Google Docs API
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

#pragma once

#include <string>

namespace gr { namespace v1
{
	const std::string feed_base		= "https://docs.google.com/feeds/default/private/full" ;
	const std::string feed_changes	= "https://docs.google.com/feeds/default/private/changes" ;
	const std::string feed_metadata	= "https://docs.google.com/feeds/metadata/default" ;
	
	const std::string root_href =
		"https://docs.google.com/feeds/default/private/full/folder%3Aroot" ;
	const std::string root_create =
		"https://docs.google.com/feeds/upload/create-session/default/private/full" ;
} }
