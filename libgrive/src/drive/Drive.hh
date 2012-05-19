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

#include "Resource.hh"
#include "State.hh"

#include "protocol/Json.hh"
#include "util/Exception.hh"

#include <string>
#include <vector>

namespace gr {

namespace http
{
	class Agent ;
}
namespace xml
{
	class Node ;
}

class OAuth2 ;
class File ;

class Drive
{
public :
	typedef std::vector<Resource>			FolderList ;
	typedef std::vector<Resource>::iterator	FolderListIterator ;

public :
	Drive( OAuth2& auth ) ;
	~Drive( ) ;

	void Update() ;
	void Sync() ;
	
	struct Error : virtual Exception {} ;
	
private :
	void UpdateFile( Entry& file, Resource *parent, http::Agent *http ) ;
	
	void ConstructDirTree( http::Agent *http ) ;
	
private :
	OAuth2&						m_auth ;
	std::vector<std::string>	m_http_hdr ;

	std::string					m_resume_link ;

	std::vector<Resource*>		m_files ;
	State						m_state ;
} ;

} // end of namespace
