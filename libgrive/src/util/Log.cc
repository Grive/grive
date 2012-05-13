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

class DefaultLog : public Log
{
public :
	// do nothing
	void operator()( const Fmt& msg, Serverity s )
	{
		operator()( msg.str().c_str(), s ) ;
	}
	
	void operator()( const char *str, Serverity s )
	{
		switch ( s )
		{
			case debug:
			case info:
				std::cout << str << std::endl ;
				break ;
			
			default:
				std::cerr << str << std::endl ;
				break ;
		}
	}
} ;

Log& Log::Inst( Log *log )
{
	static DefaultLog mlog ;
	static Log *inst = (log == 0 ? &mlog : log ) ;
	assert( inst != 0 ) ;
	return *inst ;
}

Log::Log()
{
}

Log::~Log()
{
}

void Logs( const std::string& str, Log::Serverity s )
{
	Log::Inst()( str.c_str(), s ) ;
}

} // end of namespace
