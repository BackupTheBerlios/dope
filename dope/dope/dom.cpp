#include "dom.h"

std::ostream &operator<<(std::ostream &ost,const DOM &d)
{
  return d.writeto(ost);
}

DOM::DOM(const char *href=NULL,const char *prefix=NULL) : ns(NULL), root(NULL),cnode(NULL), cns(NULL), cstate(0) {
  doc = xmlNewDoc((xmlChar *)"1.0");
  if (href)
    nshref=href;
  if (prefix)
    nspref=prefix;
}
/* create new xml document from buffer */
DOM::DOM(char *buffer,int size,const char *href) : cstate(0) {
  // todo check for errors (once again the error in constructor problem)
  doc=xmlParseMemory(buffer,size);
  DOPE_ASSERT(doc);
  cnode=root=xmlDocGetRootElement(doc);
  DOPE_ASSERT(root);
  cns = ns = xmlSearchNsByHref(doc,root,(const xmlChar *)href);
  DOPE_ASSERT(ns);
  if (href)
    nshref=href;
}
/* create new xml document from file */
DOM::DOM(const char *name) : cstate(0) {
  doc=xmlParseFile(name);
  DOPE_ASSERT(doc);
  cnode=root=xmlDocGetRootElement(doc);
  DOPE_ASSERT(root);
  cns = ns = root->ns; // xmlSearchNs(doc,root,NULL);
  DOPE_ASSERT(ns);
}

DOM::~DOM() {
  xmlFreeDoc(doc);
}
std::ostream &DOM::writeto(std::ostream &ost) const {
  xmlChar *buffer=NULL;
  int buffersize=0;
  xmlDocDumpMemory(doc,&buffer,&buffersize);
  ost.write((char *)buffer,buffersize);
  free(buffer);
  return ost;
}

void DOM::NewChild(const char * tagname, const char * thisname) {
  if (!cnode) {
    // no root yet
    root=cnode=xmlNewDocNode(doc,NULL,(xmlChar *)tagname,NULL);
    DOPE_ASSERT(root);
    xmlDocSetRootElement(doc,root);
    if (!nshref.empty()) {
      DOPE_ASSERT(!nshref.empty());
      DOPE_ASSERT(!nspref.empty());
      cns=ns=xmlNewNs(root,(const xmlChar *)nshref.c_str(),(const xmlChar *)nspref.c_str());
      xmlSetNs(root,ns);
    }
  }
  else
    cnode=xmlNewChild(cnode,cns,(xmlChar *)tagname,(xmlChar *)NULL);
  if ((thisname)&&(strcmp(thisname,"")))  setProp("this.name",thisname);
}

