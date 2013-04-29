/*
	grive: an GPL program to sync a local directory with Google Drive
	Copyright (C) 2013 Wan Wai Ho

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
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
	MA  02110-1301, USA.
*/

#pragma once

#include <string>
#include <vector>

namespace gr { namespace v2 {

class Resource
{
public :
	Resource( const std::string& id, const std::string& mime, const std::string& title ) ;
	
	std::string ID() const ;
	std::string	Mime() const ;
	std::string Title() const ;
	
	bool IsFolder() const ;
	
	void Add( const Resource& child ) ;
	
private :
	std::string		m_id ;
	std::string		m_mime ;
	std::string		m_title ;
	
	std::vector<Resource>	m_children ;
} ;

} } // end of namespace gr::v2
