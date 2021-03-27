#ifndef _ESP32_TARGZ_PATHTOOLS_
#define _ESP32_TARGZ_PATHTOOLS_

/* dirname - return directory part of PATH.
   Copyright (C) 1996-2014 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@cygnus.com>, 1996.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
__attribute__((unused))
static char *dirname(char *path)
{
  static const char dot[] = ".";
  char *last_slash;
  /* Find last '/'.  */
  last_slash = path != NULL ? strrchr (path, '/') : NULL;
  if (last_slash != NULL && last_slash != path && last_slash[1] == '\0') {
    /* Determine whether all remaining characters are slashes.  */
    char *runp;
    for (runp = last_slash; runp != path; --runp)
      if (runp[-1] != '/')
        break;
    /* The '/' is the last character, we have to look further.  */
    if (runp != path)
      last_slash = (char*)memrchr(path, '/', runp - path);
  }
  if (last_slash != NULL) {
    /* Determine whether all remaining characters are slashes.  */
    char *runp;
    for (runp = last_slash; runp != path; --runp)
      if (runp[-1] != '/')
        break;
    /* Terminate the path.  */
    if (runp == path) {
      /* The last slash is the first character in the string.  We have to
          return "/".  As a special case we have to return "//" if there
          are exactly two slashes at the beginning of the string.  See
          XBD 4.10 Path Name Resolution for more information.  */
      if (last_slash == path + 1)
        ++last_slash;
      else
        last_slash = path + 1;
    } else
      last_slash = runp;
    last_slash[0] = '\0';
  } else
    /* This assignment is ill-designed but the XPG specs require to
       return a string containing "." in any case no directory part is
       found and so a static and constant string is required.  */
    path = (char *) dot;
  return path;
}

#define strdupa(a) strcpy((char*)alloca(strlen(a) + 1), a)
// create traversing directories from a path
__attribute__((unused))
static int mkpath(fs::FS *fs, char *dir)
{
  if (!dir) return 1;
  if (strlen(dir) == 1 && dir[0] == '/') return 0;
  mkpath(fs, dirname(strdupa(dir)));
  return fs->mkdir( dir );
}


// create traversing directories from a file name
__attribute__((unused))
static void mkdirp( fs::FS *fs, const char* tempFile )
{
  if( fs->exists( tempFile ) ) {
    log_v("Destination file already exists, no need to create subfolders");
    return; // no need to create folder if the file is there
  }

  char tmp_path[256] = {0};
  snprintf( tmp_path, 256, "%s", tempFile );

  for( size_t i=0;i<strlen(tmp_path);i++ ) {
    if( !isPrintable( tmp_path[i] ) ) {
      log_w("Non printable char detected in path at offset %d, setting null", i);
      tmp_path[i] = '\0';
    }
  }

  char *dir_name = dirname( tmp_path );

  if( !fs->exists( dir_name ) ) {
    log_v("Creating %s folder for path %s", dir_name, tmp_path);
    mkpath( fs, dir_name );
  } else {
    log_v("Folder %s already exists for path %s", dir_name, tmp_path);
  }
  //delete tmp_path;
}

#endif
