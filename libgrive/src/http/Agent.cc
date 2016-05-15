/*
	Convenience wrapper methods for various kinds of HTTP requests
	Copyright (C) 2015  Vitaliy Filippov

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

#include "Agent.hh"
#include "Header.hh"
#include "util/StringStream.hh"

namespace gr {

namespace http {

long Agent::Put(
	const std::string&	url,
	const std::string&	data,
	DataStream			*dest,
	const Header&		hdr )
{
	StringStream s( data );
	return Request( "PUT", url, &s, dest, hdr );
}

long Agent::Put(
	const std::string&	url,
	File				*file,
	DataStream			*dest,
	const Header&		hdr )
{
	return Request( "PUT", url, (SeekStream*)file, dest, hdr );
}

long Agent::Get(
	const std::string& 	url,
	DataStream			*dest,
	const Header&		hdr )
{
	return Request( "GET", url, NULL, dest, hdr );
}

long Agent::Post(
	const std::string& 	url,
	const std::string&	data,
	DataStream			*dest,
	const Header&		hdr )
{
	Header h( hdr ) ;
	StringStream s( data );
	h.Add( "Content-Type: application/x-www-form-urlencoded" );
	return Request( "POST", url, &s, dest, h );
}

void Agent::SetUploadSpeed( unsigned kbytes )
{
	mMaxUpload = kbytes;
}

void Agent::SetDownloadSpeed( unsigned kbytes )
{
	mMaxDownload = kbytes;
}

} } // end of namespace
