/*
 * Copyright (C) 2002 Jens Thiele <jens.thiele@student.uni-tuebingen.de>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*!
   \file xmlsaxinstream.h
   \brief layer2 xml input stream using the sax interface
   \author Jens Thiele
   \note this needs a complete rewrite !!!
*/

#ifndef DOPE_XMLINSTREAM_H
#define DOPE_XMLINSTREAM_H

#include <iterator>
#include <stdexcept>
#include <stack>
#include <boost/smart_ptr.hpp>

#include "streamtraits.h"
#include "utils.h"
#include "typenames.h"
#include "saxwrapper.h"
#include "dopeexcept.h"
#include "xml.h"

#define TYPE_NAME(type) TypeNameTrait<type>::name()

//! Layer 2 xml input stream (using sax parser interface)
/*! this one is a bit tricky
  since we use sax reading is done via callbacks
  => we are called and should set a specific class member
  but we can't access members directly we only can forward iterate over the class members
  => we iterate until we get a match - if it is a compound we push a mark on a stack
  now we return from our callback
  => we get called again with the members of the compound
  => we iterate until we reach our mark - dig in and start from the beginning
  
  special cases: containers

  perhaps look at the problem from the other side
  we write with the help of sax into a class
  perhaps this would be a good idea for the whole library
  there is no read/write only a transform
  perhaps take a look again what i did earlier (xmlclassrw?) with extended runtime type info
  but this raises following problem:

  \bug because we call the composite function more than once a method to read like the one below
  will fail and if the Mesh class in the example has a container it will even segfault:

  template <typename Layer2>
  void Foo::composite(Layer2 &layer2, True)
  {
    Mesh m;
    layer2.simple(m,NULL);
    setFromMesh(m);
  }

  => if you write your composite function you must ensure that it is statefull - it can be called more
  than once setting only one/some members - i will call this "reentrant"
  unfortunately you don't know when reading is finished => impossible to workaround
  => you would have to use class members - and the one above would call setFromMesh more than
  once and would call it with a incomplete mesh

  The problem in short:
  - what is a composite function allowed to do ?
  - how much information can it request from the stream ?
  - may a composite function be called more than once in the way this 
    stream does it ? (should it be "reentrant" ?)
    (there will be other "streams"/usages with the same problem: the data arrives in a different
   order than we need it (in our case only the xml attributes => i could rewrite it without the need
   for "reentrant" composite functions?)

  This leads to those 2 opposite goals:
  - the more ::composite functions should be allowed to do the less a stream can do
  - the more a stream should do the less ::composite functions are allowed to do

  \todo 
  perhaps needs a complete rewrite - the requirement stated above make this stream useless.
  Ideas for a complete rewrite:
  xml tags do have a order (the same order as our members)
  xml attributes do not have a order but usually you do not have many attributes per tag

  Reading xml with sax could work like this:
  1) user calls stream.simple(x,n);
  2) if it is a composite stream tells the sax parser to get the next tag
  3) stream compares tag with composite or looks for xml attribute which matches the simple type
     if correct/found stream calls ::composite or sets the simple value otherwise step 2)
     or throw exception (we should be tolerant - because the xml "DTD" might have been extended)
  4) stream calls ::composite
  5) composite does something and then calls stream.simple(member,membername) which is step 1)

  Conclusion: this could work ? but I think i tried this already once and it didn't work ?
*/
template <typename L>
class XMLSAXInStream
{
protected:
  typedef std::string xmlString;
  typedef L Layer0;
  typedef SAXWrapper<XMLSAXInStream<Layer0> > SAXWrapperT;
  typedef SAXWrapperT* SAXWrapperTPtr;

  
  struct ReaderBase
  {
    virtual void restart()=0;
  };
  
  template <typename S, typename X>
  class Reader : public ReaderBase
  {
  protected:
    S &stream;
    X &x;
    MemberName mname;
  public:
    Reader(S &_stream, X &_x, MemberName m) : stream(_stream), x(_x), mname(m)
    {
    }
    
    void restart()
    {
      stream.simple(x,mname);
    }
  };

  struct Qualifier
  {
    Qualifier() : tname(NULL), membername(NULL), value(NULL)
    {}
    Qualifier(const char *t, const char *m, const char *v=NULL)
      :tname(t),membername(m),value(v)
    {}
    
    bool match(const Qualifier &o) const
    {
      if ((tname&&o.tname)&&(std::string(tname)!=o.tname))
	return false;
      if ((membername&&o.membername)&&(std::string(membername)!=o.membername))
	return false;
      return true;
    }
    
    void print() const
    {
      std::cerr << ( tname ? tname : "NULL") << "|" << (membername ? membername : "NULL") << "|" << (value ? value : "NULL") << std::endl;
    }
    
