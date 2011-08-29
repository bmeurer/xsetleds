bin_PROGRAMS=xsetleds

localedir = $(datadir)/locale

INCLUDES = @X11_CFLAGS@ -DLOCALEDIR=\"$(localedir)\" 

xsetleds_SOURCES=xsetleds.c xsetleds.h utils.c

xsetleds_LDADD = @X11_LDFLAGS@

