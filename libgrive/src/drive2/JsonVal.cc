/*
	grive: an GPL program to sync a local directory with Google Drive
	Copyright (C) 2013 Wan Wai Ho

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
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
	MA  02110-1301, USA.
*/

#include "JsonVal.hh"

#include "Val.hh"
#include "ValBuilder.hh"

#include <yajl/yajl_parse.h>

namespace gr { namespace json {

namespace
{
	int OnNull( void *ctx )
	{
		ValBuilder *b = reinterpret_cast<ValBuilder*>(ctx) ;
		b->BuildNull() ;
		return true ;
	}
	
	int OnBool( void *ctx, int value )
	{
		ValBuilder *b = reinterpret_cast<ValBuilder*>(ctx) ;
		b->Build( static_cast<long long>(value) ) ;
		return true ;
	}
	
	int OnInt( void *ctx, long long value )
	{
		ValBuilder *b = reinterpret_cast<ValBuilder*>(ctx) ;
		b->Build(value) ;
		return true ;
	}
	
	int OnDouble( void *ctx, double value )
	{
		ValBuilder *b = reinterpret_cast<ValBuilder*>(ctx) ;
		b->Build(value) ;
		return true ;
	}
	
	int OnStr( void *ctx, const unsigned char *str, std::size_t len )
	{
		ValBuilder *b = reinterpret_cast<ValBuilder*>(ctx) ;
		b->Build( std::string(reinterpret_cast<const char*>(str), len) ) ;
		return true ;
	}
	
	int StartMap( void *ctx )
	{
		ValBuilder *b = reinterpret_cast<ValBuilder*>(ctx) ;
		b->StartObject() ;
		return true ;
	}
	
	int OnMapKey( void *ctx, const unsigned char *str, std::size_t len )
	{
		ValBuilder *b = reinterpret_cast<ValBuilder*>(ctx) ;
		b->BuildKey( std::string(reinterpret_cast<const char*>(str), len) ) ;
		return true ;
	}
	
	int EndMap( void *ctx )
	{
		ValBuilder *b = reinterpret_cast<ValBuilder*>(ctx) ;
		b->EndObject() ;
		return true ;
	}
	
	int StartArray( void *ctx )
	{
		ValBuilder *b = reinterpret_cast<ValBuilder*>(ctx) ;
		b->StartArray() ;
		return true ;
	}
	
	int EndArray( void *ctx )
	{
		ValBuilder *b = reinterpret_cast<ValBuilder*>(ctx) ;
		b->EndArray() ;
		return true ;
	}

	const yajl_callbacks callbacks = {  
		OnNull,  
		OnBool,  
		OnInt,  
		OnDouble,  
		0,  
		OnStr,  
		StartMap,  
		OnMapKey,  
		EndMap,  
		StartArray,  
		EndArray,  
	};  
}

Val Parse( const std::string& json )
{
	ValBuilder b ;
	yajl_handle hand = yajl_alloc( &callbacks, 0, &b ) ;
	yajl_parse( hand, reinterpret_cast<unsigned const char*>(json.c_str()), json.size() ) ;
	
	if ( yajl_complete_parse(hand) != yajl_status_ok )
	{
		unsigned char *msg = yajl_get_error( hand, true, reinterpret_cast<unsigned const char*>(json.c_str()), json.size() ) ;
		std::string msg_str(reinterpret_cast<char*>(msg)) ;
		yajl_free_error(hand, msg) ;
		
		BOOST_THROW_EXCEPTION( Error() << ParseErr_(msg_str) << JsonText_(json) ) ;
	}
	
	return b.Result() ;
}

} } // end of namespace gr::json
