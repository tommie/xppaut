#include "file-selector.h"

#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "editutil.h"
#include "../ggets.h"
#include "../main.h"
#include "../many_pops.h"
#include "../mykeydef.h"
#include "../pop_list.h"
#include "../read_dir.h"
#include "../strutil.h"
#include "../base/timeutil.h"
#include "../bitmap/filebrowse.bitmap"
#include "../bitmap/linedn.bitmap"
#include "../bitmap/lineup.bitmap"
#include "../bitmap/pagedn.bitmap"
#include "../bitmap/pageup.bitmap"
#include "../bitmap/home.bitmap"
#include "../bitmap/start.bitmap"

/* --- Macros --- */
#define FILESELNWIN 10

#define HOTWILD 2
#define HOTFILE 1

/* --- Types --- */
typedef struct {
  int n, n0;
  Window base, cancel, ok, up, dn, pgup, pgdn, file, wild, w[FILESELNWIN], dir,
      home, start;
  Window fw, ww;
  char wildtxt[256], filetxt[256];
  int nwin, minwid, minhgt;
  int off, pos, hot;
  char title[256];
} FILESEL;

static int button_selector(FILESEL *filesel, Window w);
static void create_file_selector(FILESEL *filesel, const char *title, const char *file, const char *wild);
static void crossing_selector(FILESEL *filesel, Window w, int c);
static void destroy_selector(FILESEL *filesel);
static void display_file_sel(FILESEL *f, Window w);
static int do_file_select_events(FILESEL *filesel);
static int edit_fitem(FILESEL *filesel, int ch, char *string, Window w, int *off1, int *pos1,
                      int mc);
static void expose_selector(FILESEL *filesel, Window w);
static void fs_scroll(FILESEL *filesel, int i);
static void new_wild(FILESEL *filesel);
static void redraw_directory(FILESEL *filesel);
static void redraw_file_list(FILESEL *filesel);
static void redraw_fs_text(FILESEL *filesel, char *string, Window w, int flag);
static int selector_key(FILESEL *filesel, XEvent ev);

void expose_selector(FILESEL *filesel, Window w) { display_file_sel(filesel, w); }

/* this is rather lazy and slow but hey it works */
void redraw_directory(FILESEL *filesel) {
  XClearWindow(display, filesel->dir);
  expose_selector(filesel, filesel->dir);
}

void redraw_file_list(FILESEL *filesel) {
  int i;
  for (i = 0; i < filesel->nwin; i++) {
    XClearWindow(display, filesel->w[i]);
    expose_selector(filesel, filesel->w[i]);
  }
}

void redraw_fs_text(FILESEL *filesel, char *string, Window w, int flag) {
  XClearWindow(display, w);
  filesel->off = 0;
  if (flag)
    filesel->pos = strlen(string);
  XDrawString(display, w, small_gc, 0, CURY_OFF, string, strlen(string));
  if (flag)
    put_edit_cursor(w, DCURXs * strlen(string));
}

void display_file_sel(FILESEL *f, Window w) {
  int i, i0;
  Window root;
  int xloc;
  int yloc;

  unsigned int cwid;
  unsigned int chgt;
  unsigned int cbwid;
  unsigned int cdepth;

  XGetGeometry(display, f->base, &root, &xloc, &yloc, &cwid, &chgt, &cbwid,
               &cdepth);
  XResizeWindow(display, f->wild, cwid - 7 * DCURXs - 5, DCURYs);
  XResizeWindow(display, f->file, cwid - 7 * DCURXs - 5, DCURYs);
  for (i = 0; i < f->nwin; i++) {
    XResizeWindow(display, f->w[i], cwid - 6 * DCURXs - 10, DCURYs);
  }
  int hgt = DCURYs + 4;
  XMoveResizeWindow(display, f->ok, cwid / 2 - 7 * DCURXs - 3, chgt - hgt,
                    7 * DCURXs, DCURYs);
  XMoveResizeWindow(display, f->cancel, cwid / 2 + 3, chgt - hgt, 7 * DCURXs,
                    DCURYs);

  char t[256];
  if (f->ok == w)
    XDrawString(display, w, small_gc, 5, CURY_OFFs, "Ok", 2);
  if (f->cancel == w)
    XDrawString(display, w, small_gc, 5, CURY_OFFs, "Cancel", 6);
  if (f->up == w) {
    /*XDrawString(display,w,small_gc,5+DCURX/2,CURY_OFFs,"^",1);
    */
  }
  if (f->dn == w) {
    /*XDrawString(display,w,small_gc,5+DCURX/2,CURY_OFFs,"vv",1);
    */
  }
  if (f->pgup == w) {
    /*XDrawString(display,w,small_gc,5,CURY_OFFs,"^^",2);
    */
  }
  if (f->pgdn == w) {
    /* XDrawString(display,w,small_gc,5,CURY_OFFs,"vv",2);
    */
  }
  if (f->file == w) {
    XClearWindow(display, w);
    XDrawString(display, w, small_gc, 2, CURY_OFFs, f->filetxt,
                strlen(f->filetxt));
  }
  if (f->wild == w) {
    XClearWindow(display, w);
    XDrawString(display, w, small_gc, 2, CURY_OFFs, f->wildtxt,
                strlen(f->wildtxt));
  }
  if (f->fw == w)
    XDrawString(display, w, small_gc, 5, CURY_OFFs, "File: ", 6);
  if (f->ww == w)
    XDrawString(display, w, small_gc, 5, CURY_OFFs, "Wild: ", 6);
  if (f->dir == w) {
    sprintf(t, " %s", f->title);
    XDrawString(display, w, small_gc, 0, CURY_OFFs, t, strlen(t));
    XTextProperty windowName;
    sprintf(t, "%s - %s", f->wildtxt, cur_dir);
    char *nameit[] = {t};
    XStringListToTextProperty(nameit, 1, &windowName);
    XSetWMName(display, f->base, &windowName);
  }
  for (i = 0; i < f->nwin; i++) {
    if (w == f->w[i]) {
      i0 = i + f->n0;
      if (i0 >= f->n)
        XDrawString(display, w, small_gc, 5, CURY_OFFs, " ", 1);
      else {
        if (i0 < my_ff.ndirs)
          sprintf(t, "<>%s", my_ff.dirnames[i0]);
        else
          sprintf(t, "%s", my_ff.filenames[i0 - my_ff.ndirs]);
        XDrawString(display, w, small_gc, 5, CURY_OFFs, t, strlen(t));
      }
    }
  }
}

void new_wild(FILESEL *filesel) {
  free_finfo(&my_ff); /* delete the old file info */
  filesel->n0 = 0;     /* back to the top of the list */
  get_fileinfo(filesel->wildtxt, cur_dir, &my_ff);
  filesel->n = my_ff.ndirs + my_ff.nfiles;
  redraw_file_list(filesel);
  XFlush(display);
}

void fs_scroll(FILESEL *filesel, int i) {
  int n0 = filesel->n0;
  int new, nend;
  int nw = filesel->nwin, n = filesel->n;
  if (n <= nw)
    return;
  new = n0 - i;
  nend = new + nw;
  if (new < 0)
    new = 0;
  if (nend > n)
    new = n - nw;
  filesel->n0 = new;
  redraw_file_list(filesel);
}

int button_selector(FILESEL *filesel, Window w) {
  int i, i0;
  int k, n = filesel->n;
  if (w == filesel->ok)
    return 1;
  if (w == filesel->cancel)
    return 2;
  if (w == filesel->up)
    fs_scroll(filesel, 1);
  if (w == filesel->dn)
    fs_scroll(filesel, -1);
  if (w == filesel->pgup)
    fs_scroll(filesel, filesel->nwin);
  if (w == filesel->pgdn)
    fs_scroll(filesel, -filesel->nwin);
  if (w == filesel->home) {

    char *HOMEDIR = getenv("HOME");
    if ((HOMEDIR == NULL) || (strlen(HOMEDIR) == 0)) {
      plintf("User's HOME environment variable not set.\n");
      return 0;
    }
    change_directory(HOMEDIR);

    get_directory(cur_dir);
    redraw_directory(filesel);
    free_finfo(&my_ff); /* delete the old file info */
    filesel->n0 = 0;     /* back to the top of the list */
    get_fileinfo(filesel->wildtxt, cur_dir, &my_ff);
    filesel->n = my_ff.ndirs + my_ff.nfiles;

    strcpy(filesel->filetxt, cur_dir);

    int m = strlen(filesel->filetxt);
    if (filesel->filetxt[m - 1] != '/') {
      strcat(filesel->filetxt, "/");
    }

    redraw_fs_text(filesel, filesel->filetxt, filesel->file, 1);
    redraw_file_list(filesel);
    XFlush(display);

    return 0;
  }
  if (w == filesel->start) {

    char *START = getenv("XPPSTART");

    if ((START == NULL) || (strlen(START) == 0)) {
      plintf("User's XPPSTART environment variable not set.\n");
      return 0;
    }

    change_directory(START);

    get_directory(cur_dir);
    redraw_directory(filesel);
    free_finfo(&my_ff); /* delete the old file info */
    filesel->n0 = 0;     /* back to the top of the list */
    get_fileinfo(filesel->wildtxt, cur_dir, &my_ff);
    filesel->n = my_ff.ndirs + my_ff.nfiles;

    strcpy(filesel->filetxt, cur_dir);

    int m = strlen(filesel->filetxt);
    if (filesel->filetxt[m - 1] != '/') {
      strcat(filesel->filetxt, "/");
    }

    redraw_fs_text(filesel, filesel->filetxt, filesel->file, 1);
    redraw_file_list(filesel);
    XFlush(display);

    return 0;
  }
  if (w == filesel->file) { /* selected the file text */
    if (filesel->hot != HOTFILE)
      filesel->pos = strlen(filesel->filetxt);

    filesel->hot = HOTFILE;
    redraw_fs_text(filesel, filesel->filetxt, filesel->file, 1);
    redraw_fs_text(filesel, filesel->wildtxt, filesel->wild, 0);
    /* set up text stuff */
    return 0;
  }
  if (w == filesel->wild) {
    if (filesel->hot != HOTWILD)
      filesel->pos = strlen(filesel->wildtxt);
    filesel->hot = HOTWILD;
    redraw_fs_text(filesel, filesel->filetxt, filesel->file, 0);
    redraw_fs_text(filesel, filesel->wildtxt, filesel->wild, 1);
    return 0;
  }
  i0 = -1;
  for (i = 0; i < filesel->nwin; i++)
    if (w == filesel->w[i])
      i0 = i;
  if (i0 > -1) { /* clicked on a file or directory */
    k = i0 + filesel->n0;
    if (k < my_ff.ndirs) { /* it is a directory so we should reset */
      change_directory(my_ff.dirnames[k]);
      get_directory(cur_dir);
      redraw_directory(filesel);
      free_finfo(&my_ff); /* delete the old file info */
      filesel->n0 = 0;     /* back to the top of the list */
      get_fileinfo(filesel->wildtxt, cur_dir, &my_ff);
      filesel->n = my_ff.ndirs + my_ff.nfiles;

      strcpy(filesel->filetxt, cur_dir);

      int m = strlen(filesel->filetxt);
      if (filesel->filetxt[m - 1] != '/') {
        strcat(filesel->filetxt, "/");
      }

      redraw_fs_text(filesel, filesel->filetxt, filesel->file, 1);
      redraw_file_list(filesel);
      XFlush(display);

      return 0;
    }
    if (k < n) {
      k = k - my_ff.ndirs;
      strcpy(filesel->filetxt, my_ff.filenames[k]);
      return 1; /* got a file */
    }
  }
  return 0;
}

void crossing_selector(FILESEL *filesel, Window w, int c) {
  int t1 = 1, t2 = 2, i;
  if (c == 1) {
    t1 = 0;
    t2 = 1;
  }
  for (i = 0; i < filesel->nwin; i++) {
    if (w == filesel->w[i]) {
      XSetWindowBorderWidth(display, w, t1);
      return;
    }
  }
  if (w == filesel->ok || w == filesel->cancel || w == filesel->pgup ||
      w == filesel->pgdn || w == filesel->up || w == filesel->dn ||
      w == filesel->file || w == filesel->wild || w == filesel->home ||
      w == filesel->start)
    XSetWindowBorderWidth(display, w, t2);
}

int do_file_select_events(FILESEL *filesel) {
  int done;
  XEvent ev;
  while (1) {
    XNextEvent(display, &ev);
    switch (ev.type) {
    case ConfigureNotify:
    case Expose:
    case MapNotify:
      if (Xup)
        do_expose(ev);
      expose_selector(filesel, ev.xany.window);
      break;
    case ButtonPress:
      done = button_selector(filesel, ev.xbutton.window);
      if (done == 1)
        return 1; /* OK made a selection */
      if (done == 2)
        return 0; /* canceled the whole thing */
      break;
    case EnterNotify:
      crossing_selector(filesel, ev.xcrossing.window, 0);
      break;
    case LeaveNotify:
      crossing_selector(filesel, ev.xcrossing.window, 1);
      break;
    case KeyPress:
      done = selector_key(filesel, ev);
      if (done == 2)
        return 0;
      if (done == 1)
        return 1;
      break;
    }
  }
}

void create_file_selector(FILESEL *filesel, const char *title, const char *file, const char *wild) {
  int n = my_ff.ndirs + my_ff.nfiles;
  int nwin = FILESELNWIN;
  /*int wid,hgt,i;
  */
  int hgt, i;
  int width, height;

  Window base;
  XTextProperty winname;
  XSizeHints size_hints;
  filesel->n = n;
  filesel->n0 = 0;
  filesel->nwin = nwin;
  strcpy(filesel->title, title);
  strcpy(filesel->wildtxt, wild);
  strcpy(filesel->filetxt, file);
  width = 80 * DCURXs;
  /*wid=30*DCURXs;*/
  hgt = DCURYs + 4;
  height = (5 + nwin) * hgt;
  filesel->minwid = width;
  filesel->minhgt = height;
  /* plintf("Title=%s\n",title); */
  /* printf("Here now 23!\n");*/
  base = make_plain_window(RootWindow(display, screen), 0, 0, width, height, 4);
  /* printf("Here now 23!\n"); */
  filesel->base = base;
  XStringListToTextProperty((char**)&title, 1, &winname);
  size_hints.flags = PPosition | PSize | PMinSize;
  size_hints.x = 0;
  size_hints.y = 0;
  size_hints.width = width;
  size_hints.height = height;
  size_hints.min_width = width;
  size_hints.min_height = height;
  size_hints.max_width = width;
  size_hints.max_height = height;

  make_icon((char *)filebrowse_bits, filebrowse_width, filebrowse_height, base);

  XClassHint class_hints;
  class_hints.res_name = "";
  class_hints.res_class = "";

  XSetWMProperties(display, base, &winname, NULL, NULL, 0, &size_hints, NULL,
                   &class_hints);

  filesel->up = make_icon_window(base, DCURXs, 2 + 3 * hgt + 72 + 15, 32, 24, 1,
                                0, 0, lineup_bits);
  filesel->dn = make_icon_window(base, DCURXs, 2 + 3 * hgt + 96 + 18, 32, 24, 1,
                                0, 0, linedn_bits);
  filesel->pgup = make_icon_window(base, DCURXs, 2 + 3 * hgt + 48 + 12, 32, 24,
                                  1, 0, 0, pageup_bits);
  filesel->pgdn = make_icon_window(base, DCURXs, 2 + 3 * hgt + 120 + 21, 32, 24,
                                  1, 0, 0, pagedn_bits);
  filesel->home =
      make_icon_window(base, DCURXs, 2 + 3 * hgt, 32, 24, 1, 0, 0, home_bits);
  filesel->start = make_icon_window(base, DCURXs, 2 + 3 * hgt + 24 + 3, 32, 24,
                                   1, 0, 0, start_bits);

  filesel->dir =
      make_plain_window(base, 7 * DCURXs, 2, width - 7 * DCURXs - 5, DCURYs, 0);
  filesel->wild = make_plain_window(base, 7 * DCURXs, 2 + hgt,
                                   width - 7 * DCURXs - 5, DCURYs, 1);
  filesel->ww = make_window(base, 2, 2 + hgt, 6 * DCURXs + 2, DCURYs, 0);
  filesel->file = make_plain_window(base, 7 * DCURXs, 2 + 2 * hgt,
                                   width - 7 * DCURXs - 5, DCURYs, 1);
  filesel->fw = make_window(base, 2, 2 + 2 * hgt, 6 * DCURXs + 2, DCURYs, 0);
  for (i = 0; i < nwin; i++) {
    filesel->w[i] = make_plain_window(base, 6 * DCURXs + 5, 2 + (3 + i) * hgt,
                                     width - 6 * DCURXs - 10, DCURYs, 0);
  }

  filesel->ok = make_window(base, width / 2 - 7 * DCURXs - 3, height - hgt,
                           7 * DCURXs, DCURYs, 1);
  filesel->cancel =
      make_window(base, width / 2 + 3, height - hgt, 7 * DCURXs, DCURYs, 1);
  filesel->hot = HOTFILE;
  filesel->pos = strlen(filesel->filetxt);
  filesel->off = 0;
}

int edit_fitem(FILESEL *filesel, int ch, char *string, Window w, int *off1, int *pos1, int mc) {
  int l = strlen(string), cp;
  int off = *off1, pos = *pos1, wpos = pos - off;
  switch (ch) {
  case LEFT:
    if (pos > 0) {
      pos--;
      wpos--;
      if (wpos < 0) {
        off = off - 4;
        if (off < 0)
          off = 0;
        wpos = pos - off;
      }
    } else
      ping();
    break;
  case RIGHT:
    if (pos < l) {
      pos++;
      wpos++;
      if (wpos > mc) {
        off = off + 4;
        if (off + mc > l)
          off = l - mc;
        wpos = pos - off;
      }
    } else
      ping();
    break;
  case HOME:
    pos = 0;
    wpos = 0;
    break;
  case END:
    pos = l;
    wpos = mc;
    break;
  case BADKEY:
    return 0;

  case DOWN:
    fs_scroll(filesel, -1);
    return 0;
  case UP:
    fs_scroll(filesel, 1);
    return 0;
  case PGUP:
    fs_scroll(filesel, filesel->nwin);
    return 0;
  case PGDN:
    fs_scroll(filesel, -filesel->nwin);
    return 0; /* junk key  */
  case ESC:
    return EDIT_ESC;
  case FINE:
    return EDIT_DONE;
  case BKSP:
  /*
  if(pos<l){
    memmov(&string[pos],&string[pos+1],l-pos);
    l--;
  }
  else
   ping();
   break; */
  case DEL:

    if (pos > 0) {
      memmov(&string[pos - 1], &string[pos], l - pos + 1);
      pos--;
      wpos--;
      if (wpos < 0) {
        off = off - 4;
        if (off < 0)
          off = 0;
        wpos = pos - off;
      }
      l--;
    } else
      ping();
    break;
  case TAB: /*TAB completion of file names */
  {

    struct dirent *dp;
    /*char ft[100];
    char ftpath[100];
    */

    char ft[XPP_MAX_NAME];
    char ftpath[XPP_MAX_NAME];

    /*User may have typed ahead (maybe they remember the path they want)"*/
    /*Try to change to that new directory if it is one.*/
    if ((dp = (struct dirent *)opendir(filesel->filetxt)) != NULL) {
      if (strcmp(cur_dir, filesel->filetxt) != 0) {
        change_directory(filesel->filetxt);
        get_directory(cur_dir);
        redraw_directory(filesel);
        free_finfo(&my_ff); /* delete the old file info */
        filesel->n0 = 0;     /* back to the top of the list */
        get_fileinfo(filesel->wildtxt, cur_dir, &my_ff);
        filesel->n = my_ff.ndirs + my_ff.nfiles;
        strcpy(filesel->filetxt, cur_dir);
        int m = strlen(filesel->filetxt);
        if (filesel->filetxt[m - 1] != '/') {
          strcat(filesel->filetxt, "/");
        }
        redraw_fs_text(filesel, filesel->filetxt, filesel->file, 1);
        redraw_file_list(filesel);
        XFlush(display);
      }
      return (EDIT_WAIT); /*Wait for further instruction...*/
    }

    int m = strlen(filesel->filetxt) + 1;

    if (m > 1) {
      strcpy(ft, filesel->filetxt);
    } else /*We are already at the root level of file system!*/
    {
      strcpy(filesel->filetxt, "/");
      strcpy(ft, filesel->filetxt);
    }

    while ((dp = (struct dirent *)opendir(ft)) == NULL) {
      /*This WHILE is perhaps a bit clunky but since we can't be sure of
      path separator user will type in the box a trial-by-error
      approach may be more robust.*/

      ft[m] = '\0';
      m--;
      if ((ft[m] != '/') & (ft[m] != '\\')) {
        ft[m] = '\0';
        m--;
      }

      if (m == 0) {
        break;
      }
    }

    int n = 0;
    ft[0] = '\0';
    if (m > strlen(filesel->filetxt)) {
      return (EDIT_WAIT);
    }
    for (n = 0; n < strlen(filesel->filetxt) - m; n++) {
      ft[n] = filesel->filetxt[m + n + 1];
      ft[n + 1] = '\0';
    }
    strcat(ft, "*");
    strcpy(ftpath, filesel->filetxt);
    ftpath[m + 1] = '\0';
    /*Make sure we are in the correct directory now
    since user could have moved cursor back up _several_
    branches in directory tree before hitting tab key.*/
    change_directory(ftpath);
    free_finfo(&my_ff);
    filesel->n0 = 0;
    get_fileinfo_tab(ft, ftpath, &my_ff, filesel->wildtxt);
    filesel->n = my_ff.ndirs + my_ff.nfiles;
    if ((my_ff.ndirs + my_ff.nfiles) == 1) {
      if (my_ff.ndirs == 1) /*Only possible directory -- take it.*/
      {
        change_directory(my_ff.dirnames[0]);
        get_directory(cur_dir);
        redraw_directory(filesel);
        free_finfo(&my_ff); /* delete the old file info */
        filesel->n0 = 0;     /* back to the top of the list */
        get_fileinfo(filesel->wildtxt, cur_dir, &my_ff);
        filesel->n = my_ff.ndirs + my_ff.nfiles;
        strcpy(filesel->filetxt, cur_dir);
        int m = strlen(filesel->filetxt);
        if (filesel->filetxt[m - 1] != '/') {
          strcat(filesel->filetxt, "/");
        }
        redraw_fs_text(filesel, filesel->filetxt, filesel->file, 1);
      } else /*Must be that (my_ff.nfiles == 1) -- but best wait for user to
                confim they actually want this file by clicking on it.*/
      {
        /*Copy the only remaining choice into the file box and wait for user to
        make final choice.
        */
        strcpy(filesel->filetxt, cur_dir);
        strcat(filesel->filetxt, "/");
        strcat(filesel->filetxt, my_ff.filenames[0]);
        redraw_fs_text(filesel, filesel->filetxt, filesel->file, 1);
      }
    } else if (filesel->n >
               1) /*Expand the file text to most generic currently represented*/
    {
      int j = 0;
      char U[256];
      if (my_ff.ndirs > 0) {
        strcpy(U, my_ff.dirnames[0]);
      } else {
        strcpy(U, my_ff.filenames[0]);
      }

      for (j = 0; j < filesel->n; j++) {
        if (j < my_ff.ndirs) {
          stringintersect(U, my_ff.dirnames[j]);
        } else {
          stringintersect(U, my_ff.filenames[j - my_ff.ndirs]);
        }

        if (strlen(U) == 0) /*No common substring*/
        {
          break;
        }
      }
      strcpy(filesel->filetxt, ftpath);
      strcat(filesel->filetxt, U);
      /*Actually don't want to force appending of path separator here
      since we want user to decide between matching directory and files.
      */
      redraw_fs_text(filesel, filesel->filetxt, filesel->file, 1);
    }
    redraw_file_list(filesel);
    XFlush(display);
    return 0;
  }
  default:
    if ((ch >= ' ') && (ch <= '~')) {
      if (strlen(string) >= 256)
        ping();
      else {
        movmem(&string[pos + 1], &string[pos], l - pos + 1);
        string[pos] = ch;
        pos = pos + 1;
        wpos++;
        l++;
        if (wpos > mc) {
          off = off + 4;
          if (off + mc > l)
            off = l - mc;
          wpos = pos - off;
        }
      }
    }
    break;
  }
  /* all done lets save everything */
  off = pos - wpos;
  *off1 = off;
  *pos1 = pos;
  XClearWindow(display, w);
  XDrawString(display, w, small_gc, 0, CURY_OFF, string + off,
              strlen(string) - off);
  cp = DCURXs * (pos - off);
  put_edit_cursor(w, cp);
  return 0;
}

int selector_key(FILESEL *filesel, XEvent ev) {
  char ch;
  int flag;
  ch = get_key_press(&ev);
  switch (filesel->hot) {
  case HOTFILE:
    flag = edit_fitem(filesel, ch, filesel->filetxt, filesel->file, &(filesel->off),
                      &(filesel->pos), 29);
    if (flag == EDIT_DONE)
      return 1;
    if (flag == EDIT_ESC)
      return 2;
    return (0);
  case HOTWILD:
    flag = edit_fitem(filesel, ch, filesel->wildtxt, filesel->wild, &(filesel->off),
                      &(filesel->pos), 29);
    if (flag == EDIT_DONE) {
      new_wild(filesel);
      return 0;
    }
    if (flag == EDIT_ESC)
      return 2;
    return 0;
  }
  return 0;
}

void destroy_selector(FILESEL *filesel) {
  waitasec(ClickTime);
  XDestroySubwindows(display, filesel->base);
  XDestroyWindow(display, filesel->base);
  free_finfo(&my_ff);
}

int file_selector(const char *title, char *file, const char *wild) {
  FILESEL filesel;
  int i;
  if (!get_directory(cur_dir))
    return 0;
  if (!get_fileinfo(wild, cur_dir, &my_ff))
    return 0;

  create_file_selector(&filesel, title, file, wild);
  i = do_file_select_events(&filesel);
  destroy_selector(&filesel);
  XFlush(display); /*Need to do this otherwise the file dialog hangs around*/
  if (i == 0)
    return 0;
  strcpy(file, filesel.filetxt);
  return 1; /* got a file name */
}
