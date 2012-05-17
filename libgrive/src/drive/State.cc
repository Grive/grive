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

#include "Collection.hh"
#include "CommonUri.hh"

#include "util/Crypt.hh"
#include "util/Log.hh"
#include "protocol/Json.hh"

#include <boost/bind.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>

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
	
	struct ByID {} ;
	struct ByPath {} ;
	
	typedef multi_index_container<
		Resource,
		indexed_by<
			hashed_non_unique<	tag<ByID>,	member<Resource, std::string,	&Resource::id> >,
			hashed_unique<		tag<ByPath>,member<Resource, fs::path,		&Resource::path>, PathHash >
		> 
	> ResourceSet ;
	
	typedef ResourceSet::index<ByID>::type		IDIdx ;
	typedef ResourceSet::index<ByPath>::type	PathIdx ;
	
	struct ByHref {} ;
	struct ByIdentity {} ;
	
	typedef multi_index_container<
		Collection*,
		indexed_by<
			hashed_non_unique<tag<ByHref>,	const_mem_fun<Collection, std::string,	&Collection::SelfHref> >,
			hashed_non_unique<tag<ByID>,	const_mem_fun<Collection, std::string,	&Collection::ResourceID> >,
			hashed_unique<tag<ByIdentity>,	identity<Collection*> >
		>
	> Folders ;
	
	typedef Folders::index<ByHref>::type		FoldersByHref ;
	typedef Folders::index<ByIdentity>::type	FolderSet ;
}

struct State::Impl
{
	ResourceSet rs ;
	Folders		folders ;
	std::string	change_stamp ;
	
	std::vector<Entry>	unresolved ;
} ;

State::State( const fs::path& filename ) :
	m_impl( new Impl )
{
	Collection *root = new Collection( ".", root_href ) ;
	m_impl->folders.insert( root ) ;

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
	FoldersByHref& idx = m_impl->folders.get<ByHref>() ;
	FoldersByHref::iterator it = idx.find( root_href ) ;
	
	assert( it != idx.end() ) ;
	Sync( p, *it ) ;
}

void State::Sync( const fs::path& p, Collection *folder )
{
// 	Trace( "synchronizing = %1%", p ) ;
	for ( fs::directory_iterator i( p ) ; i != fs::directory_iterator() ; ++i )
	{
// 		Trace( "file found = %2% (%1%)", i->path(), i->path().filename() ) ;
		if ( fs::is_directory( i->path() ) )
		{
			Collection *c = new Collection( i->path().filename().string(), "" ) ;
			folder->AddChild( c ) ;
			
			Sync( *i, c ) ;
		}
		else if ( i->path().filename().string()[0] != '.' )
			m_impl->rs.insert( Resource( i->path() ) ) ;
	}
}

void State::Write( const fs::path& filename ) const
{
	Json result ;
	result.Add( "change_stamp", Json( m_impl->change_stamp ) ) ;
	
	IDIdx& idx = m_impl->rs.get<ByID>() ;
	
	std::vector<Json> res ;
	std::transform( idx.begin(), idx.end(),
		std::back_inserter(res),
		boost::bind( &Resource::Get, _1 ) ) ;
	
	result.Add( "resources", Json(res) ) ;
	
// 	Trace( "%1%", result ) ;
	
	std::ofstream fs( filename.string().c_str() ) ;
	fs << result ;
}

void State::SetId( const fs::path& p, const std::string& id )
{
	PathIdx& pidx = m_impl->rs.get<ByPath>() ;
	PathIdx::iterator it = pidx.find( p ) ;
	if ( it != pidx.end() )
	{
		Resource r = *it ;
		r.id = id ;
		pidx.replace( it, r ) ;
	}
	else
	{
		Trace( "can't find %1%", p ) ;
	}
}

void State::OnEntry( const Entry& e )
{
	if ( !Update( e ) )
	{
		Trace( "can't find parent of %1%", e.Title() ) ;
		m_impl->unresolved.push_back( e ) ;
	}
}

void State::ResolveEntry()
{
	Trace( "trying to resolve %1% entries", m_impl->unresolved.size() ) ;
	while ( !m_impl->unresolved.empty() )
	{
		if ( TryResolveEntry() == 0 )
		{
			Trace( "cannot make progress" ) ;
			break ;
		}
	}
	
	Trace( "entries left = %1%", m_impl->unresolved.size() ) ;
}

std::size_t State::TryResolveEntry()
{
	assert( !m_impl->unresolved.empty() ) ;

	std::size_t count = 0 ;
	std::vector<Entry>& en = m_impl->unresolved ;
	
	for ( std::vector<Entry>::iterator i = en.begin() ; i != en.end() ; )
	{
		if ( Update( *i ) )
		{
			i = en.erase( i ) ;
			count++ ;
		}
		else
			++i ;
	}
	return count ;
}

bool State::Update( const Entry& e )
{
	FoldersByHref& folders = m_impl->folders.get<ByHref>() ;
	FoldersByHref::iterator i = folders.find( e.ParentHref() ) ;
	if ( i != folders.end() )
	{
		Trace( "found parent of folder %1%: %2%", e.Title(), (*i)->Title() ) ;
		
		// see if the entry already exist in local
		Collection *child = (*i)->FindChild( e.Title() ) ;
		if ( child != 0 )
		{
			// since we are updating the ID and Href, we need to remove it and re-add it.
			FolderSet& fs = m_impl->folders.get<ByIdentity>() ;
			FolderSet::iterator c = fs.find( child ) ;
			
			if ( c != fs.end() )
				fs.erase( c ) ;
				
			child->Update( e ) ;
			folders.insert( child ) ;
		}
		
		// folder entry exist in google drive, but not local.
		else
		{
			child = new Collection( e ) ;
			(*i)->AddChild( child ) ;
			folders.insert( child ) ;
		}
		return true ;
	}
	else
		return false ;
}

Collection* State::FindFolderByHref( const std::string& href )
{
	FoldersByHref& folders = m_impl->folders.get<ByHref>() ;
	FoldersByHref::iterator i = folders.find( href ) ;
	return i != folders.end() ? *i : 0 ;
}

} // end of namespace
