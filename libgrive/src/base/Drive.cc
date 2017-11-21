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

#include "Drive.hh"

#include "Entry.hh"
#include "Feed.hh"
#include "Syncer.hh"

#include "http/Agent.hh"
#include "util/Destroy.hh"
#include "util/log/Log.hh"

#include <boost/bind.hpp>

// standard C++ library
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <map>

// for debugging only
#include <iostream>

namespace gr {

Drive::Drive( Syncer *syncer, const Val& options ) :
	m_syncer	( syncer ),
	m_root		( options["path"].Str() ),
	m_state		( m_root, options ),
	m_options	( options )
{
	assert( m_syncer ) ;
}

void Drive::FromRemote( const Entry& entry )
{
	m_state.FromRemote( entry ) ;
}

void Drive::FromChange( const Entry& entry )
{
	if ( entry.IsRemoved() )
		Log( "file \"%1%\" represents a deletion, ignored", entry.Title(), log::verbose ) ;
	
	// folders go directly
	else
		m_state.FromRemote( entry ) ;
}

void Drive::SaveState()
{
	m_state.Write() ;
}

void Drive::DetectChanges()
{
	Log( "Reading local directories", log::info ) ;
	m_state.FromLocal( m_root ) ;

	Log( "Reading remote server file list", log::info ) ;
	std::unique_ptr<Feed> feed = m_syncer->GetAll() ;

	while ( feed->GetNext( m_syncer->Agent() ) )
	{
		std::for_each(
			feed->begin(), feed->end(),
			boost::bind( &Drive::FromRemote, this, _1 ) ) ;
	}
	m_state.ResolveEntry() ;
}

// pull the changes feed
// FIXME: unused until Grive will use the feed-based sync instead of reading full tree
void Drive::ReadChanges()
{
	long prev_stamp = m_state.ChangeStamp() ;
	if ( prev_stamp != -1 )
	{
		Trace( "previous change stamp is %1%", prev_stamp ) ;
		Log( "Detecting changes from last sync", log::info ) ;
		std::unique_ptr<Feed> feed = m_syncer->GetChanges( prev_stamp+1 ) ;
		while ( feed->GetNext( m_syncer->Agent() ) )
		{
			std::for_each(
				feed->begin(), feed->end(),
				boost::bind( &Drive::FromChange, this, _1 ) ) ;
		}
	}
}

void Drive::Update()
{
	Log( "Synchronizing files", log::info ) ;
	m_state.Sync( m_syncer, m_options ) ;
	
	UpdateChangeStamp( ) ;
}

void Drive::DryRun()
{
	Log( "Synchronizing files (dry-run)", log::info ) ;
	m_state.Sync( NULL, m_options ) ;
}

void Drive::UpdateChangeStamp( )
{
	// FIXME: we should go through the changes to see if it was really Grive to made that change
	// maybe by recording the updated timestamp and compare it?
	m_state.ChangeStamp( m_syncer->GetChangeStamp( m_state.ChangeStamp()+1 ) );
}

} // end of namespace gr
