// grrr #warning kann man nicht mit einem macro wrappen

#define COMPILER_WARNING(msg) #warning msg
int main()
{
  COMPILER_WARNING(this is a warning)
}
