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
#include "Exception.hh"

#include <iomanip>
#include <sstream>

// dependent libraries
#include <gcrypt.h>
#include <boost/throw_exception.hpp>

namespace gr { namespace crypt {

const std::size_t read_size = 8 * 1024 ;

std::string MD5( const fs::path& file )
{
	try
	{
		StdioFile sfile( file ) ;
		return MD5( sfile ) ;
	}
	catch ( StdioFile::Error& )
	{
		return "" ;
	}
}

std::string MD5( StdioFile& file )
{
	char buf[read_size] ;
	
	gcry_md_hd_t hd ;
	gcry_error_t err = gcry_md_open( &hd, GCRY_MD_MD5, 0 ) ;
	if ( err != GPG_ERR_NO_ERROR )
	{
		BOOST_THROW_EXCEPTION( Exception() << expt::ErrMsg( gcry_strerror(err) ) ) ;
	}
	
	std::size_t count = 0 ;
	while ( (count = file.Read( buf, sizeof(buf) )) > 0 )
		gcry_md_write( hd, buf, count ) ;

	unsigned char *md5 = gcry_md_read( hd, GCRY_MD_MD5 ) ;
	unsigned int  len  = gcry_md_get_algo_dlen(GCRY_MD_MD5) ;
	
	// format the MD5 string
	std::ostringstream ss ;
	for ( unsigned int i = 0 ; i < len ; i++ )
		ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(md5[i]) ;
	
	gcry_md_close( hd ) ;
	
	return ss.str() ;
}

} } // end of namespaces
