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
 * hprose/unserialize.h                                   *
 *                                                        *
 * hprose unserialize library for php-cpp.                *
 *                                                        *
 * LastModified: Feb 27, 2015                             *
 * Author: Ma Bingyao <andot@hprose.com>                  *
 *                                                        *
\**********************************************************/

#ifndef HPROSE_UNSERIALIZE_H_
#define HPROSE_UNSERIALIZE_H_

#include <phpcpp.h>

namespace Hprose {
    Php::Value unserialize_with_stream(Php::Parameters &params) {
        bool simple = false;
        if (params.size() > 1) simple = params[1];
        StringStream *stream = (StringStream *)params[0].implementation();
        Reader reader(*stream, simple);
        return reader.unserialize();
    }

    Php::Value unserialize_list_with_stream(Php::Parameters &params) {
        StringStream *stream = (StringStream *)params[0].implementation();
        Reader reader(*stream);
        return reader.readListWithoutTag();
    }

    Php::Value unserialize_string_with_stream(Php::Parameters &params) {
        StringStream *stream = (StringStream *)params[0].implementation();
        Reader reader(*stream);
        return reader._readString();
    }

    inline Php::Value unserialize(Php::Parameters &params) {
        bool simple = false;
        if (params.size() > 1) simple = params[1];
        StringStream stream(params[0]);
        Reader reader(stream, simple);
        return reader.unserialize();
    }

    inline void publish_unserialize(Php::Extension &ext) {
        ext.add("hprose_unserialize_with_stream",
                &unserialize_with_stream,
                {
                    Php::ByVal("s", "HproseStringStream"),
                    Php::ByVal("simple", Php::Type::Bool, false)
                },
                true)
        .add("hprose_unserialize_list_with_stream",
             &unserialize_list_with_stream,
             {
                 Php::ByVal("s", "HproseStringStream")
             },
             true)
        .add("hprose_unserialize_string_with_stream",
             &unserialize_string_with_stream,
             {
                 Php::ByVal("s", "HproseStringStream")
             },
             true)
        .add("hprose_unserialize",
             &unserialize,
             {
                 Php::ByVal("s", Php::Type::String),
                 Php::ByVal("simple", Php::Type::Bool, false)
             },
             true);
    }
}

#endif /* HPROSE_UNSERIALIZE_H_ */
