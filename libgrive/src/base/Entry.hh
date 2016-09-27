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

#include "util/Types.hh"
#include "util/DateTime.hh"
#include "util/FileSystem.hh"

#include <iosfwd>
#include <string>
#include <vector>

namespace gr {

/*!	\brief	corresponds to an "entry" in the resource feed

	This class is decodes an entry in the resource feed. It will stored the properties like
	title, filename and ETag etc in member variables. Normally entries are created by
	parsing the resource feed
*/
class Entry
{
public :
	Entry( ) ;
	
	std::string Title() const ;
	std::string Filename() const ;
	bool IsDir() const ;
	std::string MD5() const ;
	DateTime MTime() const ;
	u64_t Size() const ;
	
	std::string Name() const ;
	
	std::string ResourceID() const ;
	std::string ETag() const ;
	
	std::string SelfHref() const ;
	std::string ParentHref() const ;
	std::string ContentSrc() const ;
	bool IsEditable() const ;
	long ChangeStamp() const ;
	
	bool IsChange() const ;
	bool IsRemoved() const ;
	
	const std::vector<std::string>& ParentHrefs() const ;
	
protected :
	std::string		m_title ;
	std::string		m_filename ;
	bool			m_is_dir ;
	std::string		m_md5 ;
	std::string		m_etag ;
	std::string		m_resource_id ;

	std::vector<std::string>	m_parent_hrefs ;

	std::string		m_self_href ;
	std::string		m_content_src ;

	bool			m_is_editable ;

	long			m_change_stamp ;

	DateTime		m_mtime ;
	bool			m_is_removed ;
	u64_t			m_size ;
} ;

} // end of namespace gr
