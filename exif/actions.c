/* actions.c
 *
 * Copyright (C) 2002 Lutz M�ller <lutz@users.sourceforge.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details. 
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <config.h>
#include "actions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libexif/exif-ifd.h>

#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif

#define ENTRY_FOUND     "   *   "
#define ENTRY_NOT_FOUND "   -   "

void
action_tag_table (const char *filename, ExifData *ed)
{
	unsigned int tag;
	const char *name;
	char txt[1024];
	unsigned int i;

	memset (txt, 0, sizeof (txt));
	snprintf (txt, sizeof (txt) - 1, _("EXIF tags in '%s':"), filename);
	printf ("%-38.38s", txt);
	for (i = 0; i < EXIF_IFD_COUNT; i++)
		printf ("%-7.7s", exif_ifd_get_name (i));
	printf ("\n");
	for (tag = 0; tag < 0xffff; tag++) {
		name = exif_tag_get_title (tag);
		if (!name)
			continue;
		printf ("  0x%04x %-29.29s", tag, name);
		for (i = 0; i < EXIF_IFD_COUNT; i++)
			if (exif_content_get_entry (ed->ifd[i], tag))
				printf (ENTRY_FOUND);
			else
				printf (ENTRY_NOT_FOUND);
		printf ("\n");
	}
}

static void
show_entry (ExifEntry *entry, void *data)
{
	unsigned char *ids = data;

	if (*ids)
		printf ("0x%04x", entry->tag);
	else
		printf ("%-20.20s", exif_tag_get_title (entry->tag));
	printf ("|");
	if (*ids)
		printf ("%-73.73s", exif_entry_get_value (entry));
	else
		printf ("%-59.59s", exif_entry_get_value (entry));
	printf ("\n");
}

static void
show_ifd (ExifContent *content, void *data)
{
	exif_content_foreach_entry (content, show_entry, data);
}

static void
print_hline (unsigned char ids)
{
        unsigned int i, width;

        width = (ids ? 6 : 20); 
        for (i = 0; i < width; i++)
                printf ("-");
        printf ("+");
        for (i = 0; i < 79 - width; i++)
                printf ("-");
        printf ("\n");
}

void
action_tag_list (const char *filename, ExifData *ed, unsigned char ids)
{
	ExifByteOrder order;

	if (!ed)
		return;

	order = exif_data_get_byte_order (ed);
	printf (_("EXIF tags in '%s' ('%s' byte order):"), filename,
		exif_byte_order_get_name (order));
	printf ("\n");
	print_hline (ids);
        if (ids)
                printf ("%-6.6s", _("Tag"));
        else
                printf ("%-20.20s", _("Tag"));
        printf ("|");
        if (ids)
                printf ("%-73.73s", _("Value"));
        else
                printf ("%-59.59s", _("Value"));
        printf ("\n");
        print_hline (ids);
	exif_data_foreach_content (ed, show_ifd, &ids);
        print_hline (ids);
        if (ed->size) {
                printf (_("EXIF data contains a thumbnail (%i bytes)."),
                        ed->size);
                printf ("\n");
        }
}