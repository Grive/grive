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

#include "JsonWriter.hh"
#include "util/DataStream.hh"

#include <yajl/yajl_gen.h>

#include <cassert>

namespace gr {

struct JsonWriter::Impl
{
	yajl_gen	gen ;
	DataStream 	*out ;
} ;

JsonWriter::JsonWriter( DataStream *out ) :
	m_impl( new Impl )
{
	assert( out != 0 ) ;

	m_impl->out = out ;
	yajl_gen_config conf;
	conf.beautify = 0;
	conf.indentString = "";
	//m_impl->gen = yajl_gen_alloc(0) ;
	// yajl_gen_config( m_impl->gen, yajl_gen_print_callback, &JsonWriter::WriteCallback, this ) ;
	m_impl->gen = yajl_gen_alloc( &conf,(const yajl_alloc_funcs*) &JsonWriter::WriteCallback);
}

JsonWriter::~JsonWriter()
{
	yajl_gen_free( m_impl->gen ) ;
}

void JsonWriter::Visit( long t )
{
	yajl_gen_integer( m_impl->gen, t ) ;
}

void JsonWriter::Visit( double t )
{
	yajl_gen_double( m_impl->gen, t ) ;
}

void JsonWriter::Visit( const std::string& t )
{
	yajl_gen_string( m_impl->gen,
		reinterpret_cast<const unsigned char*>(t.c_str()), t.size() ) ;
}

void JsonWriter::Visit( bool t )
{
	yajl_gen_bool( m_impl->gen, t ) ;
}

void JsonWriter::VisitNull()
{
	yajl_gen_null( m_impl->gen ) ;
}

void JsonWriter::StartArray()
{
	yajl_gen_array_open( m_impl->gen ) ;
}

void JsonWriter::EndArray()
{
	yajl_gen_array_close( m_impl->gen ) ;
}

void JsonWriter::StartObject()
{
	yajl_gen_map_open( m_impl->gen ) ;
}

void JsonWriter::VisitKey( const std::string& t )
{
	Visit(t) ;
}

void JsonWriter::EndObject()
{
	yajl_gen_map_close( m_impl->gen ) ;
}

void JsonWriter::WriteCallback( void *ctx, const char *str, std::size_t size )
{
	JsonWriter *pthis = reinterpret_cast<JsonWriter*>(ctx) ;
	assert( pthis != 0 ) ;
	assert( pthis->m_impl->out != 0 ) ;
	
	pthis->m_impl->out->Write( str, size ) ;
}

} // end of namespace
