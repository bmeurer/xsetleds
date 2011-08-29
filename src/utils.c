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
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <xsetleds.h>


/* _basename: */
const char *
basename (const char *filename)
{
	const char *p;

	if (p = strrchr (filename, '/'))
		p = strrchr (filename, '/') + 1;
	else
		p = filename;

	return p;
}


/* error: */
void
error (const char *format, ...) 
{
	va_list args;

	va_start (args, format);
	fprintf (stderr, "%s: ", appname);
	vfprintf (stderr, format, args);
	fputc ('\n', stderr);
	fflush (stderr);
	va_end (args);

	exit (1);
}


/* warning: put a warning message on stderr */
void
warning (const char *format, ...)
{
	va_list args;

	va_start (args, format);
	fprintf (stderr, _("%s (warning): "), appname);
	vfprintf (stderr, format, args);
	fputc ('\n', stderr);
	fflush (stderr);
	va_end (args);
}

