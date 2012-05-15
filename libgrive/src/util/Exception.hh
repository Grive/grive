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

#include <boost/exception/exception.hpp>
#include <boost/exception/info.hpp>

#include <exception>
#include <string>

namespace gr {

class Backtrace ;

/**	\defgroup	exception	Exception Classes
*/

///	base class for exception in libpdfdoc
/**	\ingroup exception
	This class is the base class for all exception class in libpdfdoc.
*/
struct Exception :
	virtual public std::exception,
	virtual public boost::exception
{
	Exception( ) ;
} ;

struct FileError	: virtual Exception {} ;

///	Parse error exception.
/**	\ingroup exception
	This exception will be thrown when there is a parse error when reading
	a PDF file.
*/
struct ParseError	: virtual Exception {} ;

///	Invalid type exception.
/**	\ingroup exception
	This exception will be thrown when the Object cannot convert its
	underlying data to a specific type. The what() member function will
	describe the expected and actual type of the data.
*/
struct BadType 		: virtual Exception {} ;

struct Unsupported	: virtual Exception {} ;

// Exception informations
namespace expt
{
	// back-trace information. should be present for all exceptions
	typedef boost::error_info<struct BacktraceTag, Backtrace>	BacktraceInfo ;

	// generic error message
	typedef boost::error_info<struct MsgTag, std::string>		ErrMsg ;
	
	// nested exception
	typedef boost::error_info<struct ExceptionTag, Exception>	Nested ;
}

} // end of namespace

