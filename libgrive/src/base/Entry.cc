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

#include "Entry.hh"

#include "util/Crypt.hh"
#include "util/log/Log.hh"
#include "util/OS.hh"
#include "xml/Node.hh"
#include "xml/NodeSet.hh"

#include <algorithm>
#include <iterator>

namespace gr {

/// construct an entry for the root folder
Entry::Entry( ) :
	m_title			( "." ),
	m_is_dir		( true ),
	m_resource_id	( "folder:root" ),
	m_change_stamp	( -1 ),
	m_is_removed	( false )
{
}

const std::vector<std::string>& Entry::ParentHrefs() const
{
	return m_parent_hrefs ;
}

std::string Entry::Title() const
{
	return m_title ;
}

std::string Entry::Filename() const
{
	return m_filename ;
}

bool Entry::IsDir() const
{
	return m_is_dir ;
}

std::string Entry::MD5() const
{
	return m_md5 ;
}

DateTime Entry::MTime() const
{
	return m_mtime ;
}

std::string Entry::SelfHref() const
{
	return m_self_href ;
}

std::string Entry::ParentHref() const
{
	return m_parent_hrefs.empty() ? "" : m_parent_hrefs.front() ;
}

std::string Entry::ResourceID() const
{
	return m_resource_id ;
}

std::string Entry::ETag() const
{
	return m_etag ;
}

std::string Entry::ContentSrc() const
{
	return m_content_src ;
}

bool Entry::IsEditable() const
{
	return m_is_editable ;
}

long Entry::ChangeStamp() const
{
	return m_change_stamp ;
}

bool Entry::IsChange() const
{
	return m_change_stamp != -1 ;
}

bool Entry::IsRemoved() const
{
	return m_is_removed ;
}

std::string Entry::Name() const
{
	return !m_filename.empty() ? m_filename : m_title ;
}

} // end of namespace gr
