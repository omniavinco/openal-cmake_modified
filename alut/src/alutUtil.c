#include "alutInternal.h"

#if HAVE_NANOSLEEP && HAVE_TIME_H
#include <time.h>
#include <errno.h>
#elif HAVE_USLEEP && HAVE_UNISTD_H
#include <unistd.h>
#elif HAVE_SLEEP && HAVE_WINDOWS_H
#include <windows.h>
#else
#error No way to sleep on this platform
#endif

ALboolean
alutSleep (ALfloat duration)
{
  if (duration < 0)
    {
      _alutSetError (ALUT_ERROR_INVALID_VALUE);
      return AL_FALSE;
    }

  {
    ALuint seconds = (ALuint) duration;
    ALfloat rest = duration - (ALfloat) seconds;

#if HAVE_NANOSLEEP && HAVE_TIME_H

    ALuint microSecs = (ALuint) (rest * 1000000);
    struct timespec t, remainingTime;
    t.tv_sec = (time_t) seconds;
    t.tv_nsec = ((long) microSecs) * 1000;

    /* At least the interaction of nanosleep and signals is specified! */
    while (nanosleep (&t, &remainingTime) < 0)
      {
        if (errno != EINTR)
          {
            return AL_FALSE;
          }
        /* If we received a signal, let's try again with the remaining time. */
        t.tv_sec = remainingTime.tv_sec;
        t.tv_nsec = remainingTime.tv_nsec;
      }

#elif HAVE_USLEEP && HAVE_UNISTD_H

    while (seconds > 0)
      {
        usleep (1000000);
        seconds--;
      }
    usleep ((unsigned int) (rest * 1000000));

#elif HAVE_SLEEP && HAVE_WINDOWS_H

    while (seconds > 0)
      {
        Sleep (1000);
        seconds--;
      }
    Sleep ((DWORD) (rest * 1000));

#endif

  }
  return AL_TRUE;
}

ALvoid *
_alutMalloc (size_t size)
{
  ALvoid *ptr = malloc (size == 0 ? 1 : size);
  if (ptr == NULL)
    {
      _alutSetError (ALUT_ERROR_OUT_OF_MEMORY);
    }
  return ptr;
}
