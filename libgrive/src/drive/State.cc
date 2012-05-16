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

namespace gr {

namespace fs = boost::filesystem ;

State::State( const std::string& filename ) :
	m_db( filename )
{
}

void State::Sync( const fs::path& p )
{
	for ( fs::directory_iterator i( p ) ; i != fs::directory_iterator() ; ++i )
	{
		Trace( "file found = %1%", i->path() ) ;
		
		std::string json = m_db.Get( i->path().string() ) ;
		if ( json.empty() )
		{
			Trace( "create new entry" ) ;
			json = FileInfo(p).Str() ;
		}
		else
		{
			Json j = Json::Parse( json ) ;
			if ( j["mtime"].Int() != fs::last_write_time(p) )
			{
				Trace( "file changed" ) ;
				json = FileInfo(p).Str() ;
			}
		}
		
		m_db.Set( i->path().string(), json ) ;
	}
}

Json State::FileInfo( const boost::filesystem::path& p )
{
	Json info ;
	info.Add( "mtime", Json( fs::last_write_time(p) ) ) ;
	return info ;
}

} // end of namespace
