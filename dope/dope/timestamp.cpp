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
  : m_sec(0), m_usec(0) 
{
  init();
}

TimeStamp::TimeStamp(int sec, int usec)
  : m_sec(sec), m_usec(usec) 
{
  init();
}

TimeStamp::TimeStamp(float sec)
{
  m_sec=int(sec);
  sec-=float(m_sec);
  m_usec=int(sec*1000000);
  init();
}

TimeStamp::TimeStamp(const TimeStamp &o)
  : m_sec(o.m_sec), m_usec(o.m_usec)
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
#if defined(HAVE_GETTIMEOFDAY)
  timeval s;
  gettimeofday(&s,NULL);
  m_sec=s.tv_sec;
  m_usec=s.tv_usec;
#else
#if defined (__WIN32__) || defined(WIN32)
  // todo GetTickCount has bad resoultion
  // either use the pentium time register or
  // do it like SDL does
  //  unsigned long n=GetTickCount();
  unsigned long n=timeGetTime();
  m_sec=n/1000;
  n-=m_sec*1000;
  m_usec=n*1000;
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
  // modified version of timeval_subtract from info libc
  // \todo is this really good ?
  timeval x,y;
  x.tv_sec=m_sec;
  x.tv_usec=m_usec;
  y.tv_sec=o.m_sec;
  y.tv_usec=o.m_usec;
  
  if (x.tv_usec < y.tv_usec) {
    int nsec = (y.tv_usec - x.tv_usec) / 1000000 + 1;
    y.tv_usec -= 1000000 * nsec;
    y.tv_sec += nsec;
  }
  if (x.tv_usec - y.tv_usec > 1000000) {
    int nsec = (x.tv_usec - y.tv_usec) / 1000000;
    y.tv_usec += 1000000 * nsec;
    y.tv_sec -= nsec;
  }

  assert(x.tv_sec>=y.tv_sec);  
  m_sec = x.tv_sec - y.tv_sec;
  m_usec = x.tv_usec - y.tv_usec;
  return *this;
}

TimeStamp TimeStamp::operator+(const TimeStamp &o) const
{
  TimeStamp r(*this);
  return r+=o;
}

TimeStamp &TimeStamp::operator+=(const TimeStamp &o)
{
  int usec=m_usec+o.m_usec;
  m_usec=usec%1000000;
  m_sec+=(usec-m_usec)/1000000+o.m_sec;
  return *this;
}

void TimeStamp::sleep() const
{
#ifndef WINDOOF
  fd_set z;
  FD_ZERO(&z);
  timeval s;
  s.tv_sec=m_sec;
  s.tv_usec=m_usec;
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
  unsigned msec=m_sec*1000;
  msec+=m_usec/1000;
  Sleep(msec);
#endif  
}

