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

#include "CommonLog.hh"

namespace gr { namespace log {

CommonLog::CommonLog()
{
	m_enabled[log::debug]		= false ;
	m_enabled[log::verbose]		= false ;
	m_enabled[log::info]		= true ;
	m_enabled[log::warning]		= true ;
	m_enabled[log::error]		= true ;
	m_enabled[log::critical]	= true ;
}

bool CommonLog::Enable( log::Serverity s, bool enable )
{
	assert( s >= debug && s < serverity_count ) ;
	
	bool prev = m_enabled[s] ;
	m_enabled[s] = enable ;
	
	return prev ;
}

bool CommonLog::IsEnabled( log::Serverity s ) const
{
	assert( s >= debug && s < serverity_count ) ;
	return m_enabled[s] ;
}

}} // end of namespace
