//! example of bad const_cast usage: (should give no compiler error but a segfault)
const int a=5;

int main()
{
  const_cast<int &>(a)=3;
}
