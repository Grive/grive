/*
	REST API item list ("Feed") implementation
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

#include "base/Feed.hh"

#include "util/Exception.hh"

#include <string>

namespace gr { namespace v2 {

class Feed2: public Feed
{
public :
	Feed2( const std::string& url ) ;
    ~Feed2() ;
	bool GetNext( http::Agent *http ) ;
} ;

} } // end of namespace gr::v2
