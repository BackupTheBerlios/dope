#ifndef DOPE_XML_H
#define DOPE_XML_H


#include <stdexcept>

class XML
{
public:
  typedef std::string xmlString;
  
  // minimalistic "xml" encoding (only 8Bit characters)

  /*
    relevant xml spec

    Following the characteristics defined in the Unicode standard, characters are classed as base characters (among others, these contain the alphabetic characters of the Latin alphabet), ideographic characters, and combining characters (among others, this class contains most diacritics) Digits and extenders are also distinguished.

    For now we are only interested in unicode characters #x0000-#x00FF
    
    [85]    BaseChar    ::=    [#x0041-#x005A] | [#x0061-#x007A] | [#x00C0-#x00D6] | [#x00D8-#x00F6] | [#x00F8-#x00FF]  ...
    [88]    Digit    ::=    [#x0030-#x0039] |...
    [89]    Extender    ::=    #x00B7 | ...
    [86]    Ideographic    ::=  uninteresting
    [87]    CombiningChar    ::=    uninteresting
    [84]    Letter    ::=    BaseChar | Ideographic

    NameChar    ::=    Letter | Digit | '.' | '-' | '_' | ':' | CombiningChar | Extender [5]
    Name    ::=    (Letter | '_' | ':') (NameChar)*

    The character classes defined here can be derived from the Unicode 2.0 character database as follows:
      Name start characters must have one of the categories Ll, Lu, Lo, Lt, Nl.
      Name characters other than Name-start characters must have one of the categories Mc, Me, Mn, Lm, or Nd.
      Characters in the compatibility area (i.e. with character code greater than #xF900 and less than #xFFFE) are not allowed in XML names.
      Characters which have a font or compatibility decomposition (i.e. those with a "compatibility formatting tag" in field 5 of the database -- marked by field 5 beginning with a "<") are not allowed.
      The following characters are treated as name-start characters rather than name characters, because the property file classifies them as Alphabetic: [#x02BB-#x02C1], #x0559, #x06E5, #x06E6.
      Characters #x20DD-#x20E0 are excluded (in accordance with Unicode 2.0, section 5.14).
      Character #x00B7 is classified as an extender, because the property list so identifies it.
      Character #x0387 is added as a name character, because #x00B7 is its canonical equivalent.
      Characters ':' and '_' are allowed as name-start characters.
      Characters '-' and '.' are allowed as name characters.
  */
  //! is this character a BaseChar according the xml spec
  /*!
    For now we are only interested in unicode characters #x0000-#x00FF
    [85]    BaseChar    ::=    [#x0041-#x005A] | [#x0061-#x007A] | [#x00C0-#x00D6] | [#x00D8-#x00F6] | [#x00F8-#x00FF]  ...
  */
  static bool isBaseChar(xmlString::value_type _c)
  {
    unsigned c=_c;
    if ((c>=0x41)&&(c<=0x5a))
      return true;
    if ((c>=0x61)&&(c<=0x7A))
      return true;
    if ((c>=0xC0)&&(c<=0xD6))
      return true;
    if ((c>=0xD8)&&(c<=0xF6))
      return true;
    if ((c>=0xF8)&&(c<=0xFF))
      return true;
    // todo - when using multibyte characters you have to add all the others
    return false;
  }

  //! is this character a Digit according the xml spec
  /*!
    For now we are only interested in unicode characters #x0000-#x00FF
    [88]    Digit    ::=    [#x0030-#x0039] |...
  */
  static DOPE_INLINE bool isDigit(xmlString::value_type c)
  {
    return ((c>=0x30)&&(c<=0x39));
  }

  //! is this character a Extender according the xml spec
  /*!
    For now we are only interested in unicode characters #x0000-#x00FF
    [89]    Extender    ::=    #x00B7 | ...
   */
  static DOPE_INLINE bool isExtender(xmlString::value_type _c)
  {
    unsigned c=_c;
    return (c==0xB7);
  }

  //! is this character a letter according the xml spec
  /*!
    [84]    Letter    ::=    BaseChar | Ideographic
  */
  static DOPE_INLINE bool isLetter(xmlString::value_type c)
  {
    return isBaseChar(c);
  }

  //! convert to xml Name
  /*!
    Name    ::=    (Letter | '_' | ':') (NameChar)*
  */

  static xmlString xmlName(const char *name)
  {
    if (!name) throw std::invalid_argument(std::string(__PRETTY_FUNCTION__));
    xmlString result(name);
    if (!isLetter(result[0]))
      throw std::invalid_argument(std::string(__PRETTY_FUNCTION__));
    // the problem are following characters:
    xmlString search("<>[],: *&");
    static const char* replace[]={"Of","","CVector","","And","","_","Ptr","Ref"};
    // we replace them
    size_t pos;
    for (size_t i=1;i<result.size();){
      if ((pos=search.find(result[i]))!=xmlString::npos){
	result.replace(i,1,replace[pos]);
	i+=std::strlen(replace[pos]); 
      }else{
	++i;
      }
    }
    return result;
  }

  static xmlString xmlCharData(const std::string &data)
  {
    return data;
  }

  static xmlString xmlAttValue(const char * value)
  {
    xmlString result(value);
    if (!value)
      value="";
    // the problem are following characters:
    xmlString search("<>&\"");
    static const char* replace[]={"&lt;","&gt;","&amp;","&quot;"};

    // we replace them
    size_t pos;
    for (size_t i=1;i<result.size();){
      if ((pos=search.find(result[i]))!=xmlString::npos){
	result.replace(i,1,replace[pos]);
	i+=std::strlen(replace[pos]); 
      }else{
	++i;
      }
    }
    return xmlString("\"")+result+'"';
  }
  
  static xmlString xmlStartTag(const char *name)
  {
    if (!name) throw std::invalid_argument(std::string(__PRETTY_FUNCTION__));
    return xmlString("<")+xmlName(name)+'>';
  }

  static xmlString xmlEndTag(const char *name)
  {
    if (!name) throw std::invalid_argument(std::string(__PRETTY_FUNCTION__));
    return xmlString("</")+xmlName(name)+'>';
  }

};

#endif
