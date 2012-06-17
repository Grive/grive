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

#include "Agent.hh"

namespace gr { namespace http {

/*!	\brief	HTTP mock agent

	This HTTP agent does nothing. It is used for unit test and dry runs.
*/
class MockAgent : public Agent
{
public :
	MockAgent() ;
	
	long Put(
		const std::string&	url,
		const std::string&	data,
		Receivable			*dest,
		const Header&		hdr ) ;

	long Get(
		const std::string& 	url,
		Receivable			*dest,
		const Header&		hdr ) ;
	
	long Post(
		const std::string& 	url,
		const std::string&	data,
		Receivable			*dest,
		const Header&		hdr ) ;
	
	long Custom(
		const std::string&	method,
		const std::string&	url,
		Receivable			*dest,
		const Header&		hdr ) ;
	
	std::string RedirLocation() const ;
	
	std::string Escape( const std::string& str ) ;
	std::string Unescape( const std::string& str ) ;
} ;

} } // end of namespace
