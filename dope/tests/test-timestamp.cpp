#include <dope/timestamp.h>

int main()
{
  TimeStamp start;
  TimeStamp newTime;
  TimeStamp dt;
  start.now();
  while (1) {
    newTime.now();
    dt=newTime-start;
    std::cout << dt.getSec() << "sec " << dt.getUSec() << "usec\n";
    if (dt.getSec()>3) return 0;
  }
  return 1;
}
