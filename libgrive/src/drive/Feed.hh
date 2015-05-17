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

#include "base/Entry.hh"

#include "xml/Node.hh"
#include "xml/NodeSet.hh"

#include <vector>

#include <string>

namespace gr {

namespace http
{
	class Agent ;
}

namespace v1 {

class Feed
{
public :
	typedef std::vector<Entry> Entries;
	typedef std::vector<Entry>::const_iterator iterator;

public :
	Feed( ) ;
	void Start( http::Agent *http, const std::string& url ) ;
	bool GetNext( http::Agent *http ) ;
	iterator begin() const ;
	iterator end() const ;
	
	void EnableLog( const std::string& prefix, const std::string& suffix ) ;
private :
	struct LogInfo
	{
		std::string	prefix ;
		std::string	suffix ;
		std::size_t	sequence ;
	} ;
	std::auto_ptr<LogInfo>	m_log ;

	xml::Node		m_root ;
	Entries			m_entries ;
} ;

} } // end of namespace gr::v1
