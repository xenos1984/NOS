# Process this file with automake to produce a Makefile.in

all:

refman:
	doxygen -d Validate refman.doxyfile

clean:
	-test -d "html" && rm -r html
	-test -d "man" && rm -r man
