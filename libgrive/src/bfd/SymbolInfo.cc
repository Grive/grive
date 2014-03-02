/*
	grive: an GPL program to sync a local directory with Google Drive
	Copyright (C) 2006  Wan Wai Ho

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

#include "SymbolInfo.hh"
#include "Debug.hh"

#include <vector>

#include <bfd.h>
#include <execinfo.h>
#include <dlfcn.h>

#include <cassert>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <algorithm>

namespace gr {

struct SymbolInfo::Impl
{
	bfd 	*m_bfd ;
	asymbol	**m_symbols ;
	long	m_symbol_count ;
} ;

SymbolInfo::SymbolInfo( )
	: m_impl( new Impl )
{
	m_impl->m_bfd		= 0 ;
	m_impl->m_symbols	= 0 ;
	m_impl->m_symbol_count	= 0 ;
	
	bfd_init( ) ;
	
	// opening itself
	bfd *b = bfd_openr( "/proc/self/exe", 0 ) ;
	if ( b == NULL )
	{
		std::cerr << "cannot open executable: "
		          << bfd_errmsg( bfd_get_error() ) << std::endl ;
		return ;
	}
	
	if ( bfd_check_format( b, bfd_archive ) )
	{
		bfd_close( b ) ;
		return ;
	}
	
	char **matching ;
	if ( !bfd_check_format_matches( b, bfd_object, &matching ) )
	{
		std::cerr << "cannot read symbols from " << bfd_get_filename( b )
		          << ": " << bfd_errmsg( bfd_get_error() ) << std::endl ;

		if ( bfd_get_error( ) == bfd_error_file_ambiguously_recognized )
		{
			std::cerr << bfd_get_filename( b ) << ": Matching formats: " ;
			for ( char **p = matching ; *p != 0 ; p++ )
				std::cerr << " " << *p ;
			
			std::cerr << std::endl ;
			std::free( matching ) ;
		}
		bfd_close( b ) ;
		return ;
	}
	m_impl->m_bfd = b ;

	long storage_needed = bfd_get_symtab_upper_bound( m_impl->m_bfd ) ;

	m_impl->m_symbols		= (asymbol**)std::malloc( storage_needed ) ;
	m_impl->m_symbol_count	= bfd_canonicalize_symtab( b, m_impl->m_symbols ) ;
}

SymbolInfo::~SymbolInfo( )
{
	if ( m_impl->m_symbols != 0 )
		std::free( m_impl->m_symbols ) ;
}

struct SymbolInfo::BacktraceInfo
{
	const SymbolInfo	*m_pthis ;
	void				*m_addr ;
	const char			*m_filename ;
	const char			*m_func_name ;
	unsigned int		m_lineno ;
	unsigned int		m_is_found ;
	
	static void Callback( bfd *abfd, asection *section, void* addr ) ;
} ;

void SymbolInfo::BacktraceInfo::Callback( bfd *abfd, asection *section,
                                          void* data )
{
	BacktraceInfo *info = (BacktraceInfo *)data ;
	if ((section->flags & SEC_ALLOC) == 0)
		return ;
	
	bfd_vma vma = bfd_get_section_vma(abfd, section);
	
	unsigned long address = (unsigned long)(info->m_addr);
	if ( address < vma )
		return;
	
	bfd_size_type size = bfd_section_size(abfd, section);
	if ( address > (vma + size))
		return ;
	
	const SymbolInfo *pthis	= info->m_pthis ;
	info->m_is_found	=  bfd_find_nearest_line( abfd, section,
	                                              pthis->m_impl->m_symbols,
	                                              address - vma,
	                                              &info->m_filename,
	                                              &info->m_func_name,
	                                              &info->m_lineno ) ;
}

std::size_t SymbolInfo::Backtrace( void **stack, std::size_t count )
{
	std::size_t a = ::backtrace( stack, count ) ;
	return a ;
}

void SymbolInfo::PrintTrace( void *addr, std::ostream& os, std::size_t idx )
{
	BacktraceInfo btinfo =
	{
		this, addr, 0, 0, 0, false
	} ;
	
	Dl_info sym ;
	bfd_map_over_sections( m_impl->m_bfd,
							&SymbolInfo::BacktraceInfo::Callback,
							&btinfo ) ;

if ( btinfo.m_is_found )
	{
		std::string filename = ( btinfo.m_filename == 0 ? std::string()
									                    : btinfo.m_filename ) ;
#ifdef SRC_DIR
		std::size_t pos = filename.find( SRC_DIR ) ;
		if ( pos != std::string::npos )
			filename.erase( pos, std::strlen( SRC_DIR ) ) ;
#endif
		os << "#"  << idx << " " << addr << " "
			<< filename << ":" << btinfo.m_lineno 
			<< " "
			<< (btinfo.m_func_name != 0 ? Demangle(btinfo.m_func_name) : "" )
			<< std::endl ;
	}
	else if ( dladdr( addr, &sym ) )
		os << "#"  << idx << " " << addr << " "
			<< sym.dli_fname
			<< " "
			<< (sym.dli_sname != 0 ? Demangle( sym.dli_sname ) : "" )
			<< std::endl ;
}

SymbolInfo* SymbolInfo::Instance( )
{
	static SymbolInfo sthis ;
	return &sthis ;
}

} // end of namespace
