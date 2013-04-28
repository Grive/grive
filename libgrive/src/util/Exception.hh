/*
	webwrite: an GPL program to sync a local directory with Google Drive
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

#include <boost/exception/exception.hpp>
#include <boost/exception/info.hpp>

#include <exception>
#include <string>

namespace PROJ_NS {

class Backtrace ;

/**	\defgroup	exception	Exception Classes
*/

/**	\brief	base class for exception in WebWrite
	\ingroup exception
	This class is the base class for all exception class in WebWrite.
	It allows us to catch all WebWrite exception with one catch clause.
*/
struct Exception :
	virtual public std::exception,
	virtual public boost::exception
{
	Exception( ) ;
	
	virtual const char* what() const throw() ;
} ;

/// Exception informations
namespace expt
{
	// back-trace information. should be present for all exceptions
	typedef boost::error_info<struct BacktraceTag, Backtrace>	Backtrace_ ;

	/// generic error message
	typedef boost::error_info<struct MsgTag, std::string>			ErrMsg ;
}

} // end of namespace

