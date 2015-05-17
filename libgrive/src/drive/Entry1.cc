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

#include "Entry1.hh"
#include "CommonUri.hh"

#include "util/Crypt.hh"
#include "util/log/Log.hh"
#include "util/OS.hh"
#include "xml/Node.hh"
#include "xml/NodeSet.hh"

#include <algorithm>
#include <iterator>

namespace gr { namespace v1 {

Entry1::Entry1():
	Entry ()
{
	m_self_href = root_href;
}

/// construct an entry for remote - Doclist API v3
Entry1::Entry1( const xml::Node& n )
{
	Update( n ) ;
}

void Entry1::Update( const xml::Node& n )
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
	{
		std::string href = (*i)["@href"];
		if ( href == root_href )
			href = "root"; // API-independent root href
		m_parent_hrefs.push_back( href ) ;
	}

	// convert to lower case for easy comparison
	std::transform( m_md5.begin(), m_md5.end(), m_md5.begin(), tolower ) ;

	m_is_removed = !n["gd:deleted"].empty() || !n["docs:removed"].empty() ;
}

} } // end of namespace gr::v1
