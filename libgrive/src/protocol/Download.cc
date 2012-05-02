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

#include "Download.hh"
// #include "util/SignalHandler.hh"

#include <openssl/evp.h>

#include <cassert>
#include <new>
#include <stdexcept>

#include <signal.h>

namespace gr {

Download::Download( const std::string& filename ) :
	m_file( filename.c_str(), std::ios::out | std::ios::binary ),
	m_mdctx( ::EVP_MD_CTX_create() )
{
	if ( m_mdctx == 0 )
		throw std::bad_alloc() ;
	
	if ( ::EVP_DigestInit_ex( m_mdctx, ::EVP_md5(), 0 ) != 1 )
		throw std::runtime_error( "cannot create MD5 digest context" ) ;

	if ( !m_file )
		throw std::runtime_error( "cannot open file " + filename + " for writing" ) ;
}

Download::Download( const std::string& filename, NoChecksum ) :
	m_file( filename.c_str(), std::ios::out | std::ios::binary ),
	m_mdctx( 0 )
{
	if ( !m_file )
		throw std::runtime_error( "cannot open file " + filename + " for writing" ) ;
}

Download::~Download( )
{
	if ( m_mdctx != 0 )
		::EVP_MD_CTX_destroy( m_mdctx ) ;
}

std::string Download::Finish() const
{
	// Unregister the signal
// 	SignalHandler::GetInstance().UnregisterSignal( SIGINT ) ;

	std::string result ;

	// get the checksum and return it ;
	if ( m_mdctx != 0 )
	{
		unsigned int size = EVP_MAX_MD_SIZE ;
		result.resize( size ) ;
		
		if ( ::EVP_DigestFinal_ex( m_mdctx, reinterpret_cast<unsigned char*>(&result[0]), &size ) != 1 )
			throw std::runtime_error( "cannot calculate checksum" ) ;
		
		result.resize( size ) ;
	}

	return result ;
}

std::size_t Download::Callback( char *data, std::size_t size, std::size_t nmemb, Download *pthis )
{
	assert( pthis != 0 ) ;
	assert( data != 0 ) ;
	
	std::size_t count = size * nmemb ;
	
	if ( pthis->m_mdctx != 0 )
		::EVP_DigestUpdate( pthis->m_mdctx, data, count ) ;
	
	return pthis->m_file.rdbuf()->sputn( data, count ) ;
}

} // end of namespace
