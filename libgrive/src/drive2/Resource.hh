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
	Resource() ;
	Resource(
		const std::string& id,
		const std::string& mime,
		const std::string& title,
		const std::string& parent ) ;

	std::string ID() const ;
	std::string	Mime() const ;
	std::string Title() const ;

	bool IsFolder() const ;

	void AddChild( const std::string& child ) ;
	void SetParent( const std::string& parent ) ;

	std::size_t ChildCount() const ;
	std::string At( std::size_t idx ) const ;
	std::string Parent() const ;
	std::size_t Index( const std::string& child ) const ;

private :
	std::string		m_id ;
	std::string		m_mime ;
	std::string		m_title ;

	std::vector<std::string>	m_children ;

	std::string		m_parent ;
} ;

} } // end of namespace gr::v2
