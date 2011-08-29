/* 
 * xsetleds - reports and changes the led settings of a X display.
 *
 * Copyright (C) 2002 Benedikt Meurer
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */


#ifndef __XSETLEDS_H
#define __XSETLEDS_H


#if	HAVE_CONFIG_H
#include <config.h>
#endif


/* X11 stuff */
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>


/*
 * Take care of NLS matters
 */
#if	HAVE_LOCALE_H
#include <locale.h>
#endif

#if !HAVE_SETLOCALE
#define setlocale(Category, Locale)	/* nothing */
#endif

#if ENABLE_NLS
#include <libintl.h>
#define	_(Text)	gettext(Text)
#else
#undef	bindtextdomain
#define bindtextdomain(Domain, Directory) /* nothing */
#undef	textdomain
#define textdomain(Domain) /* nothing */
#define	_(Text)	Text
#endif


#define VERSION_STRING	PACKAGE " " VERSION	" - " \
		"Copyright (C) 2002 Benedikt Meurer <bmeurer@fwdn.de>\n"


/* defined in xsetleds.c */
extern const char *appname;


/* libgen.h defines basename, but we got our own function */
#ifdef basename
#undef basename
#endif


/* defined in utils.c */
extern void error (const char *format, ...);
extern void warning (const char *format, ...);
extern const char *basename (const char *);


#define	CAPS_LOCK		0x01
#define NUM_LOCK		0x02
#define SCROLL_LOCK		0x04


/* */
struct key {
	const char 		*name;
	const char		*description;
	unsigned int	statemask;
	void			(*action) (Display *, const struct key *);
};


/* */
struct options {
	int				show;		/* show states of leds */
};


#endif	/* __XSETLEDS_H */

