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

#pragma once

#include "base/Syncer.hh"

namespace gr {

class Feed;

namespace v2 {

class Syncer2: public Syncer
{

public :

	Syncer2( http::Agent *http );

	void DeleteRemote( Resource *res );
	void Download( Resource *res, const fs::path& file );
	bool EditContent( Resource *res, bool new_rev );
	bool Create( Resource *res );

	std::auto_ptr<Feed> GetFolders();
	std::auto_ptr<Feed> GetAll();
	std::auto_ptr<Feed> GetChanges( long min_cstamp );
	long GetChangeStamp( long min_cstamp );

private :

} ;

} } // end of namespace gr::v2
