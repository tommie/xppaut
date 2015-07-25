AM_CXXFLAGS = -Wall

bin_PROGRAMS = \
	mkavi

noinst_LIBRARIES = \
	libmkavi.a

mkavi_SOURCES = \
	drive.cc

mkavi_LDADD = \
	libmkavi.a

noinst_HEADERS = \
	avi.h \
	chunk.h \
	codec.h \
	mkavi.h \
	patchlevel.h \
	ppm.h

libmkavi_a_SOURCES = \
	mkavi.cc \
	ppm.cc \
	chunk.cc \
	codec.cc \
	avi.cc
