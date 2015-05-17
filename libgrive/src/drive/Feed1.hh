/*
	Item list ("Feed") implementation for the old "Document List" Google Docs API
	Copyright (C) 2012  Wan Wai Ho, (C) 2015 Vitaliy Filippov

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

#include "base/Feed.hh"

#include "xml/Node.hh"
#include "xml/NodeSet.hh"

#include <vector>

#include <string>

namespace gr { namespace v1 {

class Feed1: public Feed
{
public :
	Feed1( const std::string& url ) ;
	bool GetNext( http::Agent *http ) ;
} ;

} } // end of namespace gr::v1
