# Process this file with automake to produce a Makefile.in

SUBDIRS = ${bits_subdir}

DIST_SUBDIRS = x32 x64

noinst_SCRIPTS = ldscript

ldscript: ${bits_subdir}/ldscript ${bits_subdir}/lib${bits_subdir}.a
	cp $< $@
