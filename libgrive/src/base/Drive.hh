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

#include "base/State.hh"

#include "json/Val.hh"
#include "util/Exception.hh"

#include <string>
#include <vector>

namespace gr {

class Syncer ;

class Entry ;

class State ;

class Drive
{
public :
	Drive( Syncer *syncer, const Val& options ) ;

	void DetectChanges() ;
	void Update() ;
	void DryRun() ;
	void SaveState() ;
	
	struct Error : virtual Exception {} ;
	
private :
	void ReadChanges() ;
	void FromRemote( const Entry& entry ) ;
	void FromChange( const Entry& entry ) ;
	void UpdateChangeStamp( ) ;
	
private :
	Syncer			*m_syncer ;
	fs::path		m_root ;
	State			m_state ;
	Val				m_options ;
} ;

} // end of namespace gr
