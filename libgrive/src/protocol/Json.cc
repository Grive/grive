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

#include "util/DataStream.hh"

// needs to include stdint.h before json-c to avoid macro re-def warning
#include <boost/cstdint.hpp>

// disable macro re-def warning for json-c headers
#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable: 4005)
#endif
#include <json_tokener.h>
#include <linkhash.h>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif

#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>

namespace gr {

Json::Json( ) :
	m_json( ::json_object_new_object() )
{
	if ( m_json == 0 )
		BOOST_THROW_EXCEPTION(
			Error()
				<< JsonCApi_( "json_object_new_object" )
		) ;
}

Json::Json( const char *str ) :
	m_json( ::json_object_new_string( str ) )
{
	if ( m_json == 0 )
		BOOST_THROW_EXCEPTION(
			Error()
				<< JsonCApi_( "json_object_new_string" )
				<< ValueErr( str )
		) ;
}

/**	Note that json_object_new_string_len() is not used.
*/
struct json_object* Json::InitStr( const char *str, std::size_t n )
{
	struct json_object *j = ::json_object_new_string( str ) ;
	if ( j == 0 )
		BOOST_THROW_EXCEPTION(
			Error()
				<< JsonCApi_( "json_object_new_string_len" )
				<< ValueErr( std::string(str, n) )
		) ;
	return j ;
}

template <>
Json::Json( const std::string& str ) :
	m_json( InitStr( str.c_str(), str.size() ) )
{
}

template <>
Json::Json( const double& val ) :
	m_json( ::json_object_new_double( val ) )
{
	if ( m_json == 0 )
		BOOST_THROW_EXCEPTION(
			Error()
				<< JsonCApi_( "json_object_new_double" )
				<< ValueErr( val )
		) ;
}

template <>
Json::Json( const boost::int32_t& l ) :
	m_json( ::json_object_new_int( l ) )
{
	if ( m_json == 0 )
		BOOST_THROW_EXCEPTION(
			Error()
				<< JsonCApi_( "json_object_new_int" )
				<< ValueErr( l )
		) ;
}

template <>
Json::Json( const boost::int64_t& l ) :
	m_json( ::json_object_new_int64( l ) )
{
	if ( m_json == 0 )
		BOOST_THROW_EXCEPTION(
			Error()
				<< JsonCApi_( "json_object_new_int64" )
				<< ValueErr( l )
		) ;
}

template <>
Json::Json( const boost::uint32_t& l ) :
	m_json( ::json_object_new_int( static_cast<int>(l) ) )
{
	if ( m_json == 0 )
		BOOST_THROW_EXCEPTION(
			Error()
				<< JsonCApi_( "json_object_new_int" )
				<< ValueErr( l )
		) ;
}

template <>
Json::Json( const boost::uint64_t& l ) :
	m_json( ::json_object_new_int64( l ) )
{
	if ( m_json == 0 )
		BOOST_THROW_EXCEPTION(
			Error()
				<< JsonCApi_( "json_object_new_int64" )
				<< ValueErr( l )
		) ;
}

template <>
Json::Json( const std::vector<Json>& arr ) :
	m_json( ::json_object_new_array( ) )
{
	if ( m_json == 0 )
		BOOST_THROW_EXCEPTION( Error() << JsonCApi_( "json_object_new_array" ) ) ;
	
	for ( std::vector<Json>::const_iterator i = arr.begin() ; i != arr.end() ; ++i )
		Add( *i ) ;
}

template <>
Json::Json( const bool& b ) :
	m_json( ::json_object_new_boolean( b ) )
{
	if ( m_json == 0 )
		BOOST_THROW_EXCEPTION(
			Error()
				<< JsonCApi_( "json_object_new_boolean" )
				<< ValueErr( b )
		) ;
}

template <>
Json::Json( const Object& obj ) :
	m_json( ::json_object_new_object() )
{
	if ( m_json == 0 )
		BOOST_THROW_EXCEPTION( Error() << JsonCApi_( "json_object_new_object" ) ) ;

	for ( Object::const_iterator i = obj.begin() ; i != obj.end() ; ++i )
		Add( i->first, i->second ) ;
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
	
	struct json_object *j = 0 ;
	if ( !::json_object_object_get_ex( m_json, key.c_str(), &j ) )
		BOOST_THROW_EXCEPTION(
			Error()
				<< JsonCApi_( "json_object_object_get" )
				<< KeyNotFound_( key )
				<< Json_( ::json_object_to_json_string(m_json) ) ) ;
	
	assert( j != 0 ) ;
	return Json( j ) ;
}

Json Json::operator[]( const std::size_t& idx ) const
{
	assert( m_json != 0 ) ;

	struct json_object *j = ::json_object_array_get_idx( m_json, idx ) ;
	if ( j == 0 )
	{
		BOOST_THROW_EXCEPTION(
			Error()
				<< JsonCApi_( "json_object_array_get_idx" )
				<< OutOfRange_( idx )
				<< Json_( ::json_object_to_json_string(m_json) ) ) ;
	}
	
	return Json( j ) ;
}

bool Json::Has( const std::string& key ) const
{
	assert( m_json != 0 ) ;
	return ::json_object_object_get_ex( m_json, key.c_str(), 0 ) == TRUE ;
}

bool Json::Get( const std::string& key, Json& json ) const
{
	assert( m_json != 0 ) ;
	struct json_object *j = 0 ;
	if ( ::json_object_object_get_ex( m_json, key.c_str(), &j ) )
	{
		assert( j != 0 ) ;
		
		Json tmp( j ) ;
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
	return ::json_object_get_boolean( m_json ) == TRUE ;
}

template <>
bool Json::Is<bool>() const
{
	assert( m_json != 0 ) ;
	return ::json_object_is_type( m_json, json_type_boolean ) == TRUE ;
}

template <>
bool Json::As<bool>() const
{
	return Bool() ;
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
	return ::json_object_is_type( m_json, json_type_string ) == TRUE ;
}

template <>
std::string Json::As<std::string>() const
{
	return Str() ;
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
	return ::json_object_is_type( m_json, json_type_int ) == TRUE ;
}

template <>
boost::int32_t Json::As<boost::int32_t>() const
{
	return Int() ;
}

template <>
boost::uint32_t Json::As<boost::uint32_t>() const
{
	return static_cast<boost::uint32_t>(Int()) ;
}

template <>
boost::int64_t Json::As<boost::int64_t>() const
{
	return ::json_object_get_int64( m_json ) ;
}

template <>
boost::uint64_t Json::As<boost::uint64_t>() const
{
	return ::json_object_get_int64( m_json ) ;
}

std::ostream& operator<<( std::ostream& os, const Json& json )
{
	assert( json.m_json != 0 ) ;
	return os << ::json_object_to_json_string( json.m_json ) ;
}

void Json::Write( DataStream *out ) const
{
	assert( out != 0 ) ;

	const char *str = ::json_object_to_json_string( m_json ) ;
	out->Write( str, std::strlen(str) ) ;
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
	return ::json_object_is_type( m_json, json_type_object ) == TRUE ;
}

template <>
Json::Object Json::As<Json::Object>() const
{
	return AsObject() ;
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
	return ::json_object_is_type( m_json, json_type_array ) == TRUE ;
}

template <>
Json::Array Json::As<Json::Array>() const
{
	return AsArray() ;
}

///	Finds an element in the array.
///	\pre	"this" is an array
///	\return	*this[i] if *this[i][key] == value
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
		Error()
			<< JsonCApi_( "Json::FindInArray" )
			<< KeyNotFound_( key )
			<< Value_(value)
	) ;
	