    const char * tname;
    const char * membername;
    const char * value;
  };

  class ContainerElemBase
  {
  public:
    typedef XMLSAXInStream<Layer0> S;
    
    ContainerElemBase(S &_s) : s(_s), insertit(false)
    {}
    
    virtual void endElementCallback()=0;
    virtual XMLSAXInStream &simple()=0;

    void activateCallback()
    {
      insertit=true;
    }

  protected:
    S &s;
    bool insertit;
  };

  template <typename C>
  class ContainerElem : public ContainerElemBase
  {
  protected:
    C &c;
    //! type of values stored in the container
    /*!
      std::map is problematic because it is std::pair<const key, value>
    */
    typename C::value_type x;

  public:
    ContainerElem(typename ContainerElemBase::S& _s, C& _c) 
      : ContainerElemBase(_s), c(_c)
    {
    }
    virtual ~ContainerElem()
    {}
    
    void endElementCallback()
    {
      // copy into container
      if (insertit) c.insert(c.end(),x);
    }
    XMLSAXInStream &simple()
    {
      return s.simple(x,NULL);
    }
  };
  

  //! not a real stack you can access any member
  class PseudoStack
  {
  protected:
    typedef std::pair<unsigned, ContainerElemBase*> value_type;
    
    std::vector<value_type> s;
    value_type lp;
  public:
    ~PseudoStack()
    {
      clear();
    }
    
    void clear()
    {
      if (!s.empty()){
	DOPE_WARN("stack was not empty ("<<s.size()<<")");
	for (size_t i=0;i<s.size();++i)
	  if (s[i].second!=NULL)
	    delete s[i].second;
      }
    }
    
    void push(unsigned u, ContainerElemBase* cb=NULL)
    {
      s.push_back(value_type(u,cb));
    }
    ContainerElemBase* getCallback(size_t e)
    {
      assert(e<s.size());
      return s[e].second;
    }
    void setCallback(ContainerElemBase* cb)
    {
      assert(!s.empty());
      s.back().second=cb;
    }
    
    unsigned pop()
    {
      assert(!s.empty());
      lp=s.back();
      s.pop_back();
      if (lp.second) {
	lp.second->endElementCallback();
	delete lp.second;
	lp.second=NULL;
      }
      return lp.first;
    }
    unsigned lastPopped()
    {
      return lp.first;
    }
    void print()
    {
      for (size_t j=0;j<s.size();++j)
	std::cerr << "-" << s[j].first;
      std::cerr << std::endl;
    }
    unsigned operator[](size_t i)
    {
      assert(i<s.size());
      return s[i].first;
    }
    size_t size()
    {
      return s.size();
    }
    bool empty()
    {
      return s.empty();
    }
    
  };

  
  Layer0 &layer0;
  SAXWrapperTPtr saxWrapperPtr;
  ReaderBase* readerPtr;
  unsigned memberNo;
  unsigned currentDepth;
  bool qualifierMatched;
  int endElementsNeeded;
  TimeStamp timeOut;
  int retries;

  PseudoStack selectedMemberStack;
  Qualifier qualifier;
  DOPE_SMARTPTR<ReadError> errorPtr;



  
public:
  XMLSAXInStream(Layer0 &_layer0, const TimeStamp &_timeOut=TimeStamp(0,0), int _retries=0) 
    : layer0(_layer0), saxWrapperPtr(NULL), readerPtr(NULL), memberNo(0), currentDepth(0),
      qualifierMatched(false),endElementsNeeded(0), timeOut(_timeOut), retries(_retries)
  {
  }

  typedef ReadError Exception;
  typedef Layer2InStreamTraits Traits;

  /*!
    \todo this is public because otherwise i got a compiler error
  */
  template <typename X>
  XMLSAXInStream &simpleHelper(X &data, MemberName mname)
  {
    // the searched member was found simple unwind the stack
    if (alreadyMatched())
      return *this;
    // start parser if not yet running
    if (init(data,mname))
      return *this;
    if (currentDepth<selectedMemberStack.size())
      {
	if (memberNo==selectedMemberStack[currentDepth])
	  {
	    // special case when simple value is encoded as tag and not as attribut
	    ++currentDepth;
	    assert(qualifier.value);
	    stringToAny(qualifier.value,data);
	    matched();
	    return *this;
	  }
	++memberNo;
	return *this;
      }
    XML::xmlString s(XML::xmlName(TYPE_NAME(X).c_str()));
    Qualifier q(s.c_str(),mname);
    if (q.match(qualifier))
      {
	if (!mname)
	  {
	    // special case when simple value is encoded as tag and not as attribut
	    selectedMemberStack.push(memberNo);
	    matched();
	    return *this;
	  }
	assert(qualifier.value);
	stringToAny(qualifier.value,data);
	matched();
	return *this;
      }
    ++memberNo;
    return *this;
  }

#define DOPE_SIMPLE(type) \
DOPE_INLINE XMLSAXInStream &simple(type &data, MemberName mname) \
  { \
    return simpleHelper(data,mname); \
  }

