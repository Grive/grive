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
#include "json/Val.hh"
#include "json/JsonParser.hh"

#include <fstream>

namespace gr {

State::State( const fs::path& filename, const Val& options  ) :
	m_res		( options["path"].Str() ),
	m_cstamp	( -1 )
{
	Read( filename ) ;
	
	bool force = options.Has( "force" ) ? options["force"].Bool() : false ;
	
	if ( options.Has( "ignore" ) && !m_ign.empty() && options["ignore"].Str() != m_ign )
	{
		// also "-f" is implicitly turned on when ignore regexp is changed
		// because without it grive would think that previously ignored files are deleted locally
		m_ign = options["ignore"].Str();
		force = true;
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
				force = true;
			m_ign = ign;
		}
	}
	
	// the "-f" option will make grive always think remote is newer
	if ( force )
	{
		m_last_change = DateTime() ;
		m_last_sync = DateTime::Now() ;
	}
	
	m_ign_re = boost::regex( m_ign.empty() ? "^\\.(grive|grive_state|trash)" : ( m_ign+"|^\\.(grive|grive_state|trash)" ) );
	
	Log( "last server change time: %1%; last sync time: %2%", m_last_change, m_last_sync, log::verbose ) ;
}

State::~State()
{
}

/// Synchronize local directory. Build up the resource tree from files and folders
/// of local directory.
void State::FromLocal( const fs::path& p )
{
	FromLocal( p, m_res.Root() ) ;
}

bool State::IsIgnore( const std::string& filename )
{
	return regex_search( filename.c_str(), m_ign_re );
}

void State::FromLocal( const fs::path& p, Resource* folder )
{
	assert( folder != 0 ) ;
	assert( folder->IsFolder() ) ;

	// sync the folder itself
	folder->FromLocal( m_last_sync ) ;

	for ( fs::directory_iterator i( p ) ; i != fs::directory_iterator() ; ++i )
	{
		std::string fname = i->path().filename().string() ;
		fs::file_status st = fs::status(i->path());
	
		std::string path = folder->IsRoot() ? fname : ( folder->RelPath() / fname ).string();
		if ( IsIgnore( path ) )
			Log( "file %1% is ignored by grive", path, log::verbose ) ;
		
		// check for broken symblic links
		else if ( st.type() == fs::file_not_found )
			Log( "file %1% doesn't exist (broken link?), ignored", i->path(), log::verbose ) ;
		
		else
		{
			bool is_dir = st.type() == fs::directory_file;
			// if the Resource object of the child already exists, it should
			// have been so no need to do anything here
			Resource *c = folder->FindChild( fname ) ;
			if ( c == 0 )
			{
				c = new Resource( fname, is_dir ? "folder" : "file" ) ;
				folder->AddChild( c ) ;
				m_res.Insert( c ) ;
			}
			
			c->FromLocal( m_last_sync ) ;
			
			if ( is_dir )
				FromLocal( *i, c ) ;
		}
	}
}

