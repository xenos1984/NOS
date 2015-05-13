# Process this file with automake to produce a Makefile.in

SUBDIRS = ${subarch_subdir}

DIST_SUBDIRS = ia32 amd64

noinst_SCRIPTS = ldscript

ldscript: ${subarch_subdir}/ldscript ${subarch_subdir}/lib${subarch_subdir}.a
	cp $< $@
