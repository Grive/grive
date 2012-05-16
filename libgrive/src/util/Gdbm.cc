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

#include "Gdbm.hh"

#include <boost/exception/all.hpp>

#include <gdbm.h>

#include <cassert>

namespace gr {

struct Gdbm::Impl
{
	GDBM_FILE	db ;
} ;

Gdbm::Gdbm( const std::string& filename ) :
	m_impl( new Impl )
{
	std::string str( filename ) ;
	m_impl->db = ::gdbm_open( &str[0], 4096, GDBM_WRCREAT, 0600, &Gdbm::OnError ) ;
	if ( m_impl->db == 0 )
	{
		char *msg = gdbm_strerror(gdbm_errno) ;
		BOOST_THROW_EXCEPTION( Error() << expt::ErrMsg( msg ) ) ;
	}
}

Gdbm::~Gdbm()
{
}

std::string Gdbm::Get( const std::string& key ) const
{
	std::string ckey = key ;
	datum dkey = { &ckey[0], ckey.size() } ;
	datum val = gdbm_fetch( m_impl->db, dkey ) ;
	
	std::string result ;
	if ( val.dptr != 0 )
	{
		result.append( val.dptr, val.dsize ) ;
		::free( val.dptr ) ;
	}
	
	return result ;
}

void Gdbm::Set( const std::string& key, const std::string& val )
{
	assert( m_impl->db != 0 ) ;
	
	std::string ckey = key, cval = val ;
	datum dkey = { &ckey[0], ckey.size() } ;
	datum dval = { &cval[0], cval.size() } ;
	
	gdbm_store( m_impl->db, dkey, dval, GDBM_REPLACE ) ;
}

void Gdbm::OnError( )
{
	BOOST_THROW_EXCEPTION( Error() << expt::ErrMsg( gdbm_strerror(gdbm_errno) ) ) ;
}

} // end of namespace
