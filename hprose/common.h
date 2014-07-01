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
 * hprose/filter.h                                        *
 *                                                        *
 * hprose filter interface for php-cpp.                   *
 *                                                        *
 * LastModified: Jul 1, 2014                              *
 * Author: Ma Bingyao <andot@hprose.com>                  *
 *                                                        *
\**********************************************************/

#ifndef HPROSE_COMMON_H_
#define HPROSE_COMMON_H_

#include <phpcpp.h>

namespace Hprose {

    class Bytes : public Php::Base {
    public:
        std::string value;
        Bytes() {}
        Bytes(std::string str) : value(str) {}
        virtual ~Bytes() {}
        void __construct(Php::Parameters &params) {
            value = params[0].stringValue();
        }
        std::string __toString() const {
            return value;
        }
    };

    Php::Value bytes(Php::Parameters &params) {
        return Php::Object("HproseBytes", params[0]);
    }

    class Map : public Php::Base {
    public:
        std::map<std::string, Php::Value> value;
        Map() {}
        Map(std::map<std::string, Php::Value> map) : value(map) {}
        virtual ~Map() {}
        void __construct(Php::Parameters &params) {
            value = params[0].mapValue();
        }
        std::string __toString() const {
            return "Map";
        }
    };

    Php::Value map(Php::Parameters &params) {
        return Php::Object("HproseMap", params[0]);
    }

    Php::Value is_list(Php::Parameters &params) {
        return params[0].isList();
    }

    Php::Value is_utf8(Php::Parameters &params) {
        const char *str = params[0].rawValue();
        int32_t len = params[0].size();
        for (int32_t i = 0; i < len; ++i) {
            unsigned char c = str[i], b;
            switch (c >> 4) {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                    break;
                case 12:
                case 13:
                    if ((str[++i] >> 6) != 0x2) return false;
                    break;
                case 14:
                    if ((str[++i] >> 6) != 0x2) return false;
                    if ((str[++i] >> 6) != 0x2) return false;
                    break;
                case 15:
                    b = str[++i];
                    if ((str[++i] >> 6) != 0x2) return false;
                    if ((str[++i] >> 6) != 0x2) return false;
                    if ((((c & 0xf) << 2) | ((b >> 4) & 0x3)) > 0x10) return false;
                    break;
                default:
                    return false;
            }
        }
        return true;
    }

    Php::Value ustrlen(Php::Parameters &params) {
        const char *str = params[0].rawValue();
        const int32_t length = params[0].size();
        int32_t len = length, pos = 0;
        while (pos < length) {
            unsigned char a = str[pos++];
            if (a < 0x80) {
                continue;
            }
            else if ((a & 0xE0) == 0xC0) {
                ++pos;
                --len;
            }
            else if ((a & 0xF0) == 0xE0) {
                pos += 2;
                len -= 2;
            }
            else if ((a * 0xF8) == 0xF0) {
                pos += 3;
                len -= 2;
            }
        }
        return len;
    }

    Php::Value array_ref_search(Php::Parameters &params) {
        Php::Value &value = params[0];
        Php::Value &array = params[1];
        if (value.isArray()) {
            for (int32_t i = 0, n = array.size(); i < n; ++i) {
                Php::Value v = array.get(i);
                if (value.refequals(v)) {
                    return i;
                }
            }
        }
        else {
            for (int32_t i = 0, n = array.size(); i < n; ++i) {
                Php::Value v = array.get(i);
                if (value.type() == v.type() && value == v) {
                    return i;
                }
            }
        }
        return false;
    }

    inline void publish_common(Php::Extension &ext) {
        Php::Class<Bytes> b("HproseBytes");
        b.method("__construct",
                 &Hprose::Bytes::__construct,
                 {
                     Php::ByVal("bytes", Php::Type::String)
                 });
        ext.add(std::move(b));
        ext.add("bytes",
                &bytes,
                { Php::ByVal("bytes", Php::Type::String) });
        Php::Class<Map> m("HproseMap");
        m.method("__construct",
                 &Hprose::Map::__construct,
                 {
                     Php::ByVal("map", Php::Type::Array)
                 });
        ext.add(std::move(m));
        ext.add("map",
                &map,
                { Php::ByVal("map", Php::Type::Array) });

        ext.add("is_list",
                &is_list,
                {
                    Php::ByVal("a", Php::Type::Array)
                });
        ext.add("is_utf8",
                &is_utf8,
                {
                    Php::ByVal("s", Php::Type::String)
                });
        ext.add("ustrlen",
                &ustrlen,
                {
                    Php::ByVal("s", Php::Type::String)
                });
        ext.add("array_ref_search",
                &array_ref_search,
                {
                    Php::ByRef("value", Php::Type::Null),
                    Php::ByVal("array", Php::Type::Array)
                });
    }
}
#endif /* HPROSE_COMMON_H_ */
