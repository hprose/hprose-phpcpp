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
 * LastModified: Jun 24, 2014                             *
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

    inline void publish_tags(Php::Namespace &ns) {
        Php::Class<Hprose::Tags> tags("Tags");

        tags.property("TagInteger",     Hprose::TagInteger,     Php::Const);
        tags.property("TagLong",        Hprose::TagLong,        Php::Const);
        tags.property("TagDouble",      Hprose::TagDouble,      Php::Const);
        tags.property("TagNull",        Hprose::TagNull,        Php::Const);
        tags.property("TagEmpty",       Hprose::TagEmpty,       Php::Const);
        tags.property("TagTrue",        Hprose::TagTrue,        Php::Const);
        tags.property("TagFalse",       Hprose::TagFalse,       Php::Const);
        tags.property("TagNaN",         Hprose::TagNaN,         Php::Const);
        tags.property("TagInfinity",    Hprose::TagInfinity,    Php::Const);
        tags.property("TagDate",        Hprose::TagDate,        Php::Const);
        tags.property("TagTime",        Hprose::TagTime,        Php::Const);
        tags.property("TagUTC",         Hprose::TagUTC,         Php::Const);
        tags.property("TagBytes",       Hprose::TagBytes,       Php::Const);
        tags.property("TagUTF8Char",    Hprose::TagUTF8Char,    Php::Const);
        tags.property("TagString",      Hprose::TagString,      Php::Const);
        tags.property("TagGuid",        Hprose::TagGuid,        Php::Const);
        tags.property("TagList",        Hprose::TagList,        Php::Const);
        tags.property("TagMap",         Hprose::TagMap,         Php::Const);
        tags.property("TagClass",       Hprose::TagClass,       Php::Const);
        tags.property("TagObject",      Hprose::TagObject,      Php::Const);
        tags.property("TagRef",         Hprose::TagRef,         Php::Const);

        tags.property("TagPos",         Hprose::TagPos,         Php::Const);
        tags.property("TagNeg",         Hprose::TagNeg,         Php::Const);
        tags.property("TagSemicolon",   Hprose::TagSemicolon,   Php::Const);
        tags.property("TagOpenbrace",   Hprose::TagOpenbrace,   Php::Const);
        tags.property("TagClosebrace",  Hprose::TagClosebrace,  Php::Const);
        tags.property("TagQuote",       Hprose::TagQuote,       Php::Const);
        tags.property("TagPoint",       Hprose::TagPoint,       Php::Const);

        tags.property("TagFunctions",   Hprose::TagFunctions,   Php::Const);
        tags.property("TagCall",        Hprose::TagCall,        Php::Const);
        tags.property("TagResult",      Hprose::TagResult,      Php::Const);
        tags.property("TagArgument",    Hprose::TagArgument,    Php::Const);
        tags.property("TagError",       Hprose::TagError,       Php::Const);
        tags.property("TagEnd",         Hprose::TagEnd,         Php::Const);

        ns.add(std::move(tags));
    }
}
#endif /* HPROSE_TAGS_H_ */
