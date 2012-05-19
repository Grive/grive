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

#include "util/Log.hh"
#include "util/OS.hh"
#include "xml/Node.hh"
#include "xml/NodeSet.hh"

#include <algorithm>
#include <iterator>

namespace gr {

Entry::Entry( const Path& file )
{
}

Entry::Entry( const xml::Node& n )
{
	Update( n ) ;
}

Entry::Entry( const std::string& title, const std::string& kind, const std::string& href ) :
	m_title		( title ),
	m_filename	( title ),
	m_kind		( kind ),
	m_self_href	( href )
{
}

void Entry::Update( const xml::Node& n )
{
	m_title				= n["title"] ;
	m_etag				= n["@gd:etag"] ;
	m_filename			= n["docs:suggestedFilename"] ;
	m_content_src		= n["content"]["@src"] ;
	m_self_href			= n["link"].Find( "@rel", "self" )["@href"] ;
	m_server_modified	= DateTime( n["updated"] ) ;	

	m_resource_id		= n["gd:resourceId"] ;
	m_server_md5		= n["docs:md5Checksum"] ;
	m_kind				= n["category"].Find( "@scheme", "http://schemas.google.com/g/2005#kind" )["@label"] ;
	m_upload_link		= n["link"].Find( "@rel", "http://schemas.google.com/g/2005#resumable-edit-media")["@href"] ;

	m_parent_hrefs.clear( ) ;
	xml::NodeSet parents = n["link"].Find( "@rel", "http://schemas.google.com/docs/2007#parent" ) ;
	for ( xml::NodeSet::iterator i = parents.begin() ; i != parents.end() ; ++i )
		m_parent_hrefs.push_back( (*i)["@href"] ) ;
	
	if ( !m_parent_hrefs.empty() )
		m_parent_href = m_parent_hrefs.front() ;

	// convert to lower case for easy comparison
	std::transform(
		m_server_md5.begin(),
		m_server_md5.end(),
		m_server_md5.begin(),
		tolower ) ;
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

std::string Entry::ServerMD5() const
{
	return m_server_md5 ;
}

DateTime Entry::ServerModified() const
{
	return m_server_modified ;
}

std::string Entry::SelfHref() const
{
	return m_self_href ;
}

std::string Entry::ParentHref() const
{
	return m_parent_href ;
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

std::string Entry::UploadLink() const
{
	return m_upload_link ;
}

void Entry::Swap( Entry& e )
{
	m_title.swap( e.m_title ) ;
	m_filename.swap( e.m_filename ) ;
	m_kind.swap( e.m_kind ) ;
	m_server_md5.swap( e.m_server_md5 ) ;
	m_etag.swap( e.m_etag ) ;
	m_resource_id.swap( e.m_resource_id ) ;

	m_self_href.swap( e.m_self_href ) ;
	m_parent_href.swap( e.m_parent_href ) ;
	m_content_src.swap( e.m_content_src ) ;	
	m_upload_link.swap( e.m_upload_link ) ;

	m_server_modified.Swap( e.m_server_modified ) ;
}

} // end of namespace
