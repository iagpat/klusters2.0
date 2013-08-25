/***************************************************************************
                          timer.h  -  description
                             -------------------
    begin                : lun sep 22 2003
    copyright            : (C) 2003 by Lynn Hazan
    email                : lynn.hazan@myrealbox.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#if defined(Q_OS_WIN)
#include <time.h>
#include <windows.h>
#include <winsock.h>
#else
#include <sys/time.h>
#endif

#include <sys/time.h>

static struct timeval tv0;
inline void RestartTimer()
{
  struct timezone tz;
  gettimeofday(&tv0,&tz);
}

inline float Timer()
{
  struct timeval tv;
  struct timezone tz;
  gettimeofday(&tv,&tz);
  float msec = static_cast<int>(tv.tv_usec/1000)/1000.0;
  float msec0 = static_cast<int>(tv0.tv_usec/1000)/1000.0;
  float time = (tv.tv_sec+msec)-(tv0.tv_sec+msec0);
  return time;
}


#if 0
#if defined(Q_OS_WIN) && USE_MSVC_COMPILER
    #include <time.h>
    #include <sys/timeb.h>
    int gettimeofday (struct timeval *tp, void *tz)
    {
    struct _timeb timebuffer;
    _ftime (&timebuffer);
    tp->tv_sec = timebuffer.time;
    tp->tv_usec = timebuffer.millitm * 1000;
    return 0;
    }

inline void RestartTimer()
{
  struct timeval tz;
  gettimeofday(&tv0,&tz);
}

inline float Timer()
{
  struct timeval tv;
  struct timeval tz;
  gettimeofday(&tv,&tz);
  float msec = static_cast<int>(tv.tv_usec/1000)/1000.0;
  float msec0 = static_cast<int>(tv0.tv_usec/1000)/1000.0;
  float time = (tv.tv_sec+msec)-(tv0.tv_sec+msec0);
  return time;
}
#else
inline void RestartTimer()
{
  struct timezone tz;
  gettimeofday(&tv0,&tz);
}

inline float Timer()
{
  struct timeval tv;
  struct timezone tz;
  gettimeofday(&tv,&tz);
  float msec = static_cast<int>(tv.tv_usec/1000)/1000.0;
  float msec0 = static_cast<int>(tv0.tv_usec/1000)/1000.0;
  float time = (tv.tv_sec+msec)-(tv0.tv_sec+msec0);
  return time;
}

#endif
#endif

