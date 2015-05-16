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

#include "json/Val.hh"
#include "drive2/CommonUri.hh"

#include <algorithm>
#include <iterator>

namespace gr { namespace v1 {

/// construct an entry for the root folder
Entry::Entry( ) :
	m_title			( "." ),
	m_is_dir		( true ),
	m_resource_id	( "folder:root" ),
	m_self_href		( root_href ),
	m_change_stamp	( -1 ),
	m_is_removed	( false )
{
}

/// construct an entry for remote - Doclist API v3
Entry::Entry( const xml::Node& n ) :
	m_change_stamp( -1 ),
	m_is_removed( false )
{
	Update( n ) ;
}

/// construct an entry for remote, from "file" JSON object - Drive REST API
Entry::Entry( const Val& item ) :
	m_change_stamp( -1 ),
	m_is_removed( false )
{
	Update( item ) ;
}

void Entry::Update( const xml::Node& n )
{
	m_title			= n["title"] ;
	m_etag			= n["@gd:etag"] ;
	m_filename		= n["docs:suggestedFilename"] ;
	m_content_src	= n["content"]["@src"] ;
	m_self_href		= n["link"].Find( "@rel", "self" )["@href"] ;
	m_mtime			= DateTime( n["updated"] ) ;

	m_resource_id	= n["gd:resourceId"] ;
	m_md5			= n["docs:md5Checksum"] ;
	m_is_dir		= n["category"].Find( "@scheme", "http://schemas.google.com/g/2005#kind" )["@label"] == "folder" ;
	m_is_editable	= !n["link"].Find( "@rel", m_is_dir
		? "http://schemas.google.com/g/2005#resumable-create-media" : "http://schemas.google.com/g/2005#resumable-edit-media" )
		["@href"].empty() ;

	// changestamp only appear in change feed entries
	xml::NodeSet cs	= n["docs:changestamp"]["@value"] ;
	m_change_stamp	= cs.empty() ? -1 : std::atoi( cs.front().Value().c_str() ) ;
	if ( m_change_stamp != -1 )
	{
		m_self_href	= n["link"].Find( "@rel", "http://schemas.google.com/docs/2007#alt-self" )["@href"] ;
	}

	m_parent_hrefs.clear( ) ;
	xml::NodeSet parents = n["link"].Find( "@rel", "http://schemas.google.com/docs/2007#parent" ) ;
	for ( xml::NodeSet::iterator i = parents.begin() ; i != parents.end() ; ++i )
		m_parent_hrefs.push_back( (*i)["@href"] ) ;

	// convert to lower case for easy comparison
	std::transform( m_md5.begin(), m_md5.end(), m_md5.begin(), tolower ) ;

	m_is_removed = !n["gd:deleted"].empty() || !n["docs:removed"].empty() ;
}

void Entry::Update( const Val& item )
{
	bool is_chg = item["kind"].Str() == "drive#change";

	// changestamp only appears in change feed entries
	m_change_stamp	= is_chg ? item["id"].Int() : -1 ;
	m_is_removed	= is_chg && item["deleted"].Bool() ;

	const Val& file = is_chg && !m_is_removed ? item["file"] : item;

	if ( m_is_removed )
	{
		m_resource_id = item["fileId"];
	}
	else
	{
		m_title			= file["title"] ;
		m_etag			= file["etag"] ;
		m_filename		= file["originalFilename"] ;
		m_content_src	= file["downloadUrl"] ;
		m_self_href		= file["selfLink"] ;
		m_mtime			= DateTime( file["modificationDate"] ) ;

		m_resource_id	= file["id"]; // file#id ?
		m_md5			= file["md5Checksum"] ;
		m_is_dir		= file["mimeType"].Str() == v2::mime_types::folder ;
		m_is_editable	= file["editable"].Bool() ;

		m_parent_hrefs.clear( ) ;

		Val::Array parents = file["parents"].AsArray() ;
		for ( Val::Array::iterator i = parents.begin() ; i != parents.end() ; ++i )
			m_parent_hrefs.push_back( (*i)["parentLink"] ) ; // maybe .id?

		// convert to lower case for easy comparison
		std::transform( m_md5.begin(), m_md5.end(), m_md5.begin(), tolower ) ;
	}
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

} } // end of namespace gr::v1
