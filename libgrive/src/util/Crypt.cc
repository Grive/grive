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

#include "Crypt.hh"

#include "StdioFile.hh"

#include <iomanip>
#include <sstream>

// dependent libraries
#include <openssl/evp.h>

namespace gr { namespace crypt {

std::string MD5( const fs::path& file )
{
	try
	{
		StdioFile sfile( file, "rb" ) ;
		return MD5( sfile ) ;
	}
	catch ( StdioFile::Error& )
	{
		return "" ;
	}
}

std::string MD5( StdioFile& file )
{
	char buf[64 * 1024] ;
	EVP_MD_CTX	md ;
	EVP_MD_CTX_init( &md );
	EVP_DigestInit_ex( &md, EVP_md5(), 0 ) ;
	
	std::size_t count = 0 ;
	while ( (count = file.Read( buf, sizeof(buf) )) > 0 )
	{
		EVP_DigestUpdate( &md, buf, count ) ;
	}
	
	unsigned int md5_size = EVP_MAX_MD_SIZE ;
	unsigned char md5[EVP_MAX_MD_SIZE] ;
	EVP_DigestFinal_ex( &md, md5, &md5_size ) ;
	
	// format the MD5 string
	std::ostringstream ss ;
	for ( unsigned int i = 0 ; i < md5_size ; i++ )
		ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(md5[i]) ;
	
	return ss.str() ;
}

} } // end of namespaces
