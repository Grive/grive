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

#include "Log.hh"

#include <cassert>

namespace gr {

class MockLog : public LogBase
{
public :
	void Log( const log::Fmt&, log::Serverity )
	{
	}
	
	bool Enable( log::Serverity, bool enable )
	{
		return enable ;
	}
	bool IsEnabled( log::Serverity ) const
	{
		return true ;
	}
} ;

LogBase* LogBase::Inst( std::auto_ptr<LogBase> log )
{
	static std::auto_ptr<LogBase> inst( new MockLog ) ;
	
	if ( log.get() != 0 )
		inst = log ;
		
	assert( inst.get() != 0 ) ;
	return inst.get() ;
}

LogBase::LogBase()
{
}

LogBase::~LogBase()
{
}

void Log( const std::string& str, log::Serverity s )
{
	LogBase::Inst()->Log( log::Fmt(str), s ) ;
}

void Trace( const std::string& str )
{
	LogBase::Inst()->Log( log::Fmt(str), log::debug ) ;
}

DisableLog::DisableLog( log::Serverity s ) :
	m_sev( s ),
	m_prev( LogBase::Inst()->Enable( s, false ) )
{
}

DisableLog::~DisableLog()
{
	LogBase::Inst()->Enable( m_sev, m_prev ) ;
}

} // end of namespace