  DOPE_SIMPLE(bool);
  DOPE_SIMPLE(char);
  DOPE_SIMPLE(signed char);
  DOPE_SIMPLE(unsigned char);
  DOPE_SIMPLE(short);
  DOPE_SIMPLE(unsigned short);
  DOPE_SIMPLE(int);
  DOPE_SIMPLE(unsigned int);
  DOPE_SIMPLE(long);
  DOPE_SIMPLE(unsigned long);
  DOPE_SIMPLE(long long);
  DOPE_SIMPLE(unsigned long long);
  DOPE_SIMPLE(float);
  DOPE_SIMPLE(double);
  DOPE_SIMPLE(std::string);
  // todo - is this correct ?
  DOPE_SIMPLE(const char*);

  //  DOPE_SIMPLE(char* &);
  
#undef DOPE_SIMPLE

  template <typename T>
  XMLSAXInStream &ext(const T& data)
  {
    ::composite(*this,data);
    return *this;
  }


  //! simple for all types using the global composite function
  template <class T>
  DOPE_INLINE XMLSAXInStream &simple(T& data, MemberName mname) {
    if (alreadyMatched())
      return *this;
    if (init(data,mname))
      return *this;
    if (currentDepth<selectedMemberStack.size())
      {
	if (memberNo==selectedMemberStack[currentDepth])
	  {
	    memberNo=0;
	    ++currentDepth;
	    ::composite(*this,data);
	    return *this;
	  }
	++memberNo;
	return *this;
      }
    XML::xmlString s(XML::xmlName(TYPE_NAME(T).c_str()));
    Qualifier q(s.c_str(),mname);
    if (q.match(qualifier))
      {
	selectedMemberStack.push(memberNo);
	matched();
	return *this;
      }
    ++memberNo;
    return *this;
  }

  

  // map methods not named simple

  DOPE_INLINE XMLSAXInStream &dynCString(const char* data, MemberName mname) 
  {
    return simple(data);
  }
  DOPE_INLINE XMLSAXInStream &fixedCString(const char* data, size_t max, MemberName mname) 
  {
    return simple(data);
  }

  template <class T>
  DOPE_INLINE XMLSAXInStream& dynCVector(T* &data, size_t &size, MemberName mname)
  {
    if (alreadyMatched())
      return *this;

    // todo
    //    if (init(data,mname))
    //      return *this;
    if (currentDepth<selectedMemberStack.size())
      {
	if (memberNo==selectedMemberStack[currentDepth])
	  {
	    memberNo=0;
	    ++currentDepth;
	    if (currentDepth<selectedMemberStack.size()){
	      assert(selectedMemberStack[currentDepth]<size);
	      assert(data);
	      memberNo=selectedMemberStack[currentDepth];
	      return simple(data[memberNo],NULL);
	    }else{
	      // add elem to cvector
	      T* ndata=new T[size+1];
	      if (data) {
		for (unsigned i=0;i<size;++i)
		  ndata[i]=data[i];
		delete [] data;
	      }
	      data=ndata;
	      memberNo=size;
	      ++size;
	      return simple(data[size],NULL);
	    }
	  }
	++memberNo;
	return *this;
      }
    XML::xmlString s(XML::xmlName(TYPE_NAME(T[]).c_str()));
    Qualifier q(s.c_str(),mname);
    if (q.match(qualifier))
      {
	// the caller must by convention have freed the memory - otherwise it leaks
	data=NULL;
	size=0;
	selectedMemberStack.push(memberNo);
	matched();
	return *this;
      }
    ++memberNo;
    return *this;
  }
  
  //! C-vector of fixed size
  template <class T>
  DOPE_INLINE XMLSAXInStream& fixedCVector(T data[], size_t size, MemberName mname)
  {
    if (alreadyMatched())
      return *this;

    // todo
    //    if (init(data,mname))
    //      return *this;
    if (currentDepth<selectedMemberStack.size())
      {
	if (memberNo==selectedMemberStack[currentDepth])
	  {
	    memberNo=0;
	    ++currentDepth;
	    if (currentDepth<selectedMemberStack.size()){
	      assert(selectedMemberStack[currentDepth]<size);
	      assert(data);
	      memberNo=selectedMemberStack[currentDepth];
	      return simple(data[memberNo],NULL);
	    }else{
	      // select next elem (needs hack below to work)
	      memberNo=selectedMemberStack.lastPopped()+1;
	      assert(memberNo<size);
	      return simple(data[memberNo],NULL);
	    }
	  }
	++memberNo;
	return *this;
      }
    XML::xmlString s(XML::xmlName(TYPE_NAME(T[]).c_str()));
    Qualifier q(s.c_str(),mname);
    if (q.match(qualifier))
      {
	selectedMemberStack.push(memberNo);
	// hack to get lastPopped to 0 needed above
	selectedMemberStack.push((unsigned)-1);
	selectedMemberStack.pop();
	matched();
	return *this;
      }
    ++memberNo;
    return *this;
  }
  