	// shut off compiler warnings
	return Json() ;
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
	}
	return false ;
}

Json Json::Parse( const std::string& str )
{
	struct json_object *json = ::json_tokener_parse( str.c_str() ) ;
	if ( json == 0 )
		BOOST_THROW_EXCEPTION(
			Error()
				<< JsonCApi_( "json_tokener_parse" )
				<< ValueErr( str )
		) ;
	
	return Json( json, NotOwned() ) ;
}

/// Parse a file. The file is loaded from file system.
/// \throw	Error	expt::ErrMsg contains a human-readable message describing the
///					error.
Json Json::Parse( DataStream *in )
{
	assert( in != 0 ) ;

	struct json_tokener *tok = ::json_tokener_new() ;
	struct json_object *json = 0 ;
	
	char buf[1024] ;
	std::size_t count = 0 ;

	while ( (count = in->Read( buf, sizeof(buf) ) ) > 0 )
	{
		json = ::json_tokener_parse_ex( tok, buf, count ) ;
		
		// check for parse error
		if ( ::json_tokener_get_error(tok) == ::json_tokener_continue )
			break ;
	}
	
	// save the error code and free the tokener before throwing exceptions
	::json_tokener_error err = ::json_tokener_get_error(tok) ;
	::json_tokener_free( tok ) ; tok = 0 ;
	
	if ( err != json_tokener_success || json == 0 )
	{
		BOOST_THROW_EXCEPTION(
			Error()
				<< JsonCApi_( "json_tokener_parse" )
				<< ErrMsg_( ::json_tokener_error_desc(err) )
		) ;
	}
		
	return Json( json, NotOwned() ) ;
}

}
