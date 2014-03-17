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

//        int (* yajl_null)(void * ctx);
	int OnNull( void *ctx )
	{
		ValVisitor *b = reinterpret_cast<ValVisitor*>(ctx) ;
		b->VisitNull() ;
		return true ;
	}
	
//        int (* yajl_boolean)(void * ctx, int boolVal);
	int OnBool( void *ctx, int value )
	{
		ValVisitor *b = reinterpret_cast<ValVisitor*>(ctx) ;
		b->Visit(static_cast<bool>(value));
		return true ;
	}
	
//        int (* yajl_integer)(void * ctx, long integerVal);
	int OnInt( void *ctx, long value )
	{
		ValVisitor *b = reinterpret_cast<ValVisitor*>(ctx) ;
		b->Visit(static_cast<long>(value)) ;
		return true ;
	}
	
//        int (* yajl_double)(void * ctx, double doubleVal);
	int OnDouble( void *ctx, double value )
	{
		ValVisitor *b = reinterpret_cast<ValVisitor*>(ctx) ;
		b->Visit(static_cast<double>(value)) ;
		return true ;
	}
//        int (* yajl_number)(void * ctx, const char * numberVal, unsigned int numberLen);
	int OnNumber( void *ctx, const char *str, unsigned int len)
	{
		ValVisitor *b = reinterpret_cast<ValVisitor*>(ctx) ;
		b->Visit(atol(str)) ;
		return true ;
	}

//        int (* yajl_string)(void * ctx, const unsigned char * stringVal, unsigned int stringLen);
        int OnStr( void *ctx, const unsigned char *str, unsigned int len)
        {
                ValVisitor *b = reinterpret_cast<ValVisitor*>(ctx) ;
                b->Visit(reinterpret_cast<const char*>(str)) ;
                return true ;
        }
//        int (* yajl_start_map)(void * ctx);
	int StartMap( void *ctx )
	{
		ValVisitor *b = reinterpret_cast<ValVisitor*>(ctx) ;
		b->StartObject() ;
		return true ;
	}
	
//        int (* yajl_map_key)(void * ctx, const unsigned char * key, unsigned int stringLen);
	int OnMapKey( void *ctx, const unsigned char *str, unsigned int len )
	{
		ValVisitor *b = reinterpret_cast<ValVisitor*>(ctx) ;
		b->VisitKey( reinterpret_cast<const char*>(str)) ;
		return true ;
	}
	
//        int (* yajl_end_map)(void * ctx);
	int EndMap( void *ctx )
	{
		ValVisitor *b = reinterpret_cast<ValVisitor*>(ctx) ;
		b->EndObject() ;
		return true ;
	}
	
//        int (* yajl_start_array)(void * ctx);
	int StartArray( void *ctx )
	{
		ValVisitor *b = reinterpret_cast<ValVisitor*>(ctx) ;
		b->StartArray() ;
		return true ;
	}
	
//        int (* yajl_end_array)(void * ctx);		
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
		OnNumber,  
		OnStr,  
		StartMap,  
		OnMapKey,  
		EndMap,  
		StartArray,  
		EndArray,  
	};  
}

void JsonParser::Parse( const std::string& json, ValVisitor *callback )
{
	JsonParser parser( callback ) ;
	parser.Parse( json.c_str(), json.size() ) ;
	parser.Finish() ;
}

struct JsonParser::Impl
{
	ValVisitor	*callback ;
	yajl_handle	hand ;
} ;

JsonParser::JsonParser( ValVisitor *callback ) :
	m_impl( new Impl )
{
	yajl_parser_config config;
	m_impl->callback	= callback ;
	config.allowComments = 0;
	config.checkUTF8 = 0;
	m_impl->hand		= yajl_alloc((const yajl_callbacks*) &callbacks,(const yajl_parser_config*) &config, (const yajl_alloc_funcs*)m_impl->callback,(void *) this) ;
}

JsonParser::~JsonParser()
{
	yajl_free( m_impl->hand ) ;
}

void JsonParser::Parse( const char *str, std::size_t size )
{
// TODO: Wide unicode???
	const unsigned char *ustr = reinterpret_cast<unsigned const char*>(str) ;
//	const char *ustr = reinterpret_cast<const char*>(str) ;
	
	yajl_status r = yajl_parse( m_impl->hand, ustr, size ) ;
	
	if ( r != yajl_status_ok )
	{
		unsigned char *msg = yajl_get_error( m_impl->hand, true, ustr, size ) ;
//		char *msg = yajl_get_error( m_impl->hand, true, ustr, size ) ;
		std::string msg_str(reinterpret_cast<char*>(msg)) ;
		yajl_free_error(m_impl->hand, msg) ;
		
		BOOST_THROW_EXCEPTION(
			Error()
				<< ParseErr_(msg_str)
				<< JsonText_(std::string(str,size))
		);
	}
}

void JsonParser::Finish()
{
	if ( yajl_parse_complete(m_impl->hand) != yajl_status_ok )
	{
		unsigned char *msg = yajl_get_error( m_impl->hand, false, 0, 0 ) ;
		std::string msg_str(reinterpret_cast<char*>(msg)) ;
		yajl_free_error(m_impl->hand, msg) ;
		
		BOOST_THROW_EXCEPTION( Error() << ParseErr_(msg_str) ) ;
	}
}

} // end of namespace gr::json
