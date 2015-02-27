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
 * hprose/stringstream.h                                  *
 *                                                        *
 * hprose stringstream class for php-cpp.                 *
 *                                                        *
 * LastModified: Feb 27, 2015                             *
 * Author: Ma Bingyao <andot@hprose.com>                  *
 *                                                        *
\**********************************************************/

#ifndef HPROSE_STRINGSTREAM_H_
#define HPROSE_STRINGSTREAM_H_

#include <phpcpp.h>

namespace Hprose {

    class StringStream: public Php::Base {
    private:
        std::string buffer;
        int32_t pos;
        int32_t _mark;
    public:
        StringStream() : buffer(""), pos(0), _mark(-1) {
        }
        StringStream(const std::string str) : buffer(str), pos(0), _mark(-1) {
        }
        virtual ~StringStream() {}
        void close() {
            buffer.clear();
            pos = 0;
            _mark = -1;
        }
        inline int32_t size() const {
            return (int32_t)buffer.length();
        }
        inline char getchar() {
            return buffer[pos++];
        }
        std::string read(const int32_t length) {
            std::string str = buffer.substr(pos, length);
            pos += length;
            return str;
        }
        std::string read_full() {
            std::string str = buffer.substr(pos);
            pos = size();
            return str;
        }
        std::string readuntil(const char tag) {
            auto p = buffer.find(tag, pos);
            if (p != std::string::npos) {
                std::string str = buffer.substr(pos, (int32_t)p - pos);
                pos = (int32_t)p + 1;
                return str;
            }
            return read_full();
        }
        int32_t readint(const char tag) {
            int32_t result = 0;
            int32_t len = (int32_t)buffer.size();
            char c = getchar();
            if (c == tag) {
                return result;
            }
            int32_t sign = 1;
            switch (c) {
                case TagNeg: sign = -1; // no break here
                case TagPos: c = getchar(); break;
            }
            while (pos < len && c != tag) {
                result *= 10;
                result += (c - '0') * sign;
                c = getchar();
            }
            return result;
        }
        inline void mark() {
            _mark = pos;
        }
        inline void unmark() {
            _mark = -1;
        }
        inline void reset() {
            if (_mark != -1) {
                pos = _mark;
            }
        }
        inline void skip(const int32_t n) {
            pos += n;
        }
        inline bool eof() const {
            return pos >= size();
        }
        inline StringStream &write(const std::string str, const int32_t length = -1) {
            if (length == -1) {
                buffer.append(str);
            }
            else {
                buffer.append(str, 0, length);
            }
            return *this;
        }
        inline StringStream &write(const char *str, const int32_t length) {
            buffer.append(str, length);
            return *this;
        }
        inline StringStream &write(const char c) {
            buffer.append(1, c);
            return *this;
        }
        inline StringStream &write(const int32_t i) {
            buffer.append(std::to_string(i));
            return *this;
        }
        inline StringStream &write(const int64_t i) {
            buffer.append(std::to_string(i));
            return *this;
        }
        inline StringStream &write(const double f) {
            char buf[32];
            int n = sprintf(buf, "%.16g", f);
            buffer.append(buf, n);
            return *this;
        }
        inline std::string to_string() const {
            return buffer;
        }
        // -----------------------------------------------------------
        // for PHP
        void __construct(Php::Parameters &params) {
            if (params.size() == 1) {
                buffer = params[0].stringValue();
            }
        }
        Php::Value length() const {
            return (int64_t)size();
        }
        Php::Value getc() {
            return getchar();
        }
        Php::Value read(Php::Parameters &params) {
            return read(params[0]);
        }
        Php::Value readfull() {
            return read_full();
        }
        Php::Value readuntil(Php::Parameters &params) {
            return readuntil(params[0].stringValue()[0]);
        }
        void skip(Php::Parameters &params) {
            skip(params[0]);
        }
        Php::Value eof() {
            return eof();
        }
        void write(Php::Parameters &params) {
            Php::Value &val = params[0];
            switch (val.type()) {
                case Php::Type::Numeric:
                    write(val.numericValue());
                    break;
                case Php::Type::Float:
                    write(val.floatValue());
                    break;
                case Php::Type::String:
                    if (params.size() > 1) {
                        int32_t n = params[2];
                        int32_t size = val.size();
                        write(val.rawValue(), n > size ? size : n);
                    }
                    else {
                        write(val.rawValue(), val.size());
                    }
                    break;
                default:
                    break;
            }
        }
        Php::Value toString() const {
            return buffer;
        }
        Php::Value __toString() const {
            return buffer;
        }
    };
    inline void publish_stringstream(Php::Extension &ext) {
        Php::Class<Hprose::StringStream> c("HproseStringStream");
        c.method("__construct",
                 &Hprose::StringStream::__construct,
                 { Php::ByVal("str", Php::Type::String, false) })
         .method("close", &Hprose::StringStream::close)
         .method("length", &Hprose::StringStream::length)
         .method("getc", &Hprose::StringStream::getc)
         .method("read",
                 &Hprose::StringStream::read,
                 { Php::ByVal("length", Php::Type::Numeric) })
         .method("readfull", &Hprose::StringStream::readfull)
         .method("readuntil",
                 &Hprose::StringStream::readuntil,
                 { Php::ByVal("tag", Php::Type::String) })
         .method("mark", &Hprose::StringStream::mark)
         .method("unmark", &Hprose::StringStream::unmark)
         .method("reset", &Hprose::StringStream::reset)
         .method("skip",
                 &Hprose::StringStream::skip,
                 { Php::ByVal("n", Php::Type::Numeric) })
         .method("eof", &Hprose::StringStream::eof)
         .method("write",
                 &Hprose::StringStream::write,
                 {
                     Php::ByVal("value", Php::Type::Null),
                     Php::ByVal("length", Php::Type::Numeric, false)
                 })
         .method("toString", &Hprose::StringStream::toString)
         .method("__toString", &Hprose::StringStream::__toString);
        ext.add(std::move(c));
    }
}

#endif /* HPROSE_STRINGSTREAM_H_ */
