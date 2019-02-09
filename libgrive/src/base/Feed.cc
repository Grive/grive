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

#include "Feed.hh"

#include "Entry.hh"

#include "http/Agent.hh"

namespace gr {

Feed::Feed( const std::string &url ):
	m_next( url )
{
}

Feed::~Feed()
{
}

Feed::iterator Feed::begin() const
{
	return m_entries.begin() ;
}

Feed::iterator Feed::end() const
{
	return m_entries.end() ;
}

} // end of namespace gr::v1
