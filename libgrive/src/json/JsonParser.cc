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

#include "JsonParser.hh"

#include "Val.hh"
#include "ValBuilder.hh"

#include <yajl/yajl_parse.h>

namespace gr {

namespace
{
	int OnNull( void *ctx )
	{
		ValVisitor *b = reinterpret_cast<ValVisitor*>(ctx) ;
		b->VisitNull() ;
		return true ;
	}
	
	int OnBool( void *ctx, int value )
	{
		ValVisitor *b = reinterpret_cast<ValVisitor*>(ctx) ;
		b->Visit( static_cast<long long>(value) ) ;
		return true ;
	}
	
	int OnInt( void *ctx, long long value )
	{
		ValVisitor *b = reinterpret_cast<ValVisitor*>(ctx) ;
		b->Visit(value) ;
		return true ;
	}
	
	int OnDouble( void *ctx, double value )
	{
		ValVisitor *b = reinterpret_cast<ValVisitor*>(ctx) ;
		b->Visit(value) ;
		return true ;
	}
	
	int OnStr( void *ctx, const unsigned char *str, std::size_t len )
	{
		ValVisitor *b = reinterpret_cast<ValVisitor*>(ctx) ;
		b->Visit( std::string(reinterpret_cast<const char*>(str), len) ) ;
		return true ;
	}
	
	int StartMap( void *ctx )
	{
		ValVisitor *b = reinterpret_cast<ValVisitor*>(ctx) ;
		b->StartObject() ;
		return true ;
	}
	
	int OnMapKey( void *ctx, const unsigned char *str, std::size_t len )
	{
		ValVisitor *b = reinterpret_cast<ValVisitor*>(ctx) ;
		b->VisitKey( std::string(reinterpret_cast<const char*>(str), len) ) ;
		return true ;
	}
	
	int EndMap( void *ctx )
	{
		ValVisitor *b = reinterpret_cast<ValVisitor*>(ctx) ;
		b->EndObject() ;
		return true ;
	}
	
	int StartArray( void *ctx )
	{
		ValVisitor *b = reinterpret_cast<ValVisitor*>(ctx) ;
		b->StartArray() ;
		return true ;
	}
	
	int EndArray( void *ctx )
	{
		ValVisitor *b = reinterpret_cast<ValVisitor*>(ctx) ;
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

Val ParseJson( const std::string& json )
{
	ValBuilder b;
	JsonParser parser( &b ) ;
	parser.Parse( json.c_str(), json.size() ) ;
	parser.Finish() ;
	return b.Result();
}

Val ParseJson( DataStream &in )
{
	ValBuilder b;
	JsonParser parser( &b ) ;
	parser.Parse( in ) ;
	parser.Finish() ;
	return b.Result();
}

struct JsonParser::Impl
{
	ValVisitor	*callback ;
	yajl_handle	hand ;
} ;

JsonParser::JsonParser( ValVisitor *callback ) :
	m_impl( new Impl )
{
	m_impl->callback	= callback ;
	m_impl->hand		= yajl_alloc( &callbacks, 0, m_impl->callback ) ;
}

JsonParser::~JsonParser()
{
	yajl_free( m_impl->hand ) ;
}

void JsonParser::Parse( const char *str, std::size_t size )
{
	const unsigned char *ustr = reinterpret_cast<unsigned const char*>(str) ;
	
	yajl_status r = yajl_parse( m_impl->hand, ustr, size ) ;
	
	if ( r != yajl_status_ok )
	{
		unsigned char *msg = yajl_get_error( m_impl->hand, true, ustr, size ) ;
		std::string msg_str(reinterpret_cast<char*>(msg)) ;
		yajl_free_error(m_impl->hand, msg) ;
		
		BOOST_THROW_EXCEPTION(
			Error()
				<< ParseErr_(msg_str)
				<< JsonText_(std::string(str,size))
		);
	}
}

void JsonParser::Parse( DataStream &in )
{
	char buf[1024] ;
	std::size_t count = 0 ;

	while ( (count = in.Read( buf, sizeof(buf) ) ) > 0 )
	{
		Parse( buf, count );
	}
}

void JsonParser::Finish()
{
	if ( yajl_complete_parse(m_impl->hand) != yajl_status_ok )
	{
		unsigned char *msg = yajl_get_error( m_impl->hand, false, 0, 0 ) ;
		std::string msg_str(reinterpret_cast<char*>(msg)) ;
		yajl_free_error(m_impl->hand, msg) ;
		
		BOOST_THROW_EXCEPTION( Error() << ParseErr_(msg_str) ) ;
	}
}

} // end of namespace gr::json
