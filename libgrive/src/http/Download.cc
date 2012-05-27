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

#include "Error.hh"

// boost headers
#include <boost/throw_exception.hpp>
#include <boost/exception/errinfo_api_function.hpp>
#include <boost/exception/errinfo_at_line.hpp>
#include <boost/exception/errinfo_errno.hpp>
#include <boost/exception/errinfo_file_handle.hpp>
#include <boost/exception/errinfo_file_name.hpp>
#include <boost/exception/errinfo_file_open_mode.hpp>
#include <boost/exception/info.hpp>

#include <openssl/evp.h>

#include <cassert>
#include <new>

#include <signal.h>

namespace gr { namespace http {

Download::Download( const std::string& filename ) :
	m_file( filename, 0600 ),
	m_mdctx( ::EVP_MD_CTX_create() )
{
	if ( m_mdctx == 0 )
		throw std::bad_alloc() ;
	
	if ( ::EVP_DigestInit_ex( m_mdctx, ::EVP_md5(), 0 ) != 1 )
		BOOST_THROW_EXCEPTION( Error() << expt::ErrMsg( "cannot create MD5 digest context" ) ) ;
}

Download::Download( const std::string& filename, NoChecksum ) :
	m_file( filename, 0600 ),
	m_mdctx( 0 )
{
}

Download::~Download( )
{
	if ( m_mdctx != 0 )
		::EVP_MD_CTX_destroy( m_mdctx ) ;
}

void Download::Clear()
{
	// no need to do anything
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
			throw Error() << expt::ErrMsg( "cannot calculate checksum" ) ;
		
		result.resize( size ) ;
	}

	return result ;
}

std::size_t Download::OnData( void *data, std::size_t count )
{
	assert( data != 0 ) ;
	
	if ( m_mdctx != 0 )
		::EVP_DigestUpdate( m_mdctx, data, count ) ;
	
	return m_file.Write( data, count ) ;
}

} } // end of namespace
