/*
	REST API Syncer implementation
	Copyright (C) 2015  Vitaliy Filippov

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
	bool EditContent( Resource *res, bool new_rev );
	bool Create( Resource *res );
	bool Move( Resource* res, Resource* newParent, std::string newFilename );

	std::unique_ptr<Feed> GetFolders();
	std::unique_ptr<Feed> GetAll();
	std::unique_ptr<Feed> GetChanges( long min_cstamp );
	long GetChangeStamp( long min_cstamp );

private :

	bool Upload( Resource *res, bool new_rev );

} ;

} } // end of namespace gr::v2
