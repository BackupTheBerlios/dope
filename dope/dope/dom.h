/*
 * Copyright (C) 2001 Jens Thiele <karme@unforgettable.com>
 *  
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *  
 * file <dope/xmldomtree.h> of project <DOPE>
 */   

/** \file dom.h
 *  \brief DOM wrapper class
 *
 *  \author Jens Thiele
 */

#ifndef DOPE_DOM_H
#define DOPE_DOM_H

#include <ios>
#include <iostream>
#include <libxml/parser.h>
#include <string>
#include "dope.h"

//! xml DOM-tree wrapper
/*! 
  this implementation uses libxml a.k.a libgnome-xml 
*/
class DOM {
private:
  //! copy constructor not supported 
  DOM(const DOM &o);
  void operator=(const DOM &o);
  
public:
  typedef xmlNodePtr nodeType;
  
  typedef int state;
  enum state_bit {
    goodbit = std::ios::goodbit,
    eofbit = std::ios::eofbit,
    failbit = std::ios::failbit,
    badbit = std::ios::badbit
  };
  //! create a new empty document 
  /*!       
    href   the URI associated with the namspace
    prefix the prefix for the namespace
  */
  DOM(const char *href,const char *prefix);
  //! create new document from buffer 
  /*!
    buffer the pointer to the xml character buffer
    size   the size of the buffer
    href   the URI associated with the namespace
  */
  DOM(char *buffer,int size,const char *href);
  //! create xml document from a file/uri 
  /*!
    name   filename
  */
  DOM(const char *name);

  ~DOM();
  //! returns root node of document
  xmlNodePtr getroot() {return root;}
  //! get namespace of document root
  xmlNsPtr getns(){return ns;}
  //! returns current node in document
  xmlNodePtr getcnode() {return cnode;}
  //! get namespace of current node
  xmlNsPtr getcns(){return cns;}
  //! set current node in document
  xmlNodePtr setcnode(xmlNodePtr n) {assert(n);return (cnode=n);}
  //! go to next node (returns NULL if there is none - and doesn't change current node then)
  xmlNodePtr nextNode() {if (cnode->next) return (cnode=cnode->next);return NULL;}
  //! go to previous node (returns NULL if there is none - and doesn't change current node then)
  xmlNodePtr prevNode() {if (cnode->prev) return (cnode=cnode->prev);return NULL;}
  //! go one node up (returns NULL if there is none - and doesn't change current node then)
  xmlNodePtr nodeUp() {if (cnode->parent) return (cnode=cnode->parent);return NULL;}
  //! go one node down (returns NULL if there is none - and doesn't change current node then)
  xmlNodePtr nodeDown() {if (cnode->children) return (cnode=cnode->children);return NULL;}
  //! get first child node
   xmlNodePtr getFirstChild() const 
   {
      return cnode->children;
   }
  //! create new child for current node - and go to this node
  void NewChild(const char * tagname, const char * thisname=NULL);
  //! set property for current node
  void setProp(const char *attr, const char *value) {assert(attr);assert(value);assert(cnode);xmlSetProp(cnode,(xmlChar *)attr,(xmlChar *)value);}
  //! check for property
  bool hasProp(const char *attr) {return xmlGetProp(cnode,(xmlChar *)attr)!=NULL;}
  //! get property for current node (returns NULL if attribute doesn't exist)
  std::string getProp(const char *attr) {
    assert(attr);assert(cnode);
    char* prop=(char *)xmlGetProp(cnode,(xmlChar *)attr);
    if (!prop)
      throw std::string("There is no property with name: \"")+attr+"\"";
    std::string result(prop);
    free(prop);
    return result;
  }
  //! get content of current node
  std::string getContent() const {
    assert(cnode);
    std::string c;
    char *t=(char *)xmlNodeGetContent(cnode);
    if (t) {
      c=t;
      free (t);
    }else
      DOPE_WARN("xmlNodeGetContent returned NULL");
    return c;
  }
  //! get node name
  std::string getNodeName() const {assert(cnode); assert(cnode->name); return (const char *)cnode->name;}
  //! set content of current node
  void setContent(const std::string &c) {assert(cnode);xmlNodeSetContentLen(cnode,(const xmlChar *)c.c_str(),c.size());}
  //! get number of children for the current node
  unsigned numChilds() 
  {
    unsigned num=0;
    if (nodeDown()) {
      ++num;
      while (nextNode()) 
	{
	  ++num;
	}
      nodeUp();
    }
    return num;
  }
  //! write document to file stream
  void writeto(FILE *stream) {xmlDocDump(stream,doc);}
  std::ostream &writeto(std::ostream &ost) const;
  //! dumps to a new memory buffer the caller must free (with free!)
  void DumpMemory(char **buffer,int *size) {xmlDocDumpMemory(doc,(xmlChar **)buffer,size);}
  //! check for error 
  bool good() const { return cstate == 0; }
  //! set error state
  void setstate(state flag) { cstate |= flag; }
  //! clear error state
  void clearstate(state s = 0) { cstate=s; }
protected:
  //! xml document of libxml
  xmlDocPtr doc;
  //! namespace ref
  std::string nshref;
  std::string nspref;
  //! top level namespace
  xmlNsPtr ns;
  //! root node
  xmlNodePtr root;
  //! current node
  xmlNodePtr cnode;
  //! current namespace
  xmlNsPtr cns;
  //! current state
  state cstate;
};

std::ostream &operator<<(std::ostream &ost,const DOM &d);


#endif


