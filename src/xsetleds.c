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


#if	HAVE_CONFIG_H
#include <config.h>
#endif


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include <xsetleds.h>


struct key keytab[] = {
	{ "caps", "CapsLock", CAPS_LOCK, NULL },
	{ "num", "NumLock", NUM_LOCK, NULL },
	{ "scroll", "ScrollLock", SCROLL_LOCK, NULL }
};


#define first(tab)	(tab)
#define last(tab)	((tab) + ((sizeof ((tab)) / sizeof ((tab)[0])) - 1))


const char *appname = NULL;
Display *dpy = NULL;


static void help (void);
static void show (Display *);
static void usage (const char *format, ...);


/* lookup: */
static struct key *
lookup (const char *name)
{
	struct key *k;

	for (k = first (keytab); k <= last (keytab); k++)
		if (strcmp (name, k->name) == 0)
			return k;

	return NULL;
}


/* keycode: */
static KeySym
keycode (Display *dpy, const struct key *k)
{
	if (strcmp (k->name, "caps") == 0)
		return XKeysymToKeycode (dpy, XK_Caps_Lock);
	else if (strcmp (k->name, "num") == 0)
		return XKeysymToKeycode (dpy, XK_Num_Lock);
	else if (strcmp (k->name, "scroll") == 0)
		return XKeysymToKeycode (dpy, XK_Scroll_Lock);

	/* NOTE: this should never be reached. But if so, this is caused
	 * by a programming fault!
	 */
	error (_("keycode: unknown key \'%s\'. "
				"this is a bug, please report it."), k->name);
}


/* toggle: */
static void
toggle (Display *dpy, const struct key *k)
{
	/* toggle the state of the key by sending fake key events */
	KeyCode code = keycode (dpy, k);

	if (!XTestFakeKeyEvent (dpy, code, True, CurrentTime) ||
			!XTestFakeKeyEvent (dpy, code, False, CurrentTime)) {
		error (_("error while sending fake key events"));
	}
}


#if HAVE_XKB
/* state: */
static int
state (Display *dpy, const struct key *k)
{
	unsigned int states;

	/* get key state using XKB */
	if (XkbGetIndicatorState (dpy, XkbUseCoreKbd, &states) != Success)
		error (_("error in reading keyboard indicator states"));

	return (states & k->statemask);
}
#endif


/* enable; */
static void
enable (Display *dpy, const struct key *k)
{
#if HAVE_XKB
	if (!state (dpy, k))	toggle (dpy, k);
#else
	warning (_("setting a led is only supported if " PACKAGE
				" is compiled with XKB support."));
#endif
}


/* disable: */
static void
disable (Display *dpy, const struct key *k)
{
#if HAVE_XKB
	if (state (dpy, k))		toggle (dpy, k);
#else
	warning (_("clearing a led is only supported if " PACKAGE
				" is compiled with XKB support."));
#endif
}


/* readargs: */
static void
readargs (int argc, char *argv[], struct options *opt)
{
	const char *arg;
	struct key *k;

	for (++argv, --argc; argc-- > 0; argv++) {
		arg = *argv;

		if (*arg == '-') {
			if ((k = lookup (arg + 1)) != NULL) {
				/* ok, we have a request to disable a key state */
				k->action = disable;
				continue;
			}

			if (strcmp (arg + 1, "help") == 0) {
				help ();
				exit (0);

			} else if (strcmp (arg + 1, "version") == 0) {
				printf (VERSION_STRING);
				exit (0);

			} else if (strcmp (arg + 1, "display") == 0) {
				if (argc-- > 0) {
					/* simply reset $DISPLAY here */
					setenv ("DISPLAY", *++argv, 1);
					continue;
				}

			} else if (strcmp (arg + 1, "show") == 0) {
				opt->show = 1;
				continue;
			}

		} else if (*arg == '+') {
			if ((k = lookup (arg + 1)) != NULL) {
				/* ok, we have a request to enable a key state */
				k->action = enable;
				continue;
			}

		} else if (isalpha (*arg)) {
			if ((k = lookup (arg)) != NULL) {
				/* ok, this is a toggle request */
				k->action = toggle;
				continue;
			}
		}

		usage (_("bad command line option \"%s\""), arg);
	}
}


/* main: */
int main (int argc, char *argv[])
{
	struct options opt = {0};
	struct key *k;

	/* get name of application */
	appname = basename (argv[0]);

	/* initialize GNU gettext */
	setlocale (LC_ALL, "");
	bindtextdomain (PACKAGE, LOCALEDIR);
	textdomain (PACKAGE);

	/* parse arguments */
	readargs (argc, argv, &opt);

	/* open connection to X display */
	if ((dpy = XOpenDisplay (NULL)) == NULL)
		error (_("cannot open display \"%s\""), getenv ("DISPLAY"));

	/* set key states */
	for (k = first (keytab); k <= last (keytab); k++)
		if (k->action)
			k->action (dpy, k);

	/* print out led states */
	if (opt.show)
		show (dpy);

	/* close connection to X display */
	XCloseDisplay (dpy);

	return 0;
}

/* usage: */
static void
usage (const char *format, ...)
{
	va_list args;

	va_start (args, format);
	fprintf (stderr, "%s: ", appname);
	vfprintf (stderr, format, args);
	fprintf (stderr, "\n\n");
	va_end (args);

	fprintf (stderr, _(
"usage: %s [-version] [-help] [-show] [-display displayname] [[+/-]caps]\n"
"    [[+/-]num] [[+/-]scroll]\n"
"\n"
"Type %s -help for a full description.\n"
"\n"), appname, appname);

	exit (1);
}


/* help: */
static void
help (void)
{
	fprintf (stderr, _(
"usage:\n"
"   %s [options ...]\n"
"\n"
"where options include:\n"
"    -version                     print the version number\n"
"    -help                        print this message\n"
"    -show                        print the current led states\n"
"    -display displayname         X server to connect to\n"
"    +/-caps                      Set/clear/toggle Caps Lock\n"
"    +/-num                       Set/clear/toggle Num Lock\n"
"    +/-scroll                    Set/clear/toggle Scroll Lock\n"
"\n"
"Note, that clearing, setting and showing the states of the leds is only\n"
"avaiable if %s is compiled with XKB support.\n"
"\n"),
			appname, PACKAGE);
}


/* show: */
static void
show (Display *dpy)
{
#if HAVE_XKB
	struct key *k;

	printf (_("current states: "));

	for (k = first (keytab); k <= last (keytab); k++) {
		printf ("%s %s   ", k->description,
				state (dpy, k) ? _(" on") : _("off"));
	}

	printf ("\n");
#else
	warning (_("showing the led states is only possible if "
				PACKAGE " is compiled with XKB support."));
#endif
}


