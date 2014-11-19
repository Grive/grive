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

#pragma once

#include "json/Val.hh"
#include "util/Exception.hh"

#include <string>

namespace gr
{
namespace http
{
	class Agent ;
	class Header ;
}

class Val ;

namespace v2 {

class Feed
{
public :
	// exception info
	typedef boost::error_info<struct DriveFeed,	Val>	DriveFeed_ ;

public :
	Feed( const std::string& base ) ;
	void Query( const std::string& field, const std::string& value ) ;


	bool Next( http::Agent *agent ) ;

	Val Content() const ;

private :
	std::string	m_base ;
	Val 		m_content ;
} ;

} } // end of namespace gr::v2
