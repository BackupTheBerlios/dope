#ifndef DOPE_SAXWRAPPER_H
#define DOPE_SAXWRAPPER_H

#include <libxml/parser.h>
#include <string.h>
#include "dope.h"
#include "dopeexcept.h"

#define DOPE_CAST(NEWTYPE,VAR) ((NEWTYPE)VAR)



//! wrap xml parser with sax interface
/* in the moment only tested with libxml2 (a.k.a. libgnome-xml) 

   requirements to template parameter X: (concept)
   1. must have a membertype Layer0 (corresponding to the layer0 Concept => the basic_streambuf concept
   2. must have a membertype Exception
   3. X::Exception type must have a Exception::Exception(std::string)
   4. following member methods must exist:
   4.1 X::setError(X::Exception)
   4.2 X::startElement(const char*, const char **)
   4.3 X::endElement(const char*)
   4.4 X::characters(const char* ,int)
   todo:
   we should store our exceptions and don't use X to do this => requires X to be "exception safe"
*/
template <typename X>
class SAXWrapper
{
protected:
  typedef typename X::Exception Exception;
  
  X &x;
  typename X::Layer0 &layer0;

  // callback struct
  // could be made const static (but perhaps in the future someone wants to change the callbacks during parsing)
  xmlSAXHandler handler;
  xmlParserCtxtPtr ctxt;
  int currentDepth;
  bool stop;
  
#ifndef NDEBUG
  static bool xmlInited;

  //! map malloc/free/realloc/strdup to C++ replacements for debugging purposes
  /*! Why is that complicated ?
    Because realloc needs to know the size of the allocated memory
    => we store the size in front of the memory area we report as allocated
  */

  //! map xmlFree to c++ delete[]
  static void m_free(void *ptr)
  {
    char *c=(char *)(((size_t *)ptr)-1);
    delete [] c;
  }

  //! map xmlMalloc to c++ new[]
  static void *m_malloc(size_t s)
  {
    char *m=new char[s+sizeof(size_t)];
    size_t *i=(size_t *)m;
    i[0]=s;
    void *r=(void *)(i+1);
    return r;
  }
  //! map xmlRealloc to c++ new[]/delete[] and copy
  static void* m_realloc(void *ptr,size_t s)
  {
    size_t *i=((size_t *)ptr)-1;
    size_t os=i[0];
    if (s==0) {
      m_free(ptr);
      return NULL;
    }
    if (s<=os)
      return ptr;
    void *n=m_malloc(s);
    memcpy((char *)n,(char *)ptr,os);
    m_free(ptr);
    return n;
  }
  //! map strdup to m_malloc and copy
  static char* m_strdup(const char *s)
  {
    size_t l(std::strlen(s)+1);
    char *r=(char *)m_malloc(l);
    memcpy(r,s,l);
    return r;
  }
#endif  

public:
  SAXWrapper(X &_x, typename X::Layer0 &_layer0) 
    : x(_x), layer0(_layer0), ctxt(NULL), currentDepth(0), stop(false)
  {
#if 0
    // todo i know havve a second stream which uses libxml => xmlMemSetup must be split out of the sax wrapper
    // tell libxml to use our memory functions
    if (!xmlInited) {
      xmlMemSetup(&SAXWrapper<X>::m_free,&SAXWrapper<X>::m_malloc,&SAXWrapper<X>::m_realloc,&SAXWrapper<X>::m_strdup);
    }
#endif
    
    /* this is (in the moment?) the complete xmlSAXHandler structure
       internalSubsetSAXFunc internalSubset;
       isStandaloneSAXFunc isStandalone;
       hasInternalSubsetSAXFunc hasInternalSubset;
       hasExternalSubsetSAXFunc hasExternalSubset;
       resolveEntitySAXFunc resolveEntity;
       getEntitySAXFunc getEntity;
       entityDeclSAXFunc entityDecl;
       notationDeclSAXFunc notationDecl;
       attributeDeclSAXFunc attributeDecl;
       elementDeclSAXFunc elementDecl;
       unparsedEntityDeclSAXFunc unparsedEntityDecl;
       setDocumentLocatorSAXFunc setDocumentLocator;
       startDocumentSAXFunc startDocument;
       endDocumentSAXFunc endDocument;
       startElementSAXFunc startElement;
       endElementSAXFunc endElement;
       referenceSAXFunc reference;
       charactersSAXFunc characters;
       ignorableWhitespaceSAXFunc ignorableWhitespace;
       processingInstructionSAXFunc processingInstruction;
       commentSAXFunc comment;
       warningSAXFunc warning;
       errorSAXFunc error;
       fatalErrorSAXFunc fatalError;
       getParameterEntitySAXFunc getParameterEntity;
       cdataBlockSAXFunc cdataBlock;
       externalSubsetSAXFunc externalSubset;
       int initialized;
    */

    // set all to NULL
    memset((void *)&handler,0,sizeof(handler));

    // register some callbacks
    handler.startElement=&SAXWrapper<X>::startElement;
    handler.endElement=&SAXWrapper<X>::endElement;
    handler.characters=&SAXWrapper<X>::characters;
    handler.getEntity=&SAXWrapper<X>::getEntity;
    handler.getParameterEntity=&SAXWrapper<X>::getParameterEntity;
    handler.error=&SAXWrapper<X>::error;
    handler.fatalError=&SAXWrapper<X>::fatalError;

    // some default callbacks
    handler.ignorableWhitespace=ignorableWhitespace;

    // entities in the moment are simply replaced
    // todo ATTENTION: this is a global variable of libxml !!!
    xmlSubstituteEntitiesDefaultValue=1;
  }

