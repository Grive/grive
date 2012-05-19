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

#include "Resource.hh"
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

struct State::Impl
{
// 	ResourceSet		rs ;
	FolderSet		folders ;
	std::string		change_stamp ;
	
	std::vector<Entry>	unresolved ;
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
	Sync( p, m_impl->folders.Root() ) ;
}

void State::Sync( const boost::filesystem3::path& p, gr::Resource* folder )
{
	assert( folder != 0 ) ;

	for ( fs::directory_iterator i( p ) ; i != fs::directory_iterator() ; ++i )
	{
		if ( fs::is_directory( i->path() ) )
		{
			Resource *c = new Resource( i->path().filename().string(), "" ) ;
			folder->AddChild( c ) ;
			m_impl->folders.Insert( c ) ;

			Sync( *i, c ) ;
		}
// 		else if ( i->path().filename().string()[0] != '.' )
// 			m_impl->rs.insert( Resource( i->path() ) ) ;
	}
}

void State::Write( const fs::path& filename ) const
{
	Json result ;
	result.Add( "change_stamp", Json( m_impl->change_stamp ) ) ;
	
	std::ofstream fs( filename.string().c_str() ) ;
	fs << result ;
}

void State::SetId( const fs::path& p, const std::string& id )
{
}

void State::OnEntry( const Entry& e )
{
	if ( !Update( e ) )
	{
		Trace( "can't resolve folder %1%", e.Title() ) ;
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
		Trace( "resolving %1%", i->Title() ) ;
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
	Resource *parent = m_impl->folders.FindByHref( e.ParentHref() ) ;
	if ( parent != 0 )
	{
		Trace( "found parent of folder %1%: %2%", e.Title(), parent->Title() ) ;
		
		// see if the entry already exist in local
		Resource *child = parent->FindChild( e.Title() ) ;
		if ( child != 0 )
		{
			// since we are updating the ID and Href, we need to remove it and re-add it.
			m_impl->folders.Update( child, e ) ;
		}
		
		// folder entry exist in google drive, but not local. we should create
		// the directory
		else
		{
			child = new Resource( e ) ;
			parent->AddChild( child ) ;
			m_impl->folders.Insert( child ) ;
			
			Trace( "creating %1% directory", child->Path() ) ;
			fs::create_directories( child->Path() ) ;
		}
		return true ;
	}
	else
		return false ;
}

Resource* State::FindFolderByHref( const std::string& href )
{
	return m_impl->folders.FindByHref( href ) ;
}

} // end of namespace
