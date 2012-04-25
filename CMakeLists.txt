project(grive)

add_executable( grive
	src/main.cc
	src/OAuth2.cc
	src/HTTP.cc
	src/Json.cc
	src/Drive.cc )

target_link_libraries( grive
	curl
	json )
