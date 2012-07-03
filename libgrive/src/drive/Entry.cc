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
#include "CommonUri.hh"

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
	m_kind			( "folder" ),
	m_resource_id	( "folder:root" ),
	m_self_href		( root_href ),
	m_create_link	( root_create ),
	m_change_stamp	( -1 ),
	m_is_removed	( false )
{
}

/// construct an entry for remote
Entry::Entry( const xml::Node& n ) :
	m_change_stamp( -1 ),
	m_is_removed( false )
{
	Update( n ) ;
}

void Entry::Update( const xml::Node& n )
{
	m_title			= n["title"] ;
	m_etag			= n["@gd:etag"] ;
	m_filename		= n["docs:suggestedFilename"] ;
	m_content_src	= n["content"]["@src"] ;
	m_self_href		= n["link"].Find( "@rel", "self" )["@href"] ;
	m_alt_self		= n["link"].Find( "@rel", "http://schemas.google.com/docs/2007#alt-self" )["@href"] ;
	m_mtime			= DateTime( n["updated"] ) ;

	m_resource_id	= n["gd:resourceId"] ;
	m_md5			= n["docs:md5Checksum"] ;
	m_kind			= n["category"].Find( "@scheme", "http://schemas.google.com/g/2005#kind" )["@label"] ;
	m_edit_link		= n["link"].Find( "@rel", "http://schemas.google.com/g/2005#resumable-edit-media")["@href"] ;
	m_create_link	= n["link"].Find( "@rel", "http://schemas.google.com/g/2005#resumable-create-media")["@href"] ;

	// changestamp only appear in change feed entries
	xml::NodeSet cs	= n["docs:changestamp"]["@value"] ;
	m_change_stamp	= cs.empty() ? -1 : std::atoi( cs.front().Value().c_str() ) ;

	m_parent_hrefs.clear( ) ;
	xml::NodeSet parents = n["link"].Find( "@rel", "http://schemas.google.com/docs/2007#parent" ) ;
	for ( xml::NodeSet::iterator i = parents.begin() ; i != parents.end() ; ++i )
		m_parent_hrefs.push_back( (*i)["@href"] ) ;

	// convert to lower case for easy comparison
	std::transform( m_md5.begin(), m_md5.end(), m_md5.begin(), tolower ) ;

	m_is_removed = !n["gd:deleted"].empty() || !n["docs:removed"].empty() ;
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

std::string Entry::Kind() const
{
	return m_kind ;
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

std::string Entry::AltSelf() const
{
	return m_alt_self ;
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

std::string Entry::EditLink() const
{
	return m_edit_link ;
}

std::string Entry::CreateLink() const
{
	return m_create_link ;
}

void Entry::Swap( Entry& e )
{
	m_title.swap( e.m_title ) ;
	m_filename.swap( e.m_filename ) ;
	m_kind.swap( e.m_kind ) ;
	m_md5.swap( e.m_md5 ) ;
	m_etag.swap( e.m_etag ) ;
	m_resource_id.swap( e.m_resource_id ) ;

	m_parent_hrefs.swap( e.m_parent_hrefs ) ;

	m_self_href.swap( e.m_self_href ) ;
	m_alt_self.swap( e.m_alt_self ) ;
	m_content_src.swap( e.m_content_src ) ;
	m_edit_link.swap( e.m_edit_link ) ;
	m_create_link.swap( e.m_create_link ) ;

	m_mtime.Swap( e.m_mtime ) ;

	std::swap( m_change_stamp, e.m_change_stamp ) ;
	std::swap( m_is_removed, e.m_is_removed ) ;
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
	return m_kind == "file" ? m_filename : m_title ;
}

} // end of namespace
