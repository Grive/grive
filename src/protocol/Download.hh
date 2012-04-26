/*
	grive: an GPL program to sync a local directory with Google Drive
	Copyright (C) 2012  Wan Wai Ho

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; version 2
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
#include <fstream>

#include <openssl/evp.h>

namespace gr {

class Download
{
public :
	struct NoChecksum {} ;
	Download( const std::string& filename ) ;
	Download( const std::string& filename, NoChecksum ) ;
	~Download( ) ;
	
	std::string Finish() const ;
	
	static std::size_t Callback( char *data, std::size_t size, std::size_t nmemb, Download *pthis ) ;
	
private :
	std::ofstream	m_file ;
	EVP_MD_CTX		*m_mdctx ;
} ;

} // end of namespace
