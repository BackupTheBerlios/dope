#include "timestamp.h"
#include <sys/time.h> // timeval
#include <unistd.h>

#if defined(__WIN32__) || defined(WIN32)

// need timeval

#define __USE_W32_SOCKETS
#include <windows.h>

#endif

TimeStamp::TimeStamp(float sec)
{
  m_sec=int(sec);
  sec-=float(m_sec);
  m_usec=int(sec*1000000);
}

void TimeStamp::now()
{
#ifdef HAVE_GETTIMEOFDAY
  timeval s;
  gettimeofday(&s,NULL);
  m_sec=s.tv_sec;
  m_usec=s.tv_usec;
#elsif defined (__WIN32__) || defined(WIN32)
  unsigned long n=GetTickCount();
  m_sec=n/1000;
  n-=m_sec*1000;
  m_usec=n*1000;
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
  timeval s;
  s.tv_sec=m_sec;
  s.tv_usec=m_usec;
  // todo: on linux we could use the timeout paramter if select was interrupted by a signal
  if ((select(0,0,0,0,&s)<0)&&(errno!=EINTR))
    DOPE_FATAL("select failed");
}

