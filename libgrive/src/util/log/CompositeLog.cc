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

#include "CompositeLog.hh"

#include <algorithm>
#include "util/Destroy.hh"

namespace gr { namespace log {

CompositeLog::CompositeLog()
{
	Enable(log::debug,		true) ;
	Enable(log::verbose,	true) ;
	Enable(log::info,		true) ;
	Enable(log::warning,	true) ;
	Enable(log::error,		true) ;
	Enable(log::critical,	true) ;
}

CompositeLog::~CompositeLog()
{
	std::for_each( m_logs.begin(), m_logs.end(), Destroy() ) ;
}
	
LogBase* CompositeLog::Add( std::auto_ptr<LogBase> log )
{
	m_logs.push_back( log.get() ) ;
	return log.release() ;
}

void CompositeLog::Log( const log::Fmt& msg, log::Serverity s )
{
	for ( std::vector<LogBase*>::iterator i = m_logs.begin(); i != m_logs.end(); ++i )
	{
		if ( IsEnabled(s) && (*i)->IsEnabled(s) )
			(*i)->Log( msg, s ) ;
	}
}

} } // end of namespace
