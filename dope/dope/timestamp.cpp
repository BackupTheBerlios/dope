#include "timestamp.h"

#ifdef HAVE_CONFIG_H
#include <dopeconfig.h>
#endif


#include <sys/time.h> // timeval
#include <unistd.h>

#if defined(__WIN32__) || defined(WIN32)

// need timeval

#define __USE_W32_SOCKETS
#include <windows.h>
#define WINDOOF 1

static int initialized=0;

#endif

TimeStamp::TimeStamp() 
  : m_stamp(0)
{
  init();
}

TimeStamp::TimeStamp(int sec, int usec)
{
  m_stamp=1000000LL*sec+usec;
  init();
}

TimeStamp::TimeStamp(float sec)
{
  // todo - either remove the float cons completely or do it right - man llrintf
  m_stamp=(sec*1000000);
  init();
}

TimeStamp::TimeStamp(const TimeStamp &o)
  : m_stamp(o.m_stamp)
{
  init();
}

TimeStamp::~TimeStamp()
{
  deinit();
}

void
TimeStamp::init()
{
#ifdef WINDOOF
  if (initialized++)
    return;
  timeBeginPeriod(1);
#endif
}

void
TimeStamp::deinit()
{
#ifdef WINDOOF
  DOPE_CHECK(initialized);
  if (--initialized)
    return;
  timeEndPeriod(1);
#endif
}


void TimeStamp::now()
{
  // todo use stdint.h and int64_t conforming to C99
  typedef long long d_int64_t;
#if defined(HAVE_GETTIMEOFDAY)
  timeval s;
  gettimeofday(&s,NULL);
  m_stamp=d_int64_t(s.tv_sec)*1000000LL+s.tv_usec;
#else
#if defined (__WIN32__) || defined(WIN32)
  // todo GetTickCount has bad resoultion
  // either use the pentium time register or
  // do it like SDL does
  //  unsigned long n=GetTickCount();
  unsigned long n=timeGetTime();
  m_stamp=d_int64_t(n)*1000LL;
#else
#error "you must implement this"
#endif
#endif

}

TimeStamp TimeStamp::operator-(const TimeStamp &o) const
{
  TimeStamp r(*this);
  return r-=o;
}

TimeStamp &TimeStamp::operator-=(const TimeStamp &o)
{
  m_stamp-=o.m_stamp;
  return *this;
}

TimeStamp TimeStamp::operator+(const TimeStamp &o) const
{
  TimeStamp r(*this);
  return r+=o;
}

TimeStamp &TimeStamp::operator+=(const TimeStamp &o)
{
  m_stamp+=o.m_stamp;
  return *this;
}

void TimeStamp::sleep() const
{
  if (m_stamp<=0) return;
#ifndef WINDOOF
  fd_set z;
  FD_ZERO(&z);
  timeval s;
  s.tv_sec=getSec();
  s.tv_usec=getUSec();
  // todo: 
  // could be interrupted by a signal !!
  // on linux we could use the timeout paramter if select was interrupted by a signal
  int ret;
  if (((ret=select(1,&z,&z,&z,&s))==-1)&&(errno!=EINTR)) {
    switch (errno) {
    case EBADF:
      DOPE_FATAL("select failed: invalid fd");
      break;
    case EINVAL:
      DOPE_FATAL("select failed: impossible");
      break;
    case ENOMEM:
      DOPE_FATAL("select failed: out of memory");
      break;
    default:
      DOPE_WARN("select failed with unknown reason");
    }
  }
#else
  unsigned msec=m_stamp/1000;
  Sleep(msec);
#endif  
}

