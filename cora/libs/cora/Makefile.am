noinst_LIBRARIES = libcora.a

libcora_a_SOURCES = cora.cc cora.h pnloader.cc pnloader.h imatrix.cc imatrix.h  extmarking.cc extmarking.h cnode.cc cnode.h rnode.cc rnode.h covergraph.cc covergraph.h stubbornset.cc stubbornset.h

BUILT_SOURCES = $(top_srcdir)/src/cmdline.h

# to find customer's config.h header
libpnapi_a_CPPFLAGS = -I$(top_srcdir)/src

$(top_srcdir)/src/cmdline.h:
	$(MAKE) cmdline.c -C$(top_srcdir)/src
