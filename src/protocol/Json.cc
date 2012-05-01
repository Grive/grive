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

#include "Json.hh"

#include <json/json_tokener.h>
#include <json/linkhash.h>

#include <cassert>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace gr {

Json::Json( ) :
	m_json( ::json_object_new_object() )
{
	if ( m_json == 0 )
		throw std::runtime_error( "cannot create json object" ) ;
}

template <>
Json::Json( const std::string& str ) :
	m_json( ::json_object_new_string( str.c_str() ) )
{
	if ( m_json == 0 )
		throw std::runtime_error( "cannot create json string \"" + str + "\"" ) ;

	// paranoid check
	assert( ::json_object_get_string( m_json ) == str ) ;
}

Json Json::Parse( const std::string& str )
{
	struct json_object *json = ::json_tokener_parse( str.c_str() ) ;
	if ( json == 0 )
		throw std::runtime_error( "json parse error" ) ;
	
	return Json( json, NotOwned() ) ;
}

Json::Json( struct json_object *json, NotOwned ) :
	m_json( json )
{
	assert( m_json != 0 ) ;
}

Json::Json( struct json_object *json ) :
	m_json( json )
{
	assert( json != 0 ) ;
	::json_object_get( m_json ) ;
}

Json::Json( const Json& rhs ) :
	m_json( rhs.m_json )
{
	assert( m_json != 0 ) ;
	::json_object_get( m_json ) ;
}

Json::~Json( )
{
	assert( m_json != 0 ) ;
	if ( m_json != 0 )
		::json_object_put( m_json ) ;
}

Json& Json::operator=( const Json& rhs )
{
	Json tmp( rhs ) ;
	Swap( tmp ) ;
	return *this ;
}

void Json::Swap( Json& other )
{
	assert( m_json != 0 ) ;
	assert( other.m_json != 0 ) ;
	std::swap( m_json, other.m_json ) ;
}

Json Json::operator[]( const std::string& key ) const
{
	assert( m_json != 0 ) ;
	
	struct json_object *j = ::json_object_object_get( m_json, key.c_str() ) ;
	if ( j == 0 )
		throw std::runtime_error( "key: " + key + " is not found in object" ) ;
	
	return Json( j ) ;
}

Json Json::operator[]( const std::size_t& idx ) const
{
	assert( m_json != 0 ) ;

	struct json_object *j = ::json_object_array_get_idx( m_json, idx ) ;
	if ( j == 0 )
	{
		std::ostringstream ss ;
		ss << "index " << idx << " is not found in array" ;
		throw std::runtime_error( ss.str() ) ;
	}
	
	return Json( j ) ;
}

bool Json::Has( const std::string& key ) const
{
	assert( m_json != 0 ) ;
	return ::json_object_object_get( m_json, key.c_str() ) != 0 ;
}

void Json::Add( const std::string& key, const Json& json )
{
	assert( m_json != 0 ) ;
	assert( json.m_json != 0 ) ;
	
	::json_object_get( json.m_json ) ;
	::json_object_object_add( m_json, key.c_str(), json.m_json ) ;
}

template <>
bool Json::As<bool>() const
{
	assert( m_json != 0 ) ;
	return ::json_object_get_boolean( m_json ) ;
}

template <>
bool Json::Is<bool>() const
{
	assert( m_json != 0 ) ;
	return ::json_object_is_type( m_json, json_type_boolean ) ;
}

template <>
std::string Json::As<std::string>() const
{
	assert( m_json != 0 ) ;
	return ::json_object_get_string( m_json ) ;
}

template <>
bool Json::Is<std::string>() const
{
	assert( m_json != 0 ) ;
	return ::json_object_is_type( m_json, json_type_string ) ;
}

template <>
int Json::As<int>() const
{
	assert( m_json != 0 ) ;
	return ::json_object_get_int( m_json ) ;
}

template <>
bool Json::Is<int>() const
{
	assert( m_json != 0 ) ;
	return ::json_object_is_type( m_json, json_type_int ) ;
}

std::ostream& operator<<( std::ostream& os, const Json& json )
{
	assert( json.m_json != 0 ) ;
	return os << ::json_object_to_json_string( json.m_json ) ;
}

Json::Type Json::DataType() const
{
	assert( m_json != 0 ) ;
	return static_cast<Type>( ::json_object_get_type( m_json ) ) ;
}

template <>
Json::Object Json::As<Json::Object>() const
{
	Object result ;
	
	json_object_object_foreach( m_json, key, val )
	{
		result.insert( Object::value_type( key, Json( val ) ) ) ;
	}
	
	return result ;
}

template <>
bool Json::Is<Json::Object>() const
{
	assert( m_json != 0 ) ;
	return ::json_object_is_type( m_json, json_type_object ) ;
}

template <>
Json::Array Json::As<Json::Array>() const
{
	std::size_t count = ::json_object_array_length( m_json ) ;
	Array result ;
	
	for ( std::size_t i = 0 ; i < count ; ++i )
		result.push_back( Json( ::json_object_array_get_idx( m_json, i ) ) ) ;
	
	return result ;
}

template <>
bool Json::Is<Json::Array>() const
{
	assert( m_json != 0 ) ;
	return ::json_object_is_type( m_json, json_type_array ) ;
}

Json Json::FindInArray( const std::string& key, const std::string& value ) const
{
	std::size_t count = ::json_object_array_length( m_json ) ;
	
	for ( std::size_t i = 0 ; i < count ; ++i )
	{
		Json item( ::json_object_array_get_idx( m_json, i ) ) ;
		if ( item.Has(key) && item[key].As<std::string>() == value )
			return item ;
	}
	throw std::runtime_error( "cannot find " + key + " = " + value + " in array" ) ;
}

bool Json::FindInArray( const std::string& key, const std::string& value, Json& result ) const
{
	try
	{
		result = FindInArray( key, value ) ;
		return true ;
	}
	catch ( std::runtime_error& )
	{
		return false ;
	}
}

std::string Json::Str() const
{
	return As<std::string>() ;
}


}
