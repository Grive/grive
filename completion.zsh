#compdef grive
# ------------------------------------------------------------------------------
# Copyright (c) 2015 Github zsh-users - http://github.com/zsh-users
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the zsh-users nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL ZSH-USERS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# ------------------------------------------------------------------------------
# Description
# -----------
#
#  Completion script for Grive (https://github.com/vitalif/grive2)
#
# ------------------------------------------------------------------------------
# Authors
# -------
#
#  * Doron Behar <https://github.com/doronbehar>
#
# ------------------------------------------------------------------------------
# -*- mode: zsh; sh-indentation: 2; indent-tabs-mode: nil; sh-basic-offset: 2; -*-
# vim: ft=zsh sw=2 ts=2 et
# ------------------------------------------------------------------------------

local curcontext="$curcontext" state line ret=1
typeset -A opt_args

_arguments -C \
	'(-h --help)'{-h,--help}'[Produce help message]' \
	'(-v --version)'{-v,--version}'[Display Grive version]' \
	'(-a --auth)'{-a,--auth}'[Request authorization token]' \
	'(-p --path)'{-p,--path}'[Root directory to sync]' \
	'(-s --dir)'{-s,--dir}'[Single subdirectory to sync (remembered for next runs)]' \
	'(-V --verbose)'{-V,--verbose}'[Verbose mode. Enable more messages than normal.]' \
	'(--log-http)--log-http[Log all HTTP responses in this file for debugging.]' \
	'(--new-rev)--new-rev[Create,new revisions in server for updated files.]' \
	'(-d --debug)'{-d,--debug}'[Enable debug level messages. Implies -v.]' \
	'(-l --log)'{-l,--log}'[Set log output filename.]' \
	'(-f --force)'{-f,--force}'[Force grive to always download a file from Google Drive instead of uploading it.]' \
	'(--dry-run)--dry-run[Only,detect which files need to be uploaded/downloaded,without actually performing them.]' \
	'(--ignore)--ignore[Perl,RegExp to ignore files (matched against relative paths, remembered for next runs) ]' \
	'*: :_files' && ret=0

return ret
