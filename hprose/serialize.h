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
 * hprose/serialize.h                                     *
 *                                                        *
 * hprose serialize library for php-cpp.                  *
 *                                                        *
 * LastModified: Jul 9, 2014                              *
 * Author: Ma Bingyao <andot@hprose.com>                  *
 *                                                        *
\**********************************************************/

#ifndef HPROSE_SERIALIZE_H_
#define HPROSE_SERIALIZE_H_

#include <phpcpp.h>

namespace Hprose {
    Php::Value serialize_bool(Php::Parameters &params) {
        return params[0] ? TagTrue : TagFalse;
    }

    Php::Value serialize_string(Php::Parameters &params) {
        int32_t len = ustrlen(params[0]);
        if (len == 0) {
            return TagString + TagQuote + TagQuote;
        }
        else {
            return TagString + std::to_string(ustrlen(params[0])) + TagQuote + params[0].stringValue() + TagQuote;
        }
    }

    Php::Value serialize_list(Php::Parameters &params) {
        int32_t c = params[0].size();
        if (c == 0) return TagList + TagOpenbrace + TagClosebrace;
        StringStream stream;
        bool simple = false;
        if (params.size() > 1) simple = params[1];
        Writer writer(stream, simple);
        writer.writeList(params[0]);
        return stream.toString();
    }

    inline Php::Value serialize(Php::Parameters &params) {
        StringStream stream;
        bool simple = false;
        if (params.size() > 1) simple = params[1];
        Writer writer(stream, simple);
        writer.serialize(params[0]);
        return stream.toString();
    }

    inline void publish_serialize(Php::Extension &ext) {
        ext.add("hprose_serialize_bool",
                &serialize_bool,
                {
                    Php::ByVal("b", Php::Type::Bool)
                })
        .add("hprose_serialize_string",
             &serialize_string,
             {
                 Php::ByVal("s", Php::Type::String)
             })
        .add("hprose_serialize_list",
             &serialize_list,
             {
                 Php::ByRef("a", Php::Type::Array),
                 Php::ByVal("simple", Php::Type::Bool, false)
             })
        .add("hprose_serialize",
             &serialize,
             {
                 Php::ByRef("v", Php::Type::Null),
                 Php::ByVal("simple", Php::Type::Bool, false)
             });
    }
}

#endif /* HPROSE_SERIALIZE_H_ */