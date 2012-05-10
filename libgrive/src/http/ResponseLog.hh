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

#include "Receivable.hh"

#include <fstream>
#include <string>

namespace gr { namespace http {

class ResponseLog : public Receivable
{
public :
	ResponseLog(
		const std::string&	prefix,
		const std::string&	suffix,
		Receivable			*next ) ;

	std::size_t OnData( void *data, std::size_t count ) ;
	void Clear() ;

private :
	static std::string Filename( const std::string& prefix, const std::string& suffix ) ;
	
private :
	std::ofstream	m_log ;
	Receivable		*m_next ;
} ;

} } // end of namespace