  //! start parsing from layer0 stream
  /*!
    returns 0 if document was wellformed -1 on error
  */
  int parse()
  {
    int ret = 0;

    ctxt = xmlCreateIOParserCtxt(&handler,(void *)this,&SAXWrapper<X>::readCallback,NULL,(void *)this,XML_CHAR_ENCODING_NONE);
    if (ctxt == NULL) return -1;
    xmlParseDocument(ctxt);

    if (ctxt->wellFormed)
      ret = 0;
    else
      ret = -1;
    xmlFreeParserCtxt(ctxt);
    ctxt=NULL;
    xmlCleanupParser();
    return ret;
  }

  /* map to our non static members */

  static void startElement(void *user_data, const xmlChar *name, const xmlChar **atts)
  {
    thisCast(user_data)->startElement(name,atts);
  }
  static void endElement(void *user_data, const xmlChar *name)
  {
    thisCast(user_data)->endElement(name);
  }
  static void characters(void *user_data, const xmlChar *s, int len)
  {
    thisCast(user_data)->x.characters(DOPE_CAST(const char *,s),len);
  }
  //! should act like read (man 2 read)
  static int readCallback(void *user_data, char *buffer, int len)
  {
    return thisCast(user_data)->read(buffer,len);
  }

  
  /* plain wrappers calling libxml functions */
  static xmlEntityPtr getEntity(void *user_data, const xmlChar *e)
  {
    return ::getEntity(thisCast(user_data)->ctxt,e);
  }
  static xmlEntityPtr getParameterEntity(void *user_data, const xmlChar *e)
  {
    return ::getParameterEntity(thisCast(user_data)->ctxt,e);
  }
  static xmlParserInputPtr resolveEntity(void *user_data, const xmlChar *publicId, const xmlChar *systemId)
  {
    return ::resolveEntity(thisCast(user_data)->ctxt,publicId,systemId);
  }
  
  /* mixture of both - wrapper and member mapper */
  static void error(void *user_data, const char *e, ...)
  {
    xmlParserError(thisCast(user_data)->ctxt, e);
    assert(e);
    thisCast(user_data)->throwLater(Exception(e));
  }

  static void fatalError(void *user_data, const char *e, ...)
  {
    xmlParserError(thisCast(user_data)->ctxt, e);
    assert(e);
    thisCast(user_data)->throwLater(Exception(e));
  }
  
protected:
  static SAXWrapper<X> *thisCast(void *user_data)
  {
    // todo perhaps add some type checking
    SAXWrapper<X> *me=DOPE_CAST(SAXWrapper<X> *,user_data);
    assert(me);
    return me;
  }

  /* map to methods of X */

  void startElement(const xmlChar *name, const xmlChar **atts)
  {
    ++currentDepth;
    x.startElement(DOPE_CAST(const char *,name),DOPE_CAST(const char **,atts));
  }

