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

#include "String.hh"

namespace gr { namespace xml {

std::string Escape( const std::string& str )
{
	std::string result ;
	for ( std::string::const_iterator i = str.begin() ; i != str.end() ; ++i )
	{
		switch ( *i )
		{
		case '\"':	result.append( "&quot;" ) ;	break ;
		case '\'':	result.append( "&apos;" ) ;	break ;
		case '<':	result.append( "&lt;" ) ;	break ;
		case '>':	result.append( "&gt;" ) ;	break ;
		case '&':	result.append( "&amp;" ) ;	break ;
		default:	result.push_back( *i ) ;	break ;
		}
	}
	return result ;
}

} } // end of namespace
