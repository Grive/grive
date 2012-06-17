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

#include "MockAgent.hh"

namespace gr { namespace http {

MockAgent::MockAgent()
{
}

long MockAgent::Put(
	const std::string&		,
	const std::string&		,
	Receivable				*,
	const Header&			)
{
	return 200 ;
}

long MockAgent::Get(
	const std::string& 		,
	Receivable				*,
	const Header&			 )
{
	return 200 ;
}

long MockAgent::Post(
	const std::string& 		,
	const std::string&		,
	Receivable				*,
	const Header&			 )
{
	return 200 ;
}

long MockAgent::Custom(
	const std::string&		,
	const std::string&		,
	Receivable				*,
	const Header&			 )
{
	return 200 ;
}

std::string MockAgent::RedirLocation() const
{
	return "" ;
}

std::string MockAgent::Escape( const std::string& str )
{
	return str ;
}

std::string MockAgent::Unescape( const std::string& str )
{
	return str ;
}

} } // end of namespace
