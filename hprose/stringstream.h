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
 * LastModified: Jun 29, 2014                             *
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
        int64_t pos;
        int64_t _mark;
    public:
        StringStream() : buffer(""), pos(0), _mark(-1) {
        }
        StringStream(std::string str) : buffer(str), pos(0), _mark(-1) {
        }
        virtual ~StringStream() {}
        void init(std::string str) {
            buffer = str;
            pos = 0;
            _mark = -1;
        }
        void close() {
            buffer.clear();
            pos = 0;
            _mark = -1;
        }
        inline int64_t size() const {
            return buffer.length();
        }
        inline char getchar() {
            return buffer[pos++];
        }
        std::string read(int64_t length) {
            std::string str = buffer.substr(pos, length);
            pos += length;
            return str;
        }
        std::string read_full() {
            std::string str = buffer.substr(pos);
            pos = length();
            return str;
        }
        std::string readuntil(char tag) {
            int64_t p = buffer.find(tag, pos);
            if (p > 0) {
                std::string str = buffer.substr(pos, p - pos);
                pos = p + 1;
                return str;
            }
            return read_full();
        }
        int seek(int64_t offset, int whence = SEEK_SET) {
            switch (whence) {
                case SEEK_SET: pos = offset; break;
                case SEEK_CUR: pos += offset; break;
                case SEEK_END: pos = length() + offset; break;
            }
            _mark = -1;
            return 0;
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
        inline void skip(int64_t n) {
            pos += n;
        }
        inline bool eof() const {
            return pos >= size();
        }
        inline void write(std::string str, int64_t length = -1) {
            if (length == -1) {
                buffer.append(str);
            }
            else {
                buffer.append(str.substr(0, length));
            }
        }
        inline void write(char c) {
            buffer.append(&c, 1);
        }
        inline void write(int32_t i) {
            char buf[16];
            int n = sprintf(buf, "%d", i);
            buffer.append(buf, n);
        }
        inline void write(int64_t i) {
            char buf[32];
            int n = sprintf(buf, "%lld", i);
            buffer.append(buf, n);
        }
        inline void write(double f) {
            char buf[32];
            int n = sprintf(buf, "%.16g", f);
            buffer.append(buf, n);
        }
        // -----------------------------------------------------------
        // for PHP
        void __construct(Php::Parameters &params) {
            if (params.size() == 1) {
                init(params[0]);
            }
        }
        void init(Php::Parameters &params) {
            init(params[0]);
        }
        Php::Value length() const {
            return size();
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
        Php::Value seek(Php::Parameters &params) {
            switch (params.size()) {
                case 1: return seek(params[0]);
                case 2: return seek(params[0], params[1]);
                default: return -1;
            }
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
                        write(val.stringValue(), params[2]);
                    }
                    else {
                        write(val.stringValue());
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
        .method("init",
                &Hprose::StringStream::init,
                { Php::ByVal("str", Php::Type::String) })
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
        .method("seek",
                &Hprose::StringStream::seek,
                {
                    Php::ByVal("offset", Php::Type::Numeric),
                    Php::ByVal("whence", Php::Type::Numeric, false)
                })
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
