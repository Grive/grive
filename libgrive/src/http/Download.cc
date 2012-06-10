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
#include "util/Crypt.hh"

// boost headers
#include <boost/throw_exception.hpp>
#include <boost/exception/errinfo_api_function.hpp>
#include <boost/exception/errinfo_at_line.hpp>
#include <boost/exception/errinfo_errno.hpp>
#include <boost/exception/errinfo_file_handle.hpp>
#include <boost/exception/errinfo_file_name.hpp>
#include <boost/exception/errinfo_file_open_mode.hpp>
#include <boost/exception/info.hpp>

#include <cassert>
#include <new>

#include <signal.h>

namespace gr { namespace http {

Download::Download( const std::string& filename ) :
	m_file( filename, 0600 ),
	m_crypt( new crypt::MD5 )
{
}

Download::Download( const std::string& filename, NoChecksum ) :
	m_file( filename, 0600 )
{
}

Download::~Download()
{
}

void Download::Clear()
{
	// no need to do anything
}

std::string Download::Finish() const
{
	return m_crypt.get() != 0 ? m_crypt->Get() : "" ;
}

std::size_t Download::OnData( void *data, std::size_t count )
{
	assert( data != 0 ) ;
	
	if ( m_crypt.get() != 0 )
		m_crypt->Write( data, count ) ;
	
	return m_file.Write( data, count ) ;
}

} } // end of namespace
