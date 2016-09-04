/*
 * Copyright (C) 2006, 2007, 2008, 2009
 *       pancake <youterm.com>
 *
 * canoe is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * canoe is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with canoe; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"

void focus_input()   { gtk_widget_grab_focus(GTK_WIDGET(entry));  }
void focus_combo()   { gtk_widget_grab_focus(GTK_WIDGET(combo));  }
void focus_actions() { gtk_widget_grab_focus(GTK_WIDGET(catact)); }
void focus_vte()     { gtk_widget_grab_focus(GTK_WIDGET(term));   }

void gradare_undo() //GtkAction *action, CanoeWindow *w)
{
	vte_terminal_feed_child(VTE_TERMINAL(term), "u", 1);
}

void gradare_redo() //GtkAction *action, CanoeWindow *w)
{
	vte_terminal_feed_child(VTE_TERMINAL(term), "U", 1);
}

void gradare_edit_script() //GtkAction *action, CanoeWindow *w)
{
	char *cmd = ":H scriptedit\n\n";
	vte_terminal_feed_child(VTE_TERMINAL(term), cmd, strlen(cmd));
}

extern int fontsize;
int console_font_size(int newsize);

void font_increase () {
	fontsize = console_font_size(++fontsize);
}

void font_decrease () {
	fontsize = console_font_size(--fontsize);
}

void gradare_run_script() //GtkAction *action, CanoeWindow *w)
{
	GtkWidget *fcd; 

	fcd = gtk_file_chooser_dialog_new (
		"Select script...", NULL, // parent
		GTK_FILE_CHOOSER_ACTION_OPEN,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
		NULL);

	gtk_window_set_position( GTK_WINDOW(fcd), GTK_WIN_POS_CENTER);
	if ( gtk_dialog_run(GTK_DIALOG(fcd)) == GTK_RESPONSE_ACCEPT )
	{
		char cmd[4096];
		char *filename = (char *)gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fcd));
		if (strstr(filename, "lua"))
			sprintf(cmd,":H lua %s\n", filename);
		else	sprintf(cmd,":. %s\n", filename);
		vte_terminal_feed_child(VTE_TERMINAL(term), cmd, strlen(cmd));
	}

	gtk_widget_destroy(fcd);
}

void do_exit () {
	exit (0);
}

void gradare_about() //GtkAction *action, CanoeWindow *w)
{
        GtkAboutDialog *gad;
        const gchar *authors[3] = {
                "pancake <pancake@youterm.com>",
                NULL, "Graphical frontend for radare"
        };
//printf("NAME: %s\n", gtk_action_get_name(action) );
//printf("WindowPath: %s\n", w->path);

        gad = (GtkAboutDialog *) gtk_about_dialog_new();
#if USE_GTK2
        gtk_about_dialog_set_name(gad, "Gradare");
#endif
        gtk_window_set_position(GTK_WINDOW(gad), GTK_WIN_POS_CENTER);
        gtk_about_dialog_set_authors(gad,authors);
        gtk_about_dialog_set_copyright(gad,authors[2]);
        gtk_about_dialog_set_version(gad, VERSION);
        gtk_window_set_modal(GTK_WINDOW(gad),TRUE);
        //gtk_about_dialog_set_logo(gad,gdk_pixbuf_new_from_xpm_data(canoe_xpm));
        gtk_dialog_run(GTK_DIALOG(gad));
        gtk_widget_destroy(GTK_WIDGET(gad));
}

// "      <menuitem action='Script editor'/>"

extern GtkWindow *window;
static const gchar *ui_info = 
"<ui>"
"  <menubar name='MenuBar'>"
"    <menu action='FileMenu'>"
"      <menuitem action='Open'/>"
"      <menuitem action='Open program'/>"
"      <menuitem action='Open process'/>"
"      <separator />"
"      <menuitem action='Open project'/>"
"      <menuitem action='Save project'/>"
"      <menuitem action='Save project as...'/>"
"      <separator />"
"      <menuitem action='Edit script'/>"
"      <menuitem action='Run script'/>"
"      <separator />"
"      <menuitem action='Quit'/>"
"    </menu>"
"    <menu action='EditMenu'>"
"      <menuitem action='Undo seek'/>"
"      <menuitem action='Redo seek'/>"
"      <separator />"
"      <menuitem action='Focus input'/>"
"      <menuitem action='Focus view'/>"
"      <menuitem action='Focus actions'/>"
"      <menuitem action='Focus terminal'/>"
"      <separator />"
"      <menuitem action='Refresh'/>"
"      <separator />"
"      <menuitem action='Preferences'/>"
"    </menu>"
"    <menu action='ViewMenu'>"
"      <menuitem action='Fullscreen'/>"
"      <menuitem action='New monitor'/>"
"      <separator />"
"      <menuitem action='Code graph'/>"
"      <separator />"
"      <menuitem action='Increase font size'/>"
"      <menuitem action='Decrease font size'/>"
"    </menu>"
"    <menu action='HelpMenu'>"
"      <menuitem action='Manual'/>"
"      <menuitem action='About'/>"
"    </menu>"
"  </menubar>"
"</ui>";

static GtkActionEntry entries[] = {
  { "FileMenu", NULL, "_File" },
  { "Open", GTK_STOCK_OPEN,
    "_Open file","<control>O",
    "Opens a file into gradare",
    G_CALLBACK (gradare_open) },
  { "Open program", GTK_STOCK_EXECUTE,
    "_Open program","<control>D",
    "Opens a program into the radare debugger",
    G_CALLBACK (gradare_open_program) },
  { "Open process", GTK_STOCK_PROPERTIES,
    "_Attach to process","<control>A",
    "Attachs to a running process",
    G_CALLBACK (gradare_open_process) },
  { "Open project", GTK_STOCK_OPEN,
    "_Open project", NULL,
    "Opens a project file",
    G_CALLBACK (gradare_open_project) },
  { "Save project", GTK_STOCK_SAVE,
    "_Save project", NULL,
    "Stores all the information to a project file",
    G_CALLBACK (gradare_save_project) },
  { "Save project as...", GTK_STOCK_SAVE,
    "_Save project as...", NULL,
    "Stores all the information to a project file",
    G_CALLBACK (gradare_save_project_as) },
  { "Run script", GTK_STOCK_GO_FORWARD,
    "_Run script", NULL,
    "Runs a script",
    G_CALLBACK (gradare_run_script) },
  { "Edit script", GTK_STOCK_GO_FORWARD,
    "_Edit script", NULL,
    "Runs GTK scriptedit plugin",
    G_CALLBACK (gradare_edit_script) },
  { "Quit", GTK_STOCK_QUIT,
    "_Quit","<control>Q",
    "Cya!",
    G_CALLBACK (exit) },
  { "ViewMenu", NULL, "_View" },
  { "New monitor", GTK_STOCK_INFO,
    "_New monitor","<control>M",
    "Opens a new monitor window",
    G_CALLBACK (gradare_new_monitor) },
  { "Fullscreen", NULL,
    "Toggle fullscreen",NULL,
    "toggle fullscreen",
    G_CALLBACK (toggle_fullscreen) },
  { "Code graph", GTK_STOCK_INFO,
    "_Code graph",NULL,
    "Open a code graph window",
    G_CALLBACK (gradare_new_graph) },
  { "EditMenu", NULL, "_Edit" },
  { "Refresh", GTK_STOCK_REFRESH,
    "_Refresh","<control>R",
    "",
    G_CALLBACK (gradare_refresh) },
  { "Undo seek", GTK_STOCK_UNDO,
    "_Undo seek","<control>Z",
    "",
    G_CALLBACK (gradare_undo) },
  { "Redo seek", GTK_STOCK_REDO,
    "_Redo seek","<control>Y",
    "",
    G_CALLBACK (gradare_undo) },
// TODO: keybindings for font size does not work
  { "Increase font size", NULL,
    "Increase font size", "<control>_",
    "",
    G_CALLBACK (font_increase) },
  { "Decrease font size", NULL,
    "Decrease font size", "<control>_",
    "",
    G_CALLBACK (font_decrease) },
  { "Focus terminal", NULL,
    "Focus terminal","<control>J",
    "",
    G_CALLBACK (focus_vte) },
  { "Focus actions", NULL,
    "Focus actions","<control>N",
    "",
    G_CALLBACK (focus_actions) },
  { "Focus input", NULL,
    "Focus input","<control>L",
    "Go root directory",
    G_CALLBACK (focus_input) },
  { "Focus view", NULL,
    "Focus view","<control>K",
    "",
    G_CALLBACK (focus_combo) },
  { "Preferences", GTK_STOCK_PROPERTIES,
    "_Preferences","<control>P",
    "",
    G_CALLBACK (prefs_open) },
  { "HelpMenu", NULL, "_Help" },
  { "Manual", GTK_STOCK_HELP,
    "_Manual","<control>H",
    "",
    G_CALLBACK (gradare_help) },
  { "About", GTK_STOCK_ABOUT,
    "_About","",
    "",
    G_CALLBACK (gradare_about) }
};
static guint n_entries = G_N_ELEMENTS (entries);

#if USE_GTK2
GtkMenu *gradare_menubar_hildon_new(GtkWindow *w)
{
	GtkMenuItem *mi;
	GtkMenu * menu = (GtkMenu*) gtk_menu_new();

	mi = (GtkMenuItem *)gtk_image_menu_item_new_with_label("Open file");
	g_signal_connect_object (mi, "activate", gradare_open, w, 0);
	gtk_menu_append (GTK_MENU_SHELL (menu), GTK_WIDGET (mi));

	mi = (GtkMenuItem *) gtk_image_menu_item_new_with_label("Debug program");
	g_signal_connect_object (mi, "activate", gradare_open_program, w, 0);
	gtk_menu_append (GTK_MENU_SHELL (menu), GTK_WIDGET (mi));

	mi = (GtkMenuItem *) gtk_image_menu_item_new_with_label("Attach to process");
	g_signal_connect_object (mi, "activate", gradare_open_process, w, 0);
	gtk_menu_append (GTK_MENU_SHELL (menu), GTK_WIDGET (mi));

	mi = (GtkMenuItem *) gtk_image_menu_item_new_with_label("Edit script");
	g_signal_connect_object (mi, "activate", gradare_edit_script, w, 0);
	gtk_menu_append (GTK_MENU_SHELL (menu), GTK_WIDGET (mi));

	mi = (GtkMenuItem *) gtk_image_menu_item_new_with_label("Code graph");
	g_signal_connect_object (mi, "activate", gradare_new_graph, w, 0);
	gtk_menu_append (GTK_MENU_SHELL (menu), GTK_WIDGET (mi));

	mi = (GtkMenuItem *) gtk_image_menu_item_new_with_label("Monitor window");
	g_signal_connect_object (mi, "activate", gradare_new_monitor, w, 0);
	gtk_menu_append (GTK_MENU_SHELL (menu), GTK_WIDGET (mi));

	mi = (GtkMenuItem *) gtk_image_menu_item_new_with_label("Refresh");
	g_signal_connect_object (mi, "activate", gradare_refresh, w, 0);
	gtk_menu_append (GTK_MENU_SHELL (menu), GTK_WIDGET (mi));

	mi = (GtkMenuItem *) gtk_image_menu_item_new_with_label("Preferences");
	g_signal_connect_object (mi, "activate", prefs_open, w, 0);
	gtk_menu_append (GTK_MENU_SHELL (menu), GTK_WIDGET (mi));

	mi = (GtkMenuItem *) gtk_image_menu_item_new_with_label("About");
	g_signal_connect_object (mi, "activate", gradare_about, w, 0);
	gtk_menu_append (GTK_MENU_SHELL (menu), GTK_WIDGET (mi));

	mi = (GtkMenuItem *) gtk_image_menu_item_new_with_label("Quit");
	g_signal_connect_object (mi, "activate", do_exit, w, 0);
	gtk_menu_append (GTK_MENU_SHELL (menu), GTK_WIDGET (mi));

	return menu;
}
#endif

GtkWidget *gradare_menubar_new(GtkWindow *w)
{
	GtkWidget *menu_bar;
	GtkActionGroup *action_group;
	GtkUIManager *uima;
	GError *error;

	/* menubar */
	uima = gtk_ui_manager_new ();
	g_object_set_data_full (G_OBJECT (w), "ui-manager", uima, 0);

	action_group = gtk_action_group_new ("AppWindowActions");
	gtk_action_group_add_actions (action_group, 
			    entries, n_entries, NULL);

	/* set the first user_data passed to the action group */
	gtk_ui_manager_insert_action_group (uima, action_group, (gint)w);

	gtk_window_add_accel_group (GTK_WINDOW (w),
		gtk_ui_manager_get_accel_group (uima));

	gtk_ui_manager_add_ui_from_string (uima, ui_info, -1, &error);
	menu_bar = gtk_ui_manager_get_widget (uima, "/MenuBar");

	gtk_widget_show_all(menu_bar);

	return menu_bar;
}