  //! types conforming to the STL container concept
  /*! 
    this is complicated because of std::map
    we have to read the element and use a callback to insert this element into the container
  */
  template <class C>
  DOPE_INLINE XMLSAXInStream& container(C &data)
  {
    if (currentDepth<selectedMemberStack.size()){
      assert(selectedMemberStack.getCallback(currentDepth));
      return selectedMemberStack.getCallback(currentDepth)->simple();
    }else{
      // create new elem
      ContainerElem<C>* nptr=new ContainerElem<C>(*this,data);
      nptr->simple();
      if (!failed()) {
	nptr->activateCallback();
	selectedMemberStack.setCallback(nptr);
      }
      return *this;
    }
  }

  //! SAX callbacks

  //! element start
  void startElement(const char *name, const char **atts)
  {
    // start of an element
    if (endElementsNeeded>0)
      {
	// ignore this element because it is a child of an ignored element
	++endElementsNeeded;
	return;
      }
    // this means we search for a class of a type named matching name
    // and itself named like the value of the attribute "this.name"
    // or
    // a simple value ...
    const char *mname=NULL;
    const char *value=NULL;
    if (atts) {
      for (int i=0;atts[i]!=NULL;i+=2){
	assert(atts[i+1]!=NULL);
	std::string s(atts[i]);
	if (s=="this.name")
	  mname=atts[i+1];
	if (s=="value")
	  value=atts[i+1];
      }
    }
    assert(name);
    qualifier=Qualifier(name,mname,value);
    restartRead();
    if (failed())
      {
	if (!mname) mname="";
	std::cerr << "Unkown tag or name:  <"<<name << "> this.name=\"" << mname << "\"\n";
	++endElementsNeeded;
      }
    else
      {
	if (atts) {
	  for (int i=0;atts[i]!=NULL;i+=2){
	    assert(atts[i+1]!=NULL);
	    if (std::string(atts[i])=="this.name")
	      continue;
	    qualifier=Qualifier(NULL,atts[i],atts[i+1]);
	    // restart read process
	    restartRead();
	    if (failed())
	      std::cerr << "Unkown attribute: \""<<atts[i]<<"\"\n";
	  }
	}
      }
  }

  void endElement(const char *n)
  {
    if (endElementsNeeded>0)
      {
	--endElementsNeeded;
	return;
      }
    assert(!selectedMemberStack.empty());
    selectedMemberStack.pop();
  }
  
  //! element content
  void characters(const char* s, int len)
  {
    // we do not encode anything in characters
    return;
  }

  void setError(const ReadError &e)
  {
    errorPtr=DOPE_SMARTPTR<ReadError>(new ReadError(e));
  }
  
protected:
  bool failed() const
  {
    return !qualifierMatched;
  }
  bool alreadyMatched() const
  {
    return qualifierMatched;
  }
  
  void matched()
  {
    qualifierMatched=true;
  }
  
  void prepareRestart()
  {
    memberNo=currentDepth=0;
    qualifierMatched=false;
  }
  
  void restartRead()
  {
    prepareRestart();
    assert(readerPtr);
    readerPtr->restart();
  }
  
  /*
    template <typename I>
    XMLSAXInStream& readContainer(I begin, I end)
    {
    return *this;
    }
  */

  //! start parser if not yet running
  template <typename X>
  bool init(X &x, MemberName mname)
  {
    if (!saxWrapperPtr)
      {
	// we have to remember how to start the read process
	readerPtr=new Reader<XMLSAXInStream<L>, X>(*this,x,mname);
	startParser();
	return true;
      }
    return false;
  }

  inline void startParser()
  {
    saxWrapperPtr=new SAXWrapperT(*this,layer0,timeOut,retries);
    int res=saxWrapperPtr->parse();
    // parsing finished
    delete saxWrapperPtr;
    saxWrapperPtr=NULL;
    delete readerPtr;
    readerPtr=NULL;
    qualifierMatched=false;
    prepareRestart();
    endElementsNeeded=0;
    selectedMemberStack.clear();
    if (errorPtr.get())
      throw *(errorPtr.get());
    if (res<0)
      throw ReadError("XML Parser error");
  }
  
};

#undef TYPE_NAME

#endif
