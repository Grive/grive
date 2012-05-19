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

State::State( const fs::path& filename )
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
	return m_change_stamp ;
}

void State::ChangeStamp( const std::string& cs )
{
	m_change_stamp = cs ;
}

void State::Sync( const fs::path& p )
{
	Sync( p, m_folders.Root() ) ;
}

void State::Sync( const fs::path& p, gr::Resource* folder )
{
	assert( folder != 0 ) ;
	assert( folder->IsFolder() ) ;

	for ( fs::directory_iterator i( p ) ; i != fs::directory_iterator() ; ++i )
	{
		if ( fs::is_directory( i->path() ) )
		{
			Resource *c = new Resource( i->path().filename().string(), "folder", "" ) ;
			folder->AddChild( c ) ;
			m_folders.Insert( c ) ;

			Sync( *i, c ) ;
		}
		else if ( i->path().filename().string()[0] != '.' )
		{
Trace( "file: %1% %2%", i->path().filename().string(), folder->Path() ) ;
			Resource *c = new Resource( i->path().filename().string(), "file", "" ) ;
			folder->AddChild( c ) ;
			m_folders.Insert( c ) ;
		}
	}
}

void State::Write( const fs::path& filename ) const
{
	Json result ;
	result.Add( "change_stamp", Json( m_change_stamp ) ) ;
	
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
		m_unresolved.push_back( e ) ;
	}
}

void State::ResolveEntry()
{
	while ( !m_unresolved.empty() )
	{
		if ( TryResolveEntry() == 0 )
			break ;
	}
}

std::size_t State::TryResolveEntry()
{
	assert( !m_unresolved.empty() ) ;

	std::size_t count = 0 ;
	std::vector<Entry>& en = m_unresolved ;
	
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
	if ( e.Kind() != "folder" && e.Filename().empty() )
	{
		Log( "file \"%1%\" is a google document, ignored", e.Title() ) ;
		return true ;
	}
	
	if ( e.ParentHref().empty() )
	{
		Log( "\"%1%\" has no parent, ignored", e.Title() ) ;
		return true ;
	}
	
	Resource *parent = m_folders.FindByHref( e.ParentHref() ) ;
	if ( parent != 0 )
	{
if ( !parent->IsFolder() )
Trace( "name = \"%1%\" \"%2%\"", e.Title(), parent->Name() ) ;
		assert( parent->IsFolder() ) ;
	
		// see if the entry already exist in local
		Resource *child = parent->FindChild( e.Title() ) ;
		if ( child != 0 )
		{
			// since we are updating the ID and Href, we need to remove it and re-add it.
			m_folders.Update( child, e ) ;
		}
		
		// folder entry exist in google drive, but not local. we should create
		// the directory
		else if ( e.Kind() == "folder" || !e.Filename().empty() )
		{
			child = new Resource( e ) ;
			parent->AddChild( child ) ;
			m_folders.Insert( child ) ;
			
			fs::path child_path = child->Path() ;
			Trace( "added %1%", child_path ) ;
			
			if ( child->IsFolder() && !fs::is_directory( child_path ) )
			{
				Log( "creating %1% directory", child_path, log::info ) ;
				fs::create_directories( child_path ) ;
			}
		}
		return true ;
	}
	else
		return false ;
}

Resource* State::FindFolderByHref( const std::string& href )
{
	return m_folders.FindByHref( href ) ;
}

State::iterator State::begin()
{
	return m_folders.begin() ;
}

State::iterator State::end()
{
	return m_folders.end() ;
}

} // end of namespace
