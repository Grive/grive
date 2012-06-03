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
#include <memory>

namespace gr {

namespace log
{
	/// defines the types of the log message
	enum Serverity
	{
		/// user unfriendly messages. only meant for developers.
		debug,
		
		/// enabled only if -V is specified. grive tries to tell you every
		/// single thing it tries to do.
		verbose,
		
		/// notification messages indicates nothing is going wrong
		info,
		
		/// potential error messages
		warning,
		
		/// an error has occurs but grive doesn't need to quit
		error,
		
		/// grive cannot proceed
		critical,
		
		
		/// must be put at the end, equal to number of serverities
		serverity_count
	} ;
	
	typedef boost::format Fmt ;
}

/*!	\brief	Base class and singleton of log facilities
*/
class LogBase
{
public :
	virtual void Log( const log::Fmt& msg, log::Serverity s = log::info ) = 0 ;
	virtual bool Enable( log::Serverity s, bool enable = true ) = 0 ;
	virtual bool IsEnabled( log::Serverity s ) const = 0 ;
	
	static LogBase* Inst( std::auto_ptr<LogBase> log = std::auto_ptr<LogBase>() ) ;
	~LogBase() ;

protected :
	LogBase() ;
} ;

class DisableLog
{
public :
	DisableLog( log::Serverity s ) ;
	~DisableLog() ;

private :
	log::Serverity	m_sev ;
	bool			m_prev ;
} ;

void Log( const std::string& str, log::Serverity s = log::info ) ;

template <typename P1>
void Log( const std::string& fmt, const P1& p1, log::Serverity s = log::info )
{
	LogBase::Inst()->Log( log::Fmt(fmt) % p1, s ) ;
}

template <typename P1, typename P2>
void Log( const std::string& fmt, const P1& p1, const P2& p2, log::Serverity s = log::info )
{
	LogBase::Inst()->Log( log::Fmt(fmt) % p1 % p2, s ) ;
}

template <typename P1, typename P2, typename P3>
void Log( const std::string& fmt, const P1& p1, const P2& p2, const P3& p3, log::Serverity s = log::info )
{
	LogBase::Inst()->Log( log::Fmt(fmt) % p1 % p2 % p3, s ) ;
}

template <typename P1, typename P2, typename P3, typename P4>
void Log(
	const std::string& fmt,
	const P1& p1,
	const P2& p2,
	const P3& p3,
	const P4& p4,
	log::Serverity s = log::info )
{
	LogBase::Inst()->Log( log::Fmt(fmt) % p1 % p2 % p3 % p4, s ) ;
}

void Trace( const std::string& str ) ;

template <typename P1>
void Trace( const std::string& fmt, const P1& p1 )
{
	LogBase::Inst()->Log( log::Fmt(fmt) % p1, log::debug ) ;
}

template <typename P1, typename P2>
void Trace( const std::string& fmt, const P1& p1, const P2& p2 )
{
	LogBase::Inst()->Log( log::Fmt(fmt) % p1 % p2, log::debug ) ;
}

template <typename P1, typename P2, typename P3>
void Trace( const std::string& fmt, const P1& p1, const P2& p2, const P3& p3 )
{
	LogBase::Inst()->Log( log::Fmt(fmt) % p1 % p2 % p3, log::debug ) ;
}

} // end of namespace
