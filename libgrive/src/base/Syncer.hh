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

#include "util/FileSystem.hh"

#include <string>
#include <vector>
#include <iosfwd>

namespace gr {

namespace http
{
	class Agent ;
}

class DateTime ;

class Resource ;

class Entry ;

class Feed ;

/*!	\brief	A Syncer incapsulates all resource-related upload/download/edit methods */
class Syncer
{
public :

	Syncer( http::Agent *http );

	http::Agent* Agent() const;

	virtual void DeleteRemote( Resource *res ) = 0;
	virtual void Download( Resource *res, const fs::path& file );
	virtual bool EditContent( Resource *res, bool new_rev ) = 0;
	virtual bool Create( Resource *res ) = 0;

	virtual std::auto_ptr<Feed> GetFolders() = 0;
	virtual std::auto_ptr<Feed> GetAll() = 0;
	virtual std::auto_ptr<Feed> GetChanges( long min_cstamp ) = 0;
	virtual long GetChangeStamp( long min_cstamp ) = 0;

protected:

	http::Agent *m_http;

	void AssignIDs( Resource *res, const Entry& remote );
	void AssignMTime( Resource *res, const DateTime& mtime );

} ;

} // end of namespace gr
