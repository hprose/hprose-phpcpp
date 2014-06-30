/**********************************************************\
|                                                          |
|                          hprose                          |
|                                                          |
| Official WebSite: http://www.hprose.com/                 |
|                   http://www.hprose.org/                 |
|                                                          |
\**********************************************************/

/**********************************************************\
 *                                                        *
 * hprose/tags.h                                          *
 *                                                        *
 * hprose tags library for php-cpp.                       *
 *                                                        *
 * LastModified: Jun 30, 2014                             *
 * Author: Ma Bingyao <andot@hprose.com>                  *
 *                                                        *
\**********************************************************/

#ifndef HPROSE_TAGS_H_
#define HPROSE_TAGS_H_

#include <phpcpp.h>

namespace Hprose {
    /* Serialize Tags */
    const char TagInteger     = 'i';
    const char TagLong        = 'l';
    const char TagDouble      = 'd';
    const char TagNull        = 'n';
    const char TagEmpty       = 'e';
    const char TagTrue        = 't';
    const char TagFalse       = 'f';
    const char TagNaN         = 'N';
    const char TagInfinity    = 'I';
    const char TagDate        = 'D';
    const char TagTime        = 'T';
    const char TagUTC         = 'Z';
    const char TagBytes       = 'b';
    const char TagUTF8Char    = 'u';
    const char TagString      = 's';
    const char TagGuid        = 'g';
    const char TagList        = 'a';
    const char TagMap         = 'm';
    const char TagClass       = 'c';
    const char TagObject      = 'o';
    const char TagRef         = 'r';
    /* Serialize Marks */
    const char TagPos         = '+';
    const char TagNeg         = '-';
    const char TagSemicolon   = ';';
    const char TagOpenbrace   = '{';
    const char TagClosebrace  = '}';
    const char TagQuote       = '"';
    const char TagPoint       = '.';
    /* Protocol Tags */
    const char TagFunctions   = 'F';
    const char TagCall        = 'C';
    const char TagResult      = 'R';
    const char TagArgument    = 'A';
    const char TagError       = 'E';
    const char TagEnd         = 'z';

    class Tags: public Php::Base {};

    inline void publish_tags(Php::Extension &ext) {
        Php::Class<Hprose::Tags> c("HproseTags");

        c.property("TagInteger",     Hprose::TagInteger,     Php::Const)
         .property("TagLong",        Hprose::TagLong,        Php::Const)
         .property("TagDouble",      Hprose::TagDouble,      Php::Const)
         .property("TagNull",        Hprose::TagNull,        Php::Const)
         .property("TagEmpty",       Hprose::TagEmpty,       Php::Const)
         .property("TagTrue",        Hprose::TagTrue,        Php::Const)
         .property("TagFalse",       Hprose::TagFalse,       Php::Const)
         .property("TagNaN",         Hprose::TagNaN,         Php::Const)
         .property("TagInfinity",    Hprose::TagInfinity,    Php::Const)
         .property("TagDate",        Hprose::TagDate,        Php::Const)
         .property("TagTime",        Hprose::TagTime,        Php::Const)
         .property("TagUTC",         Hprose::TagUTC,         Php::Const)
         .property("TagBytes",       Hprose::TagBytes,       Php::Const)
         .property("TagUTF8Char",    Hprose::TagUTF8Char,    Php::Const)
         .property("TagString",      Hprose::TagString,      Php::Const)
         .property("TagGuid",        Hprose::TagGuid,        Php::Const)
         .property("TagList",        Hprose::TagList,        Php::Const)
         .property("TagMap",         Hprose::TagMap,         Php::Const)
         .property("TagClass",       Hprose::TagClass,       Php::Const)
         .property("TagObject",      Hprose::TagObject,      Php::Const)
         .property("TagRef",         Hprose::TagRef,         Php::Const)

         .property("TagPos",         Hprose::TagPos,         Php::Const)
         .property("TagNeg",         Hprose::TagNeg,         Php::Const)
         .property("TagSemicolon",   Hprose::TagSemicolon,   Php::Const)
         .property("TagOpenbrace",   Hprose::TagOpenbrace,   Php::Const)
         .property("TagClosebrace",  Hprose::TagClosebrace,  Php::Const)
         .property("TagQuote",       Hprose::TagQuote,       Php::Const)
         .property("TagPoint",       Hprose::TagPoint,       Php::Const)

         .property("TagFunctions",   Hprose::TagFunctions,   Php::Const)
         .property("TagCall",        Hprose::TagCall,        Php::Const)
         .property("TagResult",      Hprose::TagResult,      Php::Const)
         .property("TagArgument",    Hprose::TagArgument,    Php::Const)
         .property("TagError",       Hprose::TagError,       Php::Const)
         .property("TagEnd",         Hprose::TagEnd,         Php::Const);

        ext.add(std::move(c));
    }
}
#endif /* HPROSE_TAGS_H_ */
