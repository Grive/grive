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

#include <memory>
#include <string>

namespace gr { namespace xml {

class Node ;

class TreeBuilder
{
public :
	TreeBuilder() ;
	~TreeBuilder() ;
	
	void ParseData( const char *data, std::size_t count, bool last = false ) ;
	Node Result( ) const ;
	
	// one shot helpers
	static Node ParseFile( const std::string& file ) ;
	static Node Parse( const std::string& xml ) ;

private :
	
	static void StartElement( void* pvthis, const char* name, const char** attr ) ;
	static void EndElement( void* pvthis, const char* name ) ;
	static void OnCharData( void *pvthis, const char *s, int len ) ;

private :
	struct Impl ;
	std::auto_ptr<Impl>	m_impl ;
} ;

} } // end of namespace
