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


#include "SignalHandler.hh"
#include <string>
#include <stdexcept>
#include <signal.h>
#include <iostream>
#include <sstream>

namespace gr {

SignalError::SignalError( const std::string& message ) :
		std::runtime_error( message )
{
}

SignalError::~SignalError() throw ()
{

}

SignalHandler::SignalHandler()
{

}

SignalHandler::SignalHandler( const SignalHandler& right )
{

}

SignalHandler& SignalHandler::operator ==( const SignalHandler& right )
{
	return (*this);
}

SignalHandler::~SignalHandler()
{

}

SignalHandler& SignalHandler::GetInstance()
{
	static SignalHandler _instance;
	return _instance;
}

void SignalHandler::UnregisterSignal( unsigned int signumber )
{
	m_signals[signumber] = 0 ;

	// Restore the old signal
	signal( ( int ) signumber, m_signalsOld[signumber] );
}

void SignalHandler::RegisterSignal( unsigned int signumber, Callback callback )
{
	signals_t::const_iterator anIterator ;
	for (anIterator = m_signals.begin(); anIterator != m_signals.end(); ++anIterator)
	{
		if (anIterator->first == signumber)
		{
			if (anIterator->second != 0)
			{
				std::ostringstream oss;
				oss << "Signal " << signumber << " already has a callback!";
				throw SignalError( oss.str() ); ;
			}
		}
	}

	m_signals[signumber] = callback ;

	if ( ( m_signalsOld[signumber] = signal( ( int ) signumber,  m_signals[signumber] ) ) == SIG_ERR ) {
		throw SignalError( " Error while registering the signal! " ) ;
	}
}

}



