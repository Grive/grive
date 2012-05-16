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

#include "State.hh"

#include "util/Crypt.hh"
#include "util/Log.hh"
#include "protocol/Json.hh"

#include <boost/bind.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>

#include <fstream>

namespace gr {

namespace
{
	struct Resource
	{
		std::string		id ;
		fs::path		path ;
		std::string		md5sum ;
		std::time_t		mtime ;
		
		explicit Resource( const fs::path& p ) :
			path( p ),
			md5sum( crypt::MD5( p ) ),
			mtime( fs::last_write_time( p ) )
		{
		}
		
		explicit Resource( const Json& json ) :
			id(		json["id"].Str() ),
			path(	json["path"].Str() ),
			md5sum(	json["md5"].Str() ),
			mtime(	json["mtime"].Int() )
		{
		}
		
		Json Get() const
		{
			Json entry ;
			entry.Add( "id",	Json( id ) ) ;
			entry.Add( "path",	Json( path.string() ) ) ;
			entry.Add( "md5",	Json( md5sum ) ) ;
			entry.Add( "mtime",	Json( mtime ) ) ;
			return entry ;
		}
	} ;

	struct PathHash
	{
		std::size_t operator()( const fs::path& p ) const
		{
			return boost::hash_value( p.string() ) ;
		}
	} ;
	
	using namespace boost::multi_index ;
	
	struct ById {} ;
	struct ByPath {} ;
	
	typedef multi_index_container<
		Resource,
		indexed_by<
			hashed_non_unique<	tag<ById>,	member<Resource, std::string,	&Resource::id> >,
			hashed_unique<		tag<ByPath>,member<Resource, fs::path,		&Resource::path>, PathHash >
		> 
	> ResourceSet ;
	
	typedef ResourceSet::index<ById>::type		IdIdx ;
	typedef ResourceSet::index<ByPath>::type	PathIdx ;
}

struct State::Impl
{
	ResourceSet rs ;
	std::string	change_stamp ;
} ;

State::State( const fs::path& filename ) :
	m_impl( new Impl )
{
	if ( fs::exists( filename ) )
		Read( filename );
}

void State::Read( const fs::path& filename )
{
Trace( "reading %1%", filename ) ;
	Json json = Json::ParseFile( filename.string() ) ;
	std::vector<Json> res = json["resources"].AsArray() ;
	
	for ( std::vector<Json>::iterator i = res.begin() ; i != res.end() ; ++i )
		m_impl->rs.insert( Resource( *i ) ) ;
	
	m_impl->change_stamp = json["change_stamp"].Str() ;
}

std::string State::ChangeStamp() const
{
	return m_impl->change_stamp ;
}

void State::ChangeStamp( const std::string& cs )
{
	m_impl->change_stamp = cs ;
}

void State::Sync( const fs::path& p )
{
	for ( fs::directory_iterator i( p ) ; i != fs::directory_iterator() ; ++i )
	{
		Trace( "file found = %1%", i->path() ) ;
		if ( fs::is_directory( i->path() ) )
			Sync( *i ) ;
		else if ( i->path().filename().string()[0] != '.' )
			m_impl->rs.insert( Resource( i->path() ) ) ;
	}
}

void State::Write( const fs::path& filename ) const
{
	Json result ;
	result.Add( "change_stamp", Json( m_impl->change_stamp ) ) ;
	
	IdIdx& idx = m_impl->rs.get<ById>() ;
	
	std::vector<Json> res ;
	std::transform( idx.begin(), idx.end(),
		std::back_inserter(res),
		boost::bind( &Resource::Get, _1 ) ) ;
	
	result.Add( "resources", Json(res) ) ;
	
	Trace( "%1%", result ) ;
	
	std::ofstream fs( filename.string().c_str() ) ;
	fs << result ;
}

Json State::FileInfo( const boost::filesystem::path& p )
{
	Json info ;
	info.Add( "mtime", Json( fs::last_write_time(p) ) ) ;
	return info ;
}

} // end of namespace
