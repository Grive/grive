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

#include "Ignore.hh"

namespace po = boost::program_options;

namespace gr {

Ignore::Ignore( const Config& cfg ) : 
	m_cfg ( cfg )
{
	try {
		ignoreList = m_cfg.Get( "ignore" );
	} catch( Exception e) { // ignore not in config
		ignoreList = Json();
	}
}

void Ignore::Add( const std::string& filename ) {
	ignoreList.Add( filename, Json::Parse("1") );
}

void Ignore::Remove( const std::string& filename ) {
	ignoreList.Remove( filename );
}

bool Ignore::Contains( const std::string& filename ) {
	return ignoreList.Has(filename);
}

void Ignore::Save( )
{
	m_cfg.Set( "ignore", ignoreList );
	m_cfg.Save();
}



} // end of namespace