void State::FromRemote( const Entry& e )
{
	std::string fn = e.Filename() ;
	std::string k = e.IsDir() ? "folder" : "file";

	// common checkings
	if ( !e.IsDir() && (fn.empty() || e.ContentSrc().empty()) )
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

void State::FromChange( const Entry& e )
{
	assert( e.IsChange() ) ;
	
	// entries in the change feed is always treated as newer in remote,
	// so we override the last sync time to 0
	if ( Resource *res = m_res.FindByHref( e.SelfHref() ) )
		m_res.Update( res, e, DateTime() ) ;
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
		m_res.Update( res, e, m_last_change ) ;
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
		if ( child != 0 )
		{
			// since we are updating the ID and Href, we need to remove it and re-add it.
			m_res.Update( child, e, m_last_change ) ;
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
			m_res.Update( child, e, m_last_change ) ;
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
	m_last_sync.Assign( 0 ) ;
	m_last_change.Assign( 0 ) ;
	try
	{
		File file( filename ) ;

		Val json = ParseJson( file );
		Val last_sync = json["last_sync"] ;
		Val last_change = json.Has( "last_change" ) ? json["last_change"] : json["last_sync"] ;
		m_last_sync.Assign( last_sync["sec"].Int(), last_sync["nsec"].Int() ) ;
		m_last_change.Assign( last_change["sec"].Int(), last_change["nsec"].Int() ) ;
		m_ign = json.Has( "ignore_regexp" ) ? json["ignore_regexp"].Str() : std::string();

		m_cstamp = json["change_stamp"].Int() ;
	}
	catch ( Exception& )
	{
	}
}

void State::Write( const fs::path& filename ) const
{
	Val last_sync ;
	last_sync.Add( "sec",	Val( (int)m_last_sync.Sec() ) );
	last_sync.Add( "nsec",	Val( (unsigned)m_last_sync.NanoSec() ) );
	
	Val last_change ;
	last_change.Add( "sec",	Val( (int)m_last_change.Sec() ) );
	last_change.Add( "nsec",	Val( (unsigned)m_last_change.NanoSec() ) );
	
	Val result ;
	result.Add( "last_sync", last_sync ) ;
	result.Add( "last_change", last_change ) ;
	result.Add( "change_stamp", Val(m_cstamp) ) ;
	result.Add( "ignore_regexp", Val(m_ign) ) ;
	
	std::ofstream fs( filename.string().c_str() ) ;
	fs << result ;
}

void State::Sync( Syncer *syncer, const Val& options )
{
	// set the last sync time from the time returned by the server for the last file synced
	// if the sync time hasn't changed (i.e. now files have been uploaded)
	// set the last sync time to the time on the client
	// ideally because we compare server file times to the last sync time
	// the last sync time would always be a server time rather than a client time
	// TODO - WARNING - do we use the last sync time to compare to client file times
	// need to check if this introduces a new problem
	DateTime last_change_time = m_last_change;
	m_res.Root()->Sync( syncer, last_change_time, options ) ;
	
	if ( last_change_time == m_last_change )
		Trace( "nothing changed at the server side since %1%", m_last_change ) ;
	else
	{
		Trace( "updating last server-side change time to %1%", last_change_time ) ;
		m_last_change = last_change_time;
	}
	m_last_sync = DateTime::Now();
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

bool State::Move( Syncer* syncer, fs::path old_p, fs::path new_p, fs::path grive_root )
{
	//Convert paths to canonical representations
	//Also seems to remove trailing / at the end of directory paths
	old_p = fs::canonical( old_p );
	grive_root = fs::canonical( grive_root );
	
	//new_p is a little special because fs::canonical() requires that the path exists
	if ( new_p.string()[ new_p.string().size() - 1 ] == '/') //If new_p ends with a /, remove it
		new_p = new_p.parent_path();
	new_p = fs::canonical( new_p.parent_path() ) / new_p.filename();
	
	//Fails if source file doesn't exist, or if destination file already
	//exists and is not a directory, or if the source and destination are exactly the same
	if ( (fs::exists(new_p) && !fs::is_directory(new_p) ) || !fs::exists(old_p) || fs::equivalent( old_p, new_p ) )
		return false;
	
	//If new path is an existing directory, move the file into the directory
	//instead of trying to rename it
	if ( fs::is_directory(new_p) ){
		new_p = new_p / old_p.filename();
	}
	
	//Get the paths relative to grive root.
	//Just finds the substring from the end of the grive_root to the end of the path
	//+1s are to exclude slash at beginning of relative path
	int start = grive_root.string().size() + 1;
	int nLen = new_p.string().size() - (grive_root.string().size() + 1);
	int oLen = old_p.string().size() - (grive_root.string().size() + 1);
	if ( start + nLen != new_p.string().size() || start + oLen != old_p.string().size() )
		return false;
	fs::path new_p_rootrel( new_p.string().substr( start, nLen ) );
	fs::path old_p_rootrel( old_p.string().substr( start, oLen ) );
	
	//Get resources
	Resource* res = m_res.Root();
	Resource* newParentRes = m_res.Root();
	for ( fs::path::iterator it = old_p_rootrel.begin(); it != old_p_rootrel.end(); ++it )
	{
		if ( *it != "." && *it != ".." && res != 0 )
			res = res->FindChild(it->string());
		if ( *it == ".." )
			res = res->Parent();
	}
	for ( fs::path::iterator it = new_p_rootrel.begin(); it != new_p_rootrel.end(); ++it )
	{
		if ( *it != "." && *it != ".." && *it != new_p.filename() && newParentRes != 0 )
			newParentRes = newParentRes->FindChild(it->string());
		if ( *it == "..")
			res = res->Parent();
	}
	
	//These conditions should only occur if everything is not up-to-date
	if ( res == 0 || newParentRes == 0 || res->GetState() != Resource::sync ||
	     newParentRes->GetState() != Resource::sync || 
	     newParentRes->FindChild( new_p.filename().string() ) != 0 )
		return false;

	fs::rename(old_p, new_p); //Moves local file
	syncer->Move(res, newParentRes, new_p.filename().string()); //Moves server file
	return true;
}

} // end of namespace gr
