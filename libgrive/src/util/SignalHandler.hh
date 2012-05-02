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

#include <stdexcept>
#include <string>
#include <map>

namespace gr {

class SignalError : public std::runtime_error
{
public :
	SignalError( const std::string& message ) ;
	virtual ~SignalError() throw () ;
};

class SignalFunctor
{
public :
	SignalFunctor() ;
	virtual ~SignalFunctor() ;
	static void Callback( int signumber ) ;
};

class SignalHandler
{
	typedef void (*Callback)(int);
	typedef std::map<unsigned int, Callback> signals_t ;

public :
	virtual ~SignalHandler() ;
	void RegisterSignal ( unsigned int signumber, Callback callback ) ;
	void UnregisterSignal( unsigned int signumber );
	static SignalHandler& GetInstance() ;
private :
	SignalHandler() ;
	SignalHandler( const SignalHandler& right ) ;
	SignalHandler& operator==( const SignalHandler& right ) ;

	signals_t m_signals;
	signals_t m_signalsOld;
};

}
