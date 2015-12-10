# Compiler & linker flags
AM_CXXFLAGS = -g -Wall -Werror
AM_LDFLAGS  =

# Enable debug messages
if ENABLE_DEBUG
AM_CXXFLAGS += -DENABLE_DEBUG
endif


SUBDIRS = \
    libaccl \
    unit_test


# Internal headers
noinst_HEADERS = \
    config.hxx

#
# No shared object is necessary so far
# Uncomment in case it becomes so
#lib_LTLIBRARIES = \
#    libaccl.la
#
#libaccl_la_SOURCES =
#
#libaccl_la_LIBADD =
#
#LDADD = libaccl.la


# Executables (none so far)
#bin_PROGRAMS = \
#    accl_something
#
#accl_something_SOURCES = \
#    something.cxx
