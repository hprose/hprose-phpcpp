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
    inline std::string serialize_bool(Php::Value &value) {
        return std::string() + (value ? TagTrue : TagFalse);
    }

    Php::Value serialize_bool(Php::Parameters &params) {
        return serialize_bool(params[0]);
    }

    inline std::string serialize_string(std::string value) {
        int32_t len = ustrlen(value);
        if (len == 0) {
            return std::string() + TagString + TagQuote + TagQuote;
        }
        else {
            return TagString + std::to_string(ustrlen(value)) + TagQuote + value + TagQuote;
        }
    }
    Php::Value serialize_string(Php::Parameters &params) {
        return serialize_string(params[0]);
    }

    inline std::string serialize_list(Php::Value &list, bool simple = false) {
        int32_t c = list.size();
        if (c == 0) return std::string() + TagList + TagOpenbrace + TagClosebrace;
        StringStream stream;
        Writer writer(stream, simple);
        writer.writeList(list);
        return stream.toString();
    }

    Php::Value serialize_list(Php::Parameters &params) {
        if (params.size() > 1) {
            return serialize_list(params[0], params[1]);
        }
        else {
            return serialize_list(params[0]);
        }
    }

    inline std::string serialize(Php::Value &value, bool simple = false) {
        StringStream stream;
        Writer writer(stream, simple);
        writer.serialize(value);
        return stream.toString();
    }

    inline Php::Value serialize(Php::Parameters &params) {
        if (params.size() > 1) {
            return serialize(params[0], params[1]);
        }
        else {
            return serialize(params[0]);
        }
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