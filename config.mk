PRJTYPE = StaticLib
ifeq ($(TARGET_OS), Windows_NT)
	DEFINES += _CRT_SECURE_NO_WARNINGS _CRT_SECURE_NO_DEPRECATE
endif
EXTDEPS = macu::0.0.2dev orb::dev
