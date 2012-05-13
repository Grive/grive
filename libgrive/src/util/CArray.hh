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

#include <cstddef>

namespace gr {

/*!	\brief	get the begin iterator from an array
	\internal
	
	This function returns the begin "iterator" of an array. It is useful to
	treat an array like an STL container.
	
	For example:
\code
int array[10] = { 1, 2, 3, 4, 5 } ;
std::vector<int> v ;
std::copy( Begin(array), End(array), std::back_inserter( v ) ;
\endcode

	\param	array	reference to the array
	\return	the begin iterator of the array. i.e. \a array itself
	\sa End(), Count()
*/
template <typename T, std::size_t n>
T* Begin( T (&array)[n] )
{
	return array ;
}

/*!	\brief	get the end iterator from an array
	\internal
	
	This function returns the end "iterator" of an array. It is useful to
	treat an array like an STL container.
	
	\param	array	reference to the array
	\return	the end iterator of the array. i.e. \a array+n
	\sa Begin(), Count()
*/
template <typename T, std::size_t n>
T* End( T (&array)[n] )
{
	return array + n ;
}

/*!	\brief	get the number of elements in the array
	\internal

	This function will return the number of elements in the array.
	\return	the number of elements in the array
	\sa Begin(), End()
*/
template <typename T, std::size_t n>
std::size_t Count( T (&array)[n] )
{
	return n ;
}

} // end of namespace
