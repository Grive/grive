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
#include <iostream>

namespace gr {

class DefaultLog : public LogBase
{
public :
	void Log( const log::Fmt& msg, log::Serverity s )
	{
		switch ( s )
		{
			case log::debug:
			case log::info:
				std::cout << msg << std::endl ;
				break ;
			
			default:
				std::cerr << msg << std::endl ;
				break ;
		}
	}
} ;

LogBase* LogBase::Inst( LogBase *log )
{
	static DefaultLog mlog ;
	static LogBase *inst = (log == 0 ? &mlog : log ) ;
	assert( inst != 0 ) ;
	return inst ;
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

} // end of namespace
