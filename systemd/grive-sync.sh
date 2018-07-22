#!/bin/bash

# Copyright (C) 2009 Przemyslaw Pawelczyk <przemoc@gmail.com>
#           (C) 2017 Jan Schulz <jasc@gmx.net>             
##
## This script is licensed under the terms of the MIT license.
## https://opensource.org/licenses/MIT

# Fail on all errors
set -o pipefail

# We always start in the current users home directory so that names always start there
cd ~


### ARGUMENT PARSING ###
SCRIPT="${0}"
DIRECTORY=$(systemd-escape --unescape "$2")

if [[ -z "$DIRECTORY" ]] || [[ ! -d "$DIRECTORY" ]] ; then
	echo "Need a directory name in the current users home directory as second argument. Aborting."
	exit 1
fi


if [[ -z "${1}" ]] ; then
	echo "Need a command as first argument. Aborting."
	exit 1
else
	if [[ "sync" == "${1}" ]] ; then
		COMMAND=sync
	elif [[ "listen" == "${1}" ]] ; then
		COMMAND=listen
	else
		echo "Unknown command. Aborting."
		exit 1
	fi
fi


### LOCKFILE BOILERPLATE ###
LOCKFILE="/run/user/"$(id -u)"/"$(basename "$0")"_"${DIRECTORY//\//_}""
LOCKFD=99

# PRIVATE
_lock()             { flock -"$1" "$LOCKFD"; }
_no_more_locking()  { _lock u; _lock xn && rm -f "$LOCKFILE"; }
_prepare_locking()  { eval "exec "$LOCKFD">\""$LOCKFILE"\""; trap _no_more_locking EXIT; }

# ON START
_prepare_locking

# PUBLIC
exlock_now()        { _lock xn; }  # obtain an exclusive lock immediately or fail
exlock()            { _lock x; }   # obtain an exclusive lock
shlock()            { _lock s; }   # obtain a shared lock
unlock()            { _lock u; }   # drop a lock

### SYNC SCRIPT ###
# Idea: only let one script run, but if the sync script is called a second time 
# make sure we sync a second time, too

sync_directory() {
	_directory="${1}"
	if ping -c1 -W1 -q accounts.google.com >/dev/null 2>&1; then
	    true
	    # pass
	else
	    echo "Google drive server not reachable..."
	    exit 0
	fi

	reset_timer_and_exit() { echo "Retriggered google drive sync" && touch -m "$LOCKFILE" && exit; }

	exlock_now || reset_timer_and_exit

	TIME_AT_START=0
	TIME_AT_END=1
	while [[ "${TIME_AT_START}" -lt "${TIME_AT_END}" ]]; do
	    echo "Syncing "${_directory}"..." 
	    TIME_AT_START="$(stat -c %Y "$LOCKFILE")"
	    # exclude symlinks from sync
	    cat "${_directory}"/.griveignore 2>/dev/null | sed '/#LINKS-EDIT_BEFORE_THIS$/,$d' > /tmp/.griveignore.base
	    cp /tmp/.griveignore.base "${_directory}"/.griveignore
	    rm /tmp/.griveignore.base
	    echo "#LINKS-EDIT_BEFORE_THIS" >> "${_directory}"/.griveignore
	    ( cd "${_directory}" && find . -type l | sed 's/^.\///g'; ) >> "${_directory}"/.griveignore
	    grive -p "${_directory}" 2>&1 | grep -v -E "^Reading local directories$|^Reading remote server file list$|^Synchronizing files$|^Finished!$"
	    TIME_AT_END="$(stat -c %Y "$LOCKFILE")"
	    echo "Sync of "${_directory}" done." 
	done

	# always exit ok, so that we never go into a wrong systemd state
	exit 0
}

### LISTEN TO DIRECTORY CHANGES ###


listen_directory() {
	_directory="${1}"

	type inotifywait >/dev/null 2>&1 || { echo >&2 "I require inotifywait but it's not installed. Aborting."; exit 1; }

	echo "Listening for changes in ~/"${_directory}""

	while true #run indefinitely
	do 
		# Use a different call to not need to change exit into return
		inotifywait -q -r -e modify,attrib,close_write,move,create,delete --exclude ".grive_state|.grive" "${_directory}" > /dev/null 2>&1 && ${SCRIPT} sync "${_directory}"
		#echo ${SCRIPT} "${_directory}"
	done

	# always exit ok, so that we never go into a wrong systemd state
	exit 0
}

if [[ "${COMMAND}" == listen ]] ; then
	listen_directory "${DIRECTORY}"
else
	sync_directory "${DIRECTORY}"
fi

# always exit ok, so that we never go into a wrong systemd state
exit 0
