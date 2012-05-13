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

#include <boost/format.hpp>

namespace gr {

typedef boost::format Fmt ;

/*!	\brief	Base class and singleton of log facilities
*/
class Log
{
public :
	enum Serverity { debug, info, warning, error, critical } ;

public :
	virtual void operator()( const Fmt& msg, Serverity s = debug ) = 0 ;
	virtual void operator()( const char *str, Serverity s = debug ) = 0 ;
	
	static Log& Inst( Log *log = 0 ) ;

protected :
	Log() ;
	~Log() ;
} ;

void Logs( const std::string& str, Log::Serverity s = Log::debug ) ;

template <typename P1>
void Logs( const std::string& fmt, const P1& p1, Log::Serverity s = Log::debug )
{
	Log::Inst()( Fmt(fmt) % p1, s ) ;
}

template <typename P1, typename P2>
void Logs( const std::string& fmt, const P1& p1, const P2& p2, Log::Serverity s = Log::debug )
{
	Log::Inst()( Fmt(fmt) % p1 % p2, s ) ;
}

template <typename P1, typename P2, typename P3>
void Logs( const std::string& fmt, const P1& p1, const P2& p2, const P3& p3, Log::Serverity s = Log::debug )
{
	Log::Inst()( Fmt(fmt) % p1 % p2 % p3, s ) ;
}

} // end of namespace
