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

#include "Entry.hh"
#include "Resource.hh"
#include "Syncer.hh"

#include "util/Crypt.hh"
#include "util/File.hh"
#include "util/log/Log.hh"
#include "json/JsonParser.hh"

#include <fstream>

namespace gr {

State::State( const fs::path& filename, const Val& options  ) :
	m_res		( options["path"].Str() ),
	m_cstamp	( -1 )
{
	Read( filename ) ;
	
	// the "-f" option will make grive always think remote is newer
	m_force = options.Has( "force" ) ? options["force"].Bool() : false ;
	
	if ( options.Has( "ignore" ) && options["ignore"].Str() != m_ign )
	{
		// also "-f" is implicitly turned on when ignore regexp is changed
		// because without it grive would think that previously ignored files are deleted locally
		if ( !m_ign.empty() )
			m_force = true;
		m_ign = options["ignore"].Str();
	}
	else if ( options.Has( "dir" ) )
	{
		const boost::regex trim_path( "^/+|/+$" );
		std::string m_dir = regex_replace( options["dir"].Str(), trim_path, "" );
		if ( !m_dir.empty() )
		{
			// "-s" is internally converted to an ignore regexp
			const boost::regex esc( "[.^$|()\\[\\]{}*+?\\\\]" );
			std::string ign = "^(?!"+regex_replace( m_dir, esc, "\\\\&", boost::format_sed )+"(/|$))";
			if ( !m_ign.empty() && ign != m_ign )
				m_force = true;
			m_ign = ign;
		}
	}
	
	m_ign_re = boost::regex( m_ign.empty() ? "^\\.(grive|grive_state|trash)" : ( m_ign+"|^\\.(grive|grive_state|trash)" ) );
}

State::~State()
{
}

/// Synchronize local directory. Build up the resource tree from files and folders
/// of local directory.
void State::FromLocal( const fs::path& p )
{
	m_res.Root()->FromLocal( m_st ) ;
	FromLocal( p, m_res.Root(), m_st.Item( "tree" ) ) ;
}

bool State::IsIgnore( const std::string& filename )
{
	return regex_search( filename.c_str(), m_ign_re );
}

void State::FromLocal( const fs::path& p, Resource* folder, Val& tree )
{
	assert( folder != 0 ) ;
	assert( folder->IsFolder() ) ;

	Val::Object leftover = tree.AsObject();

	for ( fs::directory_iterator i( p ) ; i != fs::directory_iterator() ; ++i )
	{
		std::string fname = i->path().filename().string() ;
		std::string path = folder->IsRoot() ? fname : ( folder->RelPath() / fname ).string();
		
		if ( IsIgnore( path ) )
			Log( "file %1% is ignored by grive", path, log::verbose ) ;
		else
		{
			// if the Resource object of the child already exists, it should
			// have been so no need to do anything here
			Resource *c = folder->FindChild( fname ) ;
			if ( !c )
			{
				c = new Resource( fname, "" ) ;
				folder->AddChild( c ) ;
				m_res.Insert( c ) ;
			}
			leftover.erase( fname );
			Val& rec = tree.Item( fname );
			if ( m_force )
				rec.Del( "srv_time" );
			c->FromLocal( rec ) ;
			if ( c->IsFolder() )
				FromLocal( *i, c, rec.Item( "tree" ) ) ;
		}
	}

	for( Val::Object::iterator i = leftover.begin(); i != leftover.end(); i++ )
	{
		// Restore state of locally deleted files
		Resource *c = folder->FindChild( i->first ) ;
		if ( !c )
		{
			c = new Resource( i->first, i->second.Has( "tree" ) ? "folder" : "file" ) ;
			folder->AddChild( c ) ;
			m_res.Insert( c ) ;
		}
		Val& rec = tree.Item( i->first );
		if ( m_force )
			rec.Del( "srv_time" );
		c->FromDeleted( rec );
	}
}

void State::FromRemote( const Entry& e )
{
	std::string fn = e.Filename() ;
	std::string k = e.IsDir() ? "folder" : "file";

	// common checkings
	if ( !e.IsDir() && ( fn.empty() || e.ContentSrc().empty() ) )
		Log( "%1% \"%2%\" is a google document, ignored", k, e.Name(), log::verbose ) ;
	
	else if ( fn.find('/') != fn.npos )
		Log( "%1% \"%2%\" contains a slash in its name, ignored", k, e.Name(), log::verbose ) ;
	
	else if ( !e.IsChange() && e.ParentHrefs().size() != 1 )
		Log( "%1% \"%2%\" has multiple parents, ignored", k, e.Name(), log::verbose ) ;

	else if ( e.IsChange() )
		FromChange( e ) ;

	else if ( !Update( e ) )
		m_unresolved.push_back( e ) ;
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
	std::list<Entry>& en = m_unresolved ;

	for ( std::list<Entry>::iterator i = en.begin() ; i != en.end() ; )
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

void State::FromChange( const Entry& e )
{
	assert( e.IsChange() ) ;
	
	// entries in the change feed is always treated as newer in remote,
	// so we override the last sync time to 0
	if ( Resource *res = m_res.FindByHref( e.SelfHref() ) )
		m_res.Update( res, e ) ;
}

bool State::Update( const Entry& e )
{
	assert( !e.IsChange() ) ;
	assert( !e.ParentHref().empty() ) ;

	if ( Resource *res = m_res.FindByHref( e.SelfHref() ) )
	{
		std::string path = res->RelPath().string();
		if ( IsIgnore( path ) )
		{
			Log( "%1% is ignored by grive", path, log::verbose ) ;
			return true;
		}
		m_res.Update( res, e ) ;
		return true;
	}
	else if ( Resource *parent = m_res.FindByHref( e.ParentHref() ) )
	{
		assert( parent->IsFolder() ) ;

		std::string path = parent->IsRoot() ? e.Name() : ( parent->RelPath() / e.Name() ).string();
		if ( IsIgnore( path ) )
		{
			Log( "%1% is ignored by grive", path, log::verbose ) ;
			return true;
		}

		// see if the entry already exist in local
		std::string name = e.Name() ;
		Resource *child = parent->FindChild( name ) ;
		if ( child )
		{
			// since we are updating the ID and Href, we need to remove it and re-add it.
			m_res.Update( child, e ) ;
		}
		
		// folder entry exist in google drive, but not local. we should create
		// the directory
		else if ( e.IsDir() || !e.Filename().empty() )
		{
			// first create a dummy resource and update it later
			child = new Resource( name, e.IsDir() ? "folder" : "file" ) ;
			parent->AddChild( child ) ;
			m_res.Insert( child ) ;
			
			// update the state of the resource
			m_res.Update( child, e ) ;
		}
		
		return true ;
	}
	else
		return false ;
}

Resource* State::FindByHref( const std::string& href )
{
	return m_res.FindByHref( href ) ;
}

State::iterator State::begin()
{
	return m_res.begin() ;
}

State::iterator State::end()
{
	return m_res.end() ;
}

void State::Read( const fs::path& filename )
{
	try
	{
		File file( filename ) ;

		m_st = ParseJson( file );
		m_ign = m_st.Has( "ignore_regexp" ) ? m_st["ignore_regexp"].Str() : std::string();

		m_cstamp = m_st["change_stamp"].Int() ;
	}
	catch ( Exception& )
	{
	}
}

void State::Write( const fs::path& filename )
{
	m_st.Set( "change_stamp", Val( m_cstamp ) ) ;
	m_st.Set( "ignore_regexp", Val( m_ign ) ) ;
	
	std::ofstream fs( filename.string().c_str() ) ;
	fs << m_st ;
}

void State::Sync( Syncer *syncer, const Val& options )
{
	// set the last sync time to the time on the client
	m_res.Root()->Sync( syncer, &m_res, options ) ;
}

long State::ChangeStamp() const
{
	return m_cstamp ;
}

void State::ChangeStamp( long cstamp )
{
	Log( "change stamp is set to %1%", cstamp, log::verbose ) ;
	m_cstamp = cstamp ;
}

} // end of namespace gr