  void endElement(const xmlChar *name)
  {
    if (!(--currentDepth)) {
      // just read end tag of top node - put back characters not needed

      /* todo improve this
	 the problem is that by default the libxml parser reads beyond the end of the top node to read
	 => you can't have more than one top node in one stream which is normally nice since xml documents
	 have only one top node - but if you stream more than one xml document f.e. via a network stream
	 this is very bad

	 now what is my solution in the moment ?
	 if we read the endtag of the top node we put back the characters we don't need
	 => we have to count the node depth and we must access the libxml internal stream
	 
	 1. get access to the inputstream
	 we assume that our input stream is the only one and current
	 we assume that one character is one byte
      */
      assert(ctxt->input);
      assert(ctxt->inputNr==1);
      assert(ctxt->input->base);
      assert(ctxt->input->cur);
      assert(ctxt->input->end);
      assert(ctxt->input->end>=ctxt->input->cur);

      // put back all characters not consumed
      char *cptr=(char *)ctxt->input->end;
      --cptr;
      while (cptr>=(char *)ctxt->input->cur)
	{
	  if (layer0.sputbackc(*cptr)==std::char_traits<char>::eof())
	    {
	      throwLater(Exception(std::string(__PRETTY_FUNCTION__)+"failed to put back character into stream - this means you have to wrap the underlying stream with a stream which is capable to putback any number of characters"));
	      break;
	    }
	  --cptr;
	}

      if (!stop) {
	// debug help
	assert(ctxt->input->end>=ctxt->input->cur);
	unsigned c=ctxt->input->end-ctxt->input->cur;
	unsigned ts=20;
	if (c<20) ts=c;
	// compare the fist ts bytes - if they really have been put back
	char *p=new char[ts+1];
	char* p2=new char[ts+1];
	memcpy(p,(char *)ctxt->input->end-c,ts);
	p[ts]=0;
	layer0.sgetn(p2,ts);
	p2[ts]=0;
	assert(!strcmp(p,p2));
	while (ts--)
	  layer0.sputbackc(p[ts]);
	delete [] p;
	delete [] p2;
	// debug help end
	
	stopParser();
      }
      
    }
    x.endElement(DOPE_CAST(const char *,name));
  }

  //! should act like read (man 2 read)
  int read(char *buffer, int len)
  {
    if (stop)
      return 0;
    int r=layer0.sgetn(buffer,len);
    // todo
    // how to detect eof/errors ? - i think the streambuf should throw an exception but it doesn't
    // if we read 0 bytes - we read one more and check for eof => we can detect eof
    return r;
  }

  //! end of top node reached => stop parser now !
  /*! attention as side effect ctxt is freed */
  void stopParser()
  {
    stop=true;
    xmlStopParser(ctxt);
  }

  //! store exception to throw it later
  /*! 
    you can't throw exception through C code => we store it until we leave the C code 
    Attention: as side effect the parser is stopped
  */
  void throwLater(const Exception &e)
  {
    stopParser();
    x.setError(e);
  }
};

#ifndef NDEBUG
template <typename X>
bool SAXWrapper<X>::xmlInited=false;
#endif


//! Parser base class - to make it easy
template <typename L0>
class ParserBase
{
public:
  typedef L0 Layer0;
  typedef ReadError Exception;

  ParserBase(Layer0 &_layer0) : layer0(_layer0) {}
  virtual ~ParserBase() {}

  virtual void setError(const ReadError &e)
  {
    errorPtr=DOPE_SMARTPTR<ReadError>(new ReadError(e));
  }
  virtual void startElement(const char*, const char **)=0;
  virtual void endElement(const char*)=0;
  virtual void characters(const char*, int)=0;
  virtual int parse()
  {
    SAXWrapper<ParserBase<Layer0> > sax(*this,layer0);
    int ret=sax.parse();
    if (errorPtr.get())
      throw *(errorPtr.get());
    if (ret!=0)
      throw ReadError("xml document was not well-formed");
    return ret;
  }
protected:
  Layer0 &layer0;
  DOPE_SMARTPTR<ReadError> errorPtr;
};

#undef DOPE_CAST

#endif
