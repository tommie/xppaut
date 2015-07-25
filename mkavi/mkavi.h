/*
 *
 * Copyright (C) 2015 by Tommie Gannert.
 * All rights reserved.
 *
 * This software may be freely copied, modified and redistributed without
 * fee for non-commerical purposes provided that this copyright notice is
 * preserved intact on all copies and modified copies.
 *
 * There is no warranty or other guarantee of fitness of this software.
 * It is provided solely "as is". The author(s) disclaim(s) all
 * responsibility and liability with respect to this software's usage
 * or its effect upon hardware, computer systems, other software, or
 * anything else.
 *
 */


#ifndef MKAVI_INCLUDED
#define MKAVI_INCLUDED

int mkavi(int nframes, int fps, int w, int h,
          int i, int task, unsigned char *image);

#endif /* MKAVI_INCLUDED */
