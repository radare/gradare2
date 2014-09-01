/*
 * Copyright (C) 2007, 2008
 *       pancake <youterm.com>
 *
 * radare is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * * radare is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with radare; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "main.h"
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <vte/vte.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

void gradare_new()
{
	system("( gradare & )");
}

static int is_executable(const char *file)
{
	struct stat buf;

	if (stat(file, &buf) != -1)
		return buf.st_mode & S_IXUSR;
}

void gradare_open_program()
{
	GtkWidget *fcd;

	fcd = gtk_file_chooser_dialog_new (
		"Debug program", NULL, // parent
		GTK_FILE_CHOOSER_ACTION_OPEN,
		"_Cancel", GTK_RESPONSE_CANCEL,
		"_Ok", GTK_RESPONSE_ACCEPT,
		NULL);

	gtk_window_set_position( GTK_WINDOW(fcd), GTK_WIN_POS_CENTER);
	if ( gtk_dialog_run(GTK_DIALOG(fcd)) == GTK_RESPONSE_ACCEPT ) {
		char cmd[4096];
		char *filename = (char *)gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fcd));
		if (is_executable(filename)) {
			sprintf(cmd,"( gradare 'dbg://%s' & )", filename);
			system(cmd);
		} else {
			dialog_error("Not executable");
		}
	}

	gtk_widget_destroy(fcd);
}

static GtkWidget *wop = NULL;
GtkWidget *combo = NULL;

static void cancel_cb()
{
	gtk_widget_destroy(wop);
	wop = NULL;
}

static void ok_cb()
{
	//int pid = gtk_combo_box_get_active(combo);
	char dpid[10];
	char cmd[4096];
	char *str = gtk_combo_box_text_get_active_text(
			GTK_COMBO_BOX(combo));
	int pid = atoi(str);

	gtk_widget_destroy(wop);
	cancel_cb();
	sprintf(cmd,"( gradare 'pid://%d' & )", pid);
	sprintf(dpid, "%d", pid);
	setenv("DPID", dpid, 1);
	system(cmd);
}

void gradare_open_process()
{
	int i, fd;
	char cmdline[1025], str[1024];
	GtkWidget *w;
	GtkVBox *vb;
	GtkHBox *hb;
	GtkWidget *hbb;
	GtkWidget *ok, *cancel;

	w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_window_set_title(GTK_WINDOW(w), "Attach to process...");

	vb = (GtkVBox*) gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	hb = (GtkHBox*) gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

	hbb = gtk_hbutton_box_new();
		gtk_container_set_border_width(GTK_CONTAINER(hbb), 5);
		gtk_button_box_set_layout(GTK_BUTTON_BOX(hbb), GTK_BUTTONBOX_END);
		ok = gtk_button_new_from_stock(GTK_STOCK_OK);
		cancel = gtk_button_new_from_stock(GTK_STOCK_CANCEL);

		g_signal_connect(cancel, "button-release-event",
			G_CALLBACK(cancel_cb), NULL);
		gtk_container_add(GTK_CONTAINER(hbb), cancel);

		g_signal_connect(ok, "button-release-event",
			G_CALLBACK(ok_cb), NULL);
		gtk_container_add(GTK_CONTAINER(hbb), ok);

	combo = gtk_combo_box_text_new();

	gtk_box_pack_start(GTK_BOX(hb), gtk_label_new("Select PID"), FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hb), combo, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vb), GTK_WIDGET(hb), FALSE, FALSE, 5);
	gtk_box_pack_end(GTK_BOX(vb), hbb, FALSE, FALSE, 5);

	gtk_container_add(GTK_CONTAINER(w), GTK_WIDGET(vb));

	// NOT portable to w32
	// TODO: use ptrace to get cmdline from esp like tuxi does
	for(i=2;i<999999;i++) {
		switch( kill(i, 0) ) {
		case 0:
			sprintf(cmdline, "/proc/%d/cmdline", i);
			fd = open(cmdline, O_RDONLY);
			cmdline[0] = '\0';
			if (fd != -1) {
				read(fd, cmdline, 1024);
				cmdline[1024] = '\0';
				close(fd);
			}
			sprintf(str, "%d %s", i, cmdline);
			gtk_combo_box_text_insert_text(GTK_COMBO_BOX(combo), i, str);
			//printf("%d %s\n", i, cmdline);
			break;
//		case -1:
//			if (errno == EPERM)
//				printf("%d [not owned]\n", i);
//			break;
		}
	}
	gtk_window_set_position( GTK_WINDOW(w), GTK_WIN_POS_CENTER);
#if 0
	if ( gtk_dialog_run(GTK_DIALOG(fcd)) == GTK_RESPONSE_ACCEPT )
	//{
	//	char cmd[4096];
	kkk	char *filename = (char *)gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fcd));
		sprintf(cmd,"( gradare 'pid://%s' & )", filename);
		system(cmd);
	}
#endif

	gtk_widget_show_all(w);
	wop = w;
//	gtk_widget_destroy(w);
}

void gradare_open()
{
	GtkWidget *fcd;

	fcd = gtk_file_chooser_dialog_new (
		"Open file...", NULL, // parent
		GTK_FILE_CHOOSER_ACTION_OPEN,
		"_Cancel", GTK_RESPONSE_CANCEL,
		"_Ok", GTK_RESPONSE_ACCEPT,
		NULL);

	gtk_window_set_position( GTK_WINDOW(fcd), GTK_WIN_POS_CENTER);
	if ( gtk_dialog_run(GTK_DIALOG(fcd)) == GTK_RESPONSE_ACCEPT )
	{
		char cmd[4096];
		char *filename = (char *)gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fcd));
		sprintf(cmd,"( gradare '%s' & )", filename);
		system(cmd);
	}
	unsetenv("DPID");
	gtk_widget_destroy(fcd);
}

void gradare_shell(const char **cmd)
{
	GtkWidget *vte;
	GtkWindow *w = GTK_WINDOW ( gtk_window_new(GTK_WINDOW_TOPLEVEL) );
	//w->allow_shrink=TRUE;
	gtk_window_resize(GTK_WINDOW(w), 600,400);
	gtk_window_set_title(GTK_WINDOW(w), "radare manpage");
	vte = vte_terminal_new();
	gtk_container_add(GTK_CONTAINER(w), GTK_WIDGET(vte));
	vte_terminal_fork_command_full(
		VTE_TERMINAL(vte),
		VTE_PTY_NO_LASTLOG | VTE_PTY_NO_UTMP | VTE_PTY_NO_WTMP,
		NULL,
		(char **)cmd,
		NULL,
		G_SPAWN_SEARCH_PATH,
		NULL, NULL, NULL, NULL);
	gtk_widget_show_all(GTK_WIDGET(w));
}

void gradare_help()
{
	const char *cmd[] ={"man", "radare2", NULL};
	gradare_shell(cmd);
}


void gradare_nop()
{
	printf("0x90 nop\n");
}

char *home = NULL; //getenv("HOME");
void gradare_shortcut( GtkWidget *widget, gpointer   data )
{
	char *path;
	
	if (home == NULL)
		home = (char *)g_get_home_dir();

	if (home && data) {
		path = malloc(strlen(home)+strlen(data)+128);
		strcpy(path, "sh '");
		strcat(path, home);
		strcat(path, "/.radare/toolbar/");
		strcat(path, (char *)data);
		strcat(path, "'");
		execute_command( path );
		free(path);
	}
}

int v = 0;
void gradare_refresh()
{
	GList *list = gtk_container_get_children(GTK_CONTAINER(tool));

	g_list_foreach(list, (GFunc)gtk_widget_destroy, NULL);
	//gtk_toolbar_remove_space(GTK_TOOLBAR(tool), 0);
	//gtk_toolbar_remove_space(GTK_TOOLBAR(tool), 1);
	//gtk_toolbar_remove_space(GTK_TOOLBAR(tool), 2);

	tool = gradare_toolbar_new(tool);
	vte_terminal_feed_child(VTE_TERMINAL(term), "Q\nV\n", 4);
	v = 1;
}

void toggle_toolbar()
{
	v++;
	switch(v) {
	case 0:
		gtk_widget_show_all(tool);
		gtk_toolbar_set_icon_size(
			GTK_TOOLBAR(tool),
			GTK_ICON_SIZE_LARGE_TOOLBAR);
		break;
	case 1:
		gtk_toolbar_set_icon_size(
			GTK_TOOLBAR(tool),
			GTK_ICON_SIZE_SMALL_TOOLBAR);
		break;
	case 2:
		gtk_widget_hide(tool);
		v=-1;
		break;
	}

	if(v) gtk_toolbar_set_icon_size(GTK_TOOLBAR(tool), GTK_ICON_SIZE_SMALL_TOOLBAR); // LARGE
	else  gtk_toolbar_set_icon_size(GTK_TOOLBAR(tool), GTK_ICON_SIZE_LARGE_TOOLBAR); // LARGE

//	if (v)
//		gtk_widget_show(tool);
//	else
//		gtk_widget_hide(tool);
}

gint gradare_shortcut_click( GtkWidget *widget, GdkEventButton *event, gpointer   data )
{
	char buf[1024];

	if (!home) home = (char *)g_get_home_dir();
	if (event->type == GDK_BUTTON_RELEASE && event->button == 3) {
		strcpy(buf, g_get_home_dir());
		strcat(buf, "/.radare/toolbar/");
		strcat(buf, data);
		unlink(buf);
		gradare_refresh();
	}

	return 0;
}

char *get_shortcut_icon(char *name)
{
	gchar buf[1024];
	FILE *fd;

	if (!home) home = (char *)g_get_home_dir();
	sprintf(buf, "%s/.radare/toolbar/.%s", home, name);
	
	fd = fopen(buf, "r");
	if (fd) {
		fgets(buf, 1000, fd);
		buf[strlen(buf)-1]='\0';
		return g_strdup(buf);
		fclose(fd);
	}
	return g_strdup(GTK_STOCK_INFO);
}

void gradare_fill_toolbar(GtkToolbar *toolbar, char *path)
{
	GDir *dir;
	gchar *name;
	gchar *ico;
	void * item;

	dir = g_dir_open (path, 0, NULL);
	if (!dir) return;

	name = (gchar *)g_dir_read_name (dir);
	while (name != NULL) {
		if (name[0] != '.') {
			ico = get_shortcut_icon(name);
			item = gtk_tool_button_new_from_stock (ico);

			g_signal_connect(item, "button-press-event", G_CALLBACK(gradare_shortcut), name);
			g_signal_connect(item, "button-release-event", G_CALLBACK(gradare_shortcut_click), name);
			gtk_toolbar_insert (toolbar, item, 0);
		}
		name = (gchar *)g_dir_read_name (dir);
	}
}


GtkWidget *gradare_toolbar_new(GtkWidget *base)
{
	char buf[1024];
	GtkWidget *toolbar = base;
	GtkToolItem* item;
	int size;

	if (toolbar == NULL)
		toolbar = gtk_toolbar_new();
	size = gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar));

	/* set defaults */
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
	gtk_container_set_border_width(GTK_CONTAINER(toolbar), 0);
	gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar), GTK_ICON_SIZE_SMALL_TOOLBAR); // LARGE

	/* default gradare buttons */

	item = gtk_tool_button_new (gtk_image_new_from_stock ("gtk-open", size), "Open");
	g_signal_connect(item, "button-press-event", G_CALLBACK(gradare_open), NULL);
	gtk_toolbar_insert (toolbar, item, -1);

	item = gtk_tool_button_new (gtk_image_new_from_stock ("gtk-execute", size), "Open program");
	g_signal_connect(item, "button-press-event", G_CALLBACK(gradare_open_program), NULL);
	gtk_toolbar_insert (toolbar, item, -1);

	item = gtk_tool_button_new (gtk_image_new_from_stock ("gtk-properties", size), "Attach");
	g_signal_connect(item, "button-press-event", G_CALLBACK(gradare_open_process), NULL);
	gtk_toolbar_insert (toolbar, item, -1);

	gtk_toolbar_insert (toolbar, gtk_separator_tool_item_new(), -1);

	item = gtk_tool_button_new (gtk_image_new_from_stock ("gtk-undo", size), "Undo");
	g_signal_connect(item, "button-press-event", G_CALLBACK(gradare_undo), NULL);
	gtk_toolbar_insert (toolbar, item, -1);

	item = gtk_tool_button_new (gtk_image_new_from_stock ("gtk-redo", size), "Redo");
	g_signal_connect(item, "button-press-event", G_CALLBACK(gradare_redo), NULL);
	gtk_toolbar_insert (toolbar, item, -1);

	gtk_toolbar_insert (toolbar, gtk_separator_tool_item_new(), -1);

	item = gtk_tool_button_new (gtk_image_new_from_stock ("gtk-refresh", size), "Refresh");
	g_signal_connect(item, "button-press-event", G_CALLBACK(gradare_refresh), NULL);
	gtk_toolbar_insert (toolbar, item, -1);

#if 0
	gtk_toolbar_append_item(
		GTK_TOOLBAR(toolbar), "Preferences", "Preferences", "",
		gtk_image_new_from_icon_name("gtk-preferences", GTK_ICON_SIZE_MENU),
		G_CALLBACK(prefs_open), NULL);
	gtk_toolbar_append_item(
		GTK_TOOLBAR(toolbar), "Help", "Help", "",
		gtk_image_new_from_icon_name("gtk-help", GTK_ICON_SIZE_MENU),
		G_CALLBACK(gradare_help), NULL);
#endif

	//gtk_toolbar_append_space(GTK_TOOLBAR(toolbar));

	/* append user-defined buttons */
	strcpy(buf, g_get_home_dir());
	strcat(buf, "/.radare/toolbar");
	gradare_fill_toolbar(GTK_TOOLBAR(toolbar), buf);

	return toolbar;
}
