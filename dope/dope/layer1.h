/*! Concept of Layer1 streams 

  This isn't code to be compiled - it is just to document the concept
*/

class Layer1InStream
{
  Layer1InStream &in(bool &data);
  Layer1InStream &in(char &data);
  Layer1InStream &in(unsigned char &data);
  Layer1InStream &in(short &data);
  Layer1InStream &in(unsigned short &data);
  Layer1InStream &in(int &data);
  Layer1InStream &in(unsigned int &data);
  Layer1InStream &in(long &data);
  Layer1InStream &in(unsigned long &data);
  Layer1InStream &in(float &data);
  Layer1InStream &in(double &data);
  //! read c bytes
  Layer1InStream &in(char *data, size_t c);
};
class Layer1OutStream
{
  Layer1OutStream &out(bool data);
  Layer1OutStream &out(char data);
  Layer1OutStream &out(unsigned char data);
  Layer1OutStream &out(short data);
  Layer1OutStream &out(unsigned short data);
  Layer1OutStream &out(int data);
  Layer1OutStream &out(unsigned int data);
  Layer1OutStream &out(long data);
  Layer1OutStream &out(unsigned long data);
  Layer1OutStream &out(float data);
  Layer1OutStream &out(double data);
  //! write c bytes
  Layer1OutStream &out(const char * const data, size_t c);
};
