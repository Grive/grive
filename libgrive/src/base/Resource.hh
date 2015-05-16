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

#include "util/DateTime.hh"
#include "util/Exception.hh"
#include "util/FileSystem.hh"

#include <string>
#include <vector>
#include <iosfwd>

namespace gr {

class Syncer ;

class Val ;

class Entry ;

/*!	\brief	A resource can be a file or a folder in the google drive

	The google drive contains a number of resources, which is represented by this class.
	It also contains linkage to other resources, such as parent and childrens.
*/
class Resource
{
public :
	typedef std::vector<Resource*> Children ;
	typedef Children::const_iterator iterator ;
	
	/// State of the resource. indicating what to do with the resource
	enum State
	{
		/// The best state: the file is the same in remote and in local.
		sync,
		
		/// Resource created in local, but remote does not have it.
		/// We should create the resource in google drive and upload new content
		local_new,
		
		/// Resource exists in both local & remote, but changes in local is newer
		/// than remote. We should upload local copy to overwrite remote.
		local_changed,
		
		/// Resource deleted from local since last time grive has checked.
		local_deleted,
		
		/// Resource created in google drive, but not exist in local.
		/// We should download the file.
		remote_new,
		
		/// Resource exists in both local & remote, but remote is newer.		
		remote_changed,
		
		/// Resource delete in remote, need to delete in local
		remote_deleted,
		
		
		/// invalid value
		unknown
	} ;

public :
	Resource(const fs::path& root_folder) ;
	Resource( const std::string& name, const std::string& kind ) ;
	
	bool IsFolder() const ;
	bool IsEditable() const ;

	std::string Name() const ;
	std::string Kind() const ;
	DateTime MTime() const ;
	std::string SelfHref() const ;
	std::string ContentSrc() const ;
	std::string ETag() const ;
	std::string ResourceID() const ;
	State GetState() const;
	
	const Resource* Parent() const ;
	Resource* Parent() ;
	void AddChild( Resource *child ) ;
	Resource* FindChild( const std::string& title ) ;
	
	fs::path Path() const ;
	bool IsInRootTree() const ;
	bool IsRoot() const ;
	bool HasID() const ;
	std::string MD5() const ;

	void FromRemote( const Entry& remote, const DateTime& last_sync ) ;
	void FromLocal( const DateTime& last_sync ) ;
	
	void Sync( Syncer* syncer, DateTime& sync_time, const Val& options ) ;

	// children access
	iterator begin() const ;
	iterator end() const ;
	std::size_t size() const ;
	
	std::string StateStr() const ;

private :

	void AssignIDs( const Entry& remote ) ;

	friend std::ostream& operator<<( std::ostream& os, State s ) ;
	friend class Syncer ;

private :
	void SetState( State new_state ) ;
	
	void FromRemoteFolder( const Entry& remote, const DateTime& last_sync ) ;
	void FromRemoteFile( const Entry& remote, const DateTime& last_sync ) ;
	
	void DeleteLocal() ;
	
	void SyncSelf( Syncer* syncer, const Val& options ) ;

private :
	std::string				m_name ;
	std::string				m_kind ;
	std::string				m_md5 ;
	DateTime				m_mtime ;

	std::string				m_id ;
	std::string				m_href ;
	std::string				m_content ;
	std::string				m_etag ;
	bool					m_is_editable ;

	// not owned
	Resource				*m_parent ;
	std::vector<Resource*>	m_child ;

	State					m_state ;
} ;

} // end of namespace gr::v1
