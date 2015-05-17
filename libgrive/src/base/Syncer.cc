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

#include "Syncer.hh"
#include "Resource.hh"
#include "Entry.hh"
#include "http/Agent.hh"
#include "http/Header.hh"
#include "http/Download.hh"
#include "util/OS.hh"
#include "util/log/Log.hh"

namespace gr {

Syncer::Syncer( http::Agent *http ):
	m_http( http )
{
}

http::Agent* Syncer::Agent() const
{
	return m_http;
}

void Syncer::Download( Resource *res, const fs::path& file )
{
	http::Download dl( file.string(), http::Download::NoChecksum() ) ;
	long r = m_http->Get( res->ContentSrc(), &dl, http::Header() ) ;
	if ( r <= 400 )
	{
		if ( res->MTime() != DateTime() )
			os::SetFileTime( file, res->MTime() ) ;
		else
			Log( "encountered zero date time after downloading %1%", file, log::warning ) ;
	}
}

void Syncer::AssignIDs( Resource *res, const Entry& remote )
{
	res->AssignIDs( remote );
}

void Syncer::AssignMTime( Resource *res, const DateTime& mtime )
{
	res->m_mtime = mtime;
}

} // end of namespace gr
