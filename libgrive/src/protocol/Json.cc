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

#include "util/StdioFile.hh"

#include <json/json_tokener.h>
#include <json/linkhash.h>

#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>

namespace gr {

Json::Json( ) :
	m_json( ::json_object_new_object() )
{
	if ( m_json == 0 )
		BOOST_THROW_EXCEPTION( Error() << expt::ErrMsg( "cannot create json object" ) ) ;
}

Json::Json( const char *str ) :
	m_json( ::json_object_new_string( str ) )
{
	if ( m_json == 0 )
		BOOST_THROW_EXCEPTION( 
			Error() << expt::ErrMsg( "cannot create json string \"" + std::string(str) + "\"" ) ) ;
}

template <>
Json::Json( const std::string& str ) :
	m_json( ::json_object_new_string( str.c_str() ) )
{
	if ( m_json == 0 )
		BOOST_THROW_EXCEPTION( 
			Error() << expt::ErrMsg( "cannot create json string \"" + str + "\"" ) ) ;

	// paranoid check
	assert( ::json_object_get_string( m_json ) == str ) ;
}

template <>
Json::Json( const int& l ) :
	m_json( ::json_object_new_int( l ) )
{
	if ( m_json == 0 )
		BOOST_THROW_EXCEPTION( Error() << expt::ErrMsg( "cannot create json int" ) ) ;
}

template <>
Json::Json( const long& l ) :
	m_json( ::json_object_new_int( static_cast<int>(l) ) )
{
	if ( m_json == 0 )
		BOOST_THROW_EXCEPTION( Error() << expt::ErrMsg( "cannot create json int" ) ) ;
}

template <>
Json::Json( const unsigned long& l ) :
	m_json( ::json_object_new_int( static_cast<int>(l) ) )
{
	if ( m_json == 0 )
		BOOST_THROW_EXCEPTION( Error() << expt::ErrMsg( "cannot create json int" ) ) ;
}

template <>
Json::Json( const std::vector<Json>& arr ) :
	m_json( ::json_object_new_array( ) )
{
	if ( m_json == 0 )
		BOOST_THROW_EXCEPTION( Error() << expt::ErrMsg( "cannot create json int" ) ) ;
	
	for ( std::vector<Json>::const_iterator i = arr.begin() ; i != arr.end() ; ++i )
		Add( *i ) ;
}

template <>
Json::Json( const bool& b ) :
	m_json( ::json_object_new_boolean( b ) )
{
	if ( m_json == 0 )
		BOOST_THROW_EXCEPTION( Error() << expt::ErrMsg( "cannot create json bool" ) ) ;
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
		BOOST_THROW_EXCEPTION(
			Error()
				<< expt::ErrMsg( "key: " + key + " is not found in object" )
				<< JsonInfo( *this ) ) ;
	
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
		BOOST_THROW_EXCEPTION(
			Error()
				<< expt::ErrMsg( ss.str() )
				<< JsonInfo( *this ) ) ;
	}
	
	return Json( j ) ;
}

bool Json::Has( const std::string& key ) const
{
	assert( m_json != 0 ) ;
	return ::json_object_object_get( m_json, key.c_str() ) != 0 ;
}

bool Json::Get( const std::string& key, Json& json ) const
{
	assert( m_json != 0 ) ;
	struct json_object *j = ::json_object_object_get( m_json, key.c_str() ) ;
	if ( j != 0 )
	{
		Json tmp( j, NotOwned() ) ;
		json.Swap( tmp ) ;
		return true ;
	}
	else
		return false ;
}

void Json::Add( const std::string& key, const Json& json )
{
	assert( m_json != 0 ) ;
	assert( json.m_json != 0 ) ;
	
	::json_object_get( json.m_json ) ;
	::json_object_object_add( m_json, key.c_str(), json.m_json ) ;
}

void Json::Add( const Json& json )
{
	assert( m_json != 0 ) ;
	assert( json.m_json != 0 ) ;
	
	::json_object_get( json.m_json ) ;
	::json_object_array_add( m_json, json.m_json ) ;
}

bool Json::Bool() const
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

std::string Json::Str() const
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

int Json::Int() const
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

void Json::Write( StdioFile& file ) const
{
	const char *str = ::json_object_to_json_string( m_json ) ;
	file.Write( str, std::strlen(str) ) ;
}

Json::Type Json::DataType() const
{
	assert( m_json != 0 ) ;
	return static_cast<Type>( ::json_object_get_type( m_json ) ) ;
}

Json::Object Json::AsObject() const
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

Json::Array Json::AsArray() const
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
		if ( item.Has(key) && item[key].Str() == value )
			return item ;
	}
	BOOST_THROW_EXCEPTION(
		Error() << expt::ErrMsg( "cannot find " + key + " = " + value + " in array" ) ) ;
}

bool Json::FindInArray( const std::string& key, const std::string& value, Json& result ) const
{
	try
	{
		result = FindInArray( key, value ) ;
		return true ;
	}
	catch ( Error& )
	{
		return false ;
	}
}

Json Json::Parse( const std::string& str )
{
	struct json_object *json = ::json_tokener_parse( str.c_str() ) ;
	if ( json == 0 )
		BOOST_THROW_EXCEPTION( Error() << expt::ErrMsg( "json parse error" ) ) ;
	
	return Json( json, NotOwned() ) ;
}

Json Json::ParseFile( const std::string& filename )
{
	StdioFile file( filename ) ;
	struct json_tokener *tok = ::json_tokener_new() ;
	
	struct json_object *json = 0 ;
	
	char buf[1024] ;
	std::size_t count = 0 ;

	while ( (count = file.Read( buf, sizeof(buf) ) ) > 0 )
		json = ::json_tokener_parse_ex( tok, buf, count ) ;
	
	if ( json == 0 )
		BOOST_THROW_EXCEPTION( Error() << expt::ErrMsg( ::json_tokener_errors[tok->err] ) ) ;
	
	::json_tokener_free( tok ) ;
	
	return Json( json, NotOwned() ) ;
}

}
