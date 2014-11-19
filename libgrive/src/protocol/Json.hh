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

#include "util/Exception.hh"

#include <string>
#include <map>
#include <vector>

struct json_object ;

namespace gr {

class DataStream ;

/*!	\brief	Simple wrapper around JSON-C objects.

	This class represents JSON-C objects, which can be integers, booleans, strings
	double, arrays and object.
*/
class Json
{
public :
	typedef std::map<std::string, Json>	Object ;
	typedef std::vector<Json>			Array ;

	struct Error : virtual Exception {} ;
	typedef boost::error_info<struct JsonTag, 			std::string>	Json_ ;
	typedef boost::error_info<struct OutOfRange,		std::size_t>	OutOfRange_ ;
	typedef boost::error_info<struct KeyNotFound,		std::string>	KeyNotFound_ ;
	typedef boost::error_info<struct JsonCApi,			std::string>	JsonCApi_ ;
	typedef boost::error_info<struct Value,				std::string>	Value_ ;
	typedef boost::error_info<struct ErrMsg,			std::string>	ErrMsg_ ;

	template <typename T>
	struct Val_
	{
		typedef boost::error_info<struct Value, T> Err ;
	} ;

public :
	template <typename T>
	explicit Json( const T& val ) ;

	template <std::size_t n>
	explicit Json( const char (&str)[n] ) :
		m_json( InitStr( str, n ) )
	{
	}

	Json() ;
	Json( const Json& rhs ) ;
	Json( const char *str ) ;
	~Json() ;

	static Json Parse( const std::string& str ) ;
	static Json Parse( DataStream *in ) ;

	Json operator[]( const std::string& key ) const ;
	Json operator[]( const std::size_t& idx ) const ;
	Json& operator=( const Json& rhs ) ;

	void Swap( Json& other ) ;

	std::string Str() const ;
	int		Int() const ;
	double	Double() const ;
	bool	Bool() const ;
	Array	AsArray() const ;
	Object	AsObject() const ;

	template <typename T>
	bool Is() const ;

	template <typename T>
	T As() const ;

	bool Has( const std::string& key ) const ;
	bool Get( const std::string& key, Json& json ) const ;
	void Add( const std::string& key, const Json& json ) ;
	void Add( const Json& json ) ;
	Json FindInArray( const std::string& key, const std::string& value ) const ;
	bool FindInArray( const std::string& key, const std::string& value, Json& result ) const ;

	/**	Expect *this is a JSON array of objects. Select all "key" values inside each
		objects in the array and copies them in the output iterator \a out.
	*/
	template <typename T, typename Out>
	Out Select( const std::string& key, Out out )
	{
		Array a = AsArray() ;
		for ( Array::iterator i = a.begin() ; i != a.end() ; ++i )
		{
			Json value;
			if ( i->Get( key, value ) )
				*out++ = value.As<T>() ;
		}
		return out ;
	}

	friend std::ostream& operator<<( std::ostream& os, const Json& json ) ;
	void Write( DataStream *out ) const ;

	enum Type { null_type, bool_type, double_type, int_type, object_type, array_type, string_type } ;

	Type DataType() const ;

private :
	Json( struct json_object *json ) ;

	struct NotOwned {} ;
	Json( struct json_object *json, NotOwned ) ;

	static struct json_object* InitStr( const char *str, std::size_t n ) ;

	// helper for throwing exception
	template <typename T> static typename Val_<T>::Err ValueErr( const T& t )
	{
		return typename Val_<T>::Err(t);
	}

private :
	struct json_object	*m_json ;
} ;

}
