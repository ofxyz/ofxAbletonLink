# All variables and this file are optional, if they are not present the PG and the
# makefiles will try to parse the correct values from the file system.
#
# Variables that specify exclusions can use % as a wildcard to specify that anything in
# that position will match. A partial path can also be specified to, for example, exclude
# a whole folder from the parsed paths from the file system
#
# Variables can be specified using = or +=
# = will clear the contents of that variable both specified from the file or the ones parsed
# from the file system
# += will add the values to the previous ones in the file or the ones parsed from the file 
# system
# 
# The PG can be used to detect errors in this file, just create a new project with this addon 
# and the PG will write to the console the kind of error and in which line it is

meta:
	ADDON_NAME = ofxAbletonLink
	ADDON_DESCRIPTION = Addon for Ableton Link tempo synchronisation
	ADDON_AUTHOR = Akihiro Komori
	ADDON_TAGS = "Sound" "Sync" "Ableton"
	ADDON_URL = https://github.com/2bbb/ofxAbletonLink

common:
	# include search paths
	ADDON_INCLUDES = src
	ADDON_INCLUDES += libs/link/include
	ADDON_INCLUDES += libs/link/include/ableton
	ADDON_INCLUDES += libs/link/modules/asio-standalone/asio/include

	# ASIO is used in header-only standalone mode (no Boost required)
	ADDON_CFLAGS += -DASIO_STANDALONE
	ADDON_CFLAGS += -DASIO_NO_DEPRECATED

	# exclude the Link SDK source tree from being compiled directly
	ADDON_SOURCES_EXCLUDE = libs/link/%

linux64:

linux:

linuxarmv6l:

linuxarmv7l:

msys2:
	# Windows Vista+ required for ASIO/winsock2
	ADDON_CFLAGS += -D_WIN32_WINNT=0x0601

android/armeabi:

android/armeabi-v7a:

osx:

ios:
