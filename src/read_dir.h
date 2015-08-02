#ifndef XPPAUT_READ_DIR_H
#define XPPAUT_READ_DIR_H

#include "xpplim.h"

/* --- Macros --- */
/* Let's try to be consistent with file name buffer sizes and any strings that
 * may hold a path name (e.g. dialog message etc.)
 */
#define MAXPATHLEN XPP_MAX_NAME

/* --- Types --- */
typedef struct {
  char **dirnames, **filenames;
  int nfiles, ndirs;
} FILEINFO;

/* --- Data --- */
extern char cur_dir[MAXPATHLEN];
extern FILEINFO my_ff;

/* --- Functions --- */
int change_directory(const char *path);
void free_finfo(FILEINFO *ff);
int get_directory(char *direct);
int get_fileinfo(const char *wild, const char *direct, FILEINFO *ff);
int get_fileinfo_tab(const char *wild, const char *direct, FILEINFO *ff, const char *wild2);

#endif /* XPPAUT_READ_DIR_H */
