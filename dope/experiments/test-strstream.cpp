#include <iostream>
#include <strstream>
#include <string>

void main()
 {
	stringbuf s;
	ostream o(&s);
	o << "hello\n";
}
