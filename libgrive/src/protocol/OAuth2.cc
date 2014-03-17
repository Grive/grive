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

#include "OAuth2.hh"

#include "JsonResponse.hh"
#include "Json.hh"

#include "http/CurlAgent.hh"
#include "http/Header.hh"
#include "util/log/Log.hh"

// for debugging
#include <iostream>

namespace gr {

const std::string token_url = "https://accounts.google.com/o/oauth2/token";

OAuth2::OAuth2(
		const std::string& refresh_code,
		const std::string& client_id,
		const std::string& client_secret)
		:
				m_refresh(refresh_code),
				m_client_id(client_id),
				m_client_secret(client_secret)
{
	Refresh();
}

OAuth2::OAuth2(
		const std::string& client_id,
		const std::string& client_secret)
		:
				m_client_id(client_id),
				m_client_secret(client_secret)
{
	m_time = 0;
}

void OAuth2::Auth(
		const std::string& auth_code,
		int tries = OAuth2::default_tries)
		{
	int t = 0;
	bool success = false;

	while (success == false && t < tries) {
		std::string post =
				"code=" + auth_code +
						"&client_id=" + m_client_id +
						"&client_secret=" + m_client_secret +
						"&redirect_uri=" + "urn:ietf:wg:oauth:2.0:oob" +
						"&grant_type=authorization_code";

		http::JsonResponse resp;
		http::CurlAgent http;

		DisableLog dlog(log::debug);
		http.Post(token_url, post, &resp, http::Header());
		time(&m_time);

		Json jresp = resp.Response();
		if (!Json::Get("error", jresp)) {
			m_access = jresp["access_token"].Str();
			m_refresh = jresp["refresh_token"].Str();
			m_expire = jresp["expires_in"].Str();
			success = true;
		}
	}
	if (!success && t >= tries) {
		BOOST_THROW_EXCEPTION(Error() << jresp["error"]);
	}
}

std::string OAuth2::MakeAuthURL(
		const std::string& client_id,
		const std::string& state)
		{
	http::CurlAgent h;
	/*
	 * 		"?scope=" +
	 h.Escape( "https://www.googleapis.com/auth/userinfo.email" )	+ "+" +
	 h.Escape( "https://www.googleapis.com/auth/userinfo.profile" )	+ "+" +
	 h.Escape( "https://docs.google.com/feeds/" )					+ "+" +
	 h.Escape( "https://docs.googleusercontent.com/" )				+ "+" +
	 h.Escape( "https://spreadsheets.google.com/feeds/" )			+
	 *
	 */
	return "https://accounts.google.com/o/oauth2/auth"
			"?scope=" +
			h.Escape("https://www.googleapis.com/auth/drive.file") + "+" +
			h.Escape("https://www.googleapis.com/auth/drive") + "+" +
			h.Escape("https://www.googleapis.com/auth/userinfo.email") + "+" +
			h.Escape("https://www.googleapis.com/auth/userinfo.profile") + "+" +
			h.Escape("https://docs.google.com/feeds/") + "+" +
			h.Escape("https://docs.googleusercontent.com/") + "+" +
			h.Escape("https://spreadsheets.google.com/feeds/") +
			"&redirect_uri=urn:ietf:wg:oauth:2.0:oob"
					"&response_type=code"
					"&client_id=" + client_id;
}

void OAuth2::Refresh(int tries = OAuth2::default_tries)
		{
	int t = 0;
	bool success = false;

	while (success == false && t < tries) {
		t++; // Count try
		std::string post =
				"refresh_token=" + m_refresh +
						"&client_id=" + m_client_id +
						"&client_secret=" + m_client_secret +
						"&grant_type=refresh_token";

		http::JsonResponse resp;
		http::CurlAgent http;

		DisableLog dlog(log::debug);
		http.Post(token_url, post, &resp, http::Header());
		time(&m_time);
		Json jresp = resp.Response();
		if (!Json::Get("error", jresp)) {
			m_access = jresp["access_token"].Str();
			m_expire = jresp["expires_in"].Str();
			success = true;
		}
	}
	if (!success && t >= tries) {
		BOOST_THROW_EXCEPTION(Error() << jresp["error"]);
	}
}

std::string OAuth2::RefreshToken() const
{
	return m_refresh;
}

std::string OAuth2::AccessToken() const
{
	return m_access;
}

std::size_t OAuth2::ExpiresIn() const
{
	return atoi(m_expire.c_str());
}

std::size_t OAuth2::Time() const
{
	return m_time;
}

std::string OAuth2::HttpHeader() const
{
	return "Authorization: Bearer " + m_access;
}

} // end of namespace
