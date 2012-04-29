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

#include "Collection.hh"

#include <string>
#include <vector>

namespace gr {

class OAuth2 ;
class Json ;

class Drive
{
public :
	typedef std::vector<Collection>				FolderList ;
	typedef std::vector<Collection>::iterator	FolderListIterator ;

public :
	Drive( OAuth2& auth ) ;
	~Drive( ) ;

private :
	void UpdateFile( const Json& entry ) ;
	void UploadFile( const Json& entry ) ;
	std::string Parent( const Json& entry ) ;
	
	void ConstructDirTree( const std::vector<Json>& entries ) ;
	
	static std::string Kind( const Json& entry ) ;
	
	FolderListIterator FindFolder( const std::string& href ) ;
	
private :
	OAuth2&						m_auth ;
	std::vector<std::string>	m_http_hdr ;

	std::string					m_resume_link ;
	
	FolderList					m_coll ;
	Collection					m_root ;
} ;

} // end of namespace
