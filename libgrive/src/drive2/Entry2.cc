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

#include "Entry2.hh"
#include "CommonUri.hh"

#include "util/Crypt.hh"
#include "util/log/Log.hh"
#include "util/OS.hh"

#include "json/Val.hh"

#include <algorithm>
#include <iterator>

namespace gr { namespace v2 {

/// construct an entry for remote, from "file" JSON object - Drive REST API
Entry2::Entry2( const Val& item )
{
	Update( item ) ;
}

void Entry2::Update( const Val& item )
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

} } // end of namespace gr::v2
