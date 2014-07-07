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
 * hprose/reader.h                                        *
 *                                                        *
 * hprose reader class for php-cpp.                       *
 *                                                        *
 * LastModified: Jul 7, 2014                              *
 * Author: Ma Bingyao <andot@hprose.com>                  *
 *                                                        *
\**********************************************************/

#ifndef HPROSE_READER_H_
#define HPROSE_READER_H_

#include <unordered_map>
#include <phpcpp.h>
#include <math.h>

namespace Hprose {
    class ReaderRefer {
    public:
        inline virtual void set(Php::Value &value) = 0;
        inline virtual Php::Value read(int32_t index) = 0;
        inline virtual void reset() = 0;
        ReaderRefer() {}
        virtual ~ReaderRefer() {}
    };

    class FakeReaderRefer : public ReaderRefer {
    public:
        inline virtual void set(Php::Value &value) override {}
        inline virtual Php::Value read(int32_t index) override {
            throw Php::Exception(std::string("Unexcepted serialize tag '") + TagRef + "' in stream");
        };
        inline virtual void reset() override {};
        FakeReaderRefer() {}
        virtual ~FakeReaderRefer() {}
    };

    class RealReaderRefer : public ReaderRefer {
    private:
        std::vector<Php::Value> ref;
    public:
        inline virtual void set(Php::Value &value) override {
            if (value.isArray()) {
                printf("isref: %d\r\n", value.isRef());
                ref.push_back(value.ref());
            }
            else {
                ref.push_back(value);
            }
        }
        inline virtual Php::Value read(int32_t index) override {
            printf("isref: %d\r\n", ref[index].isRef());
            return ref[index].ref();
        }
        inline virtual void reset() override {
            ref.clear();
        }
        RealReaderRefer() {}
        virtual ~RealReaderRefer() {}
    };

    class Reader : public RawReader {
    private:
        std::vector<std::pair<std::string, std::vector<std::string>>> classref;
        ReaderRefer *refer;
        inline void init_refer(bool simple) {
            if (simple) {
                refer = new FakeReaderRefer();
            }
            else {
                refer = new RealReaderRefer();
            }
        }
        std::string _readStringWithoutTag() {
            int32_t len = stream->readint(TagQuote), utf8len = 0;
            stream->mark();
            for (int32_t i = 0; i < len; ++i) {
                unsigned char tag = stream->getchar();
                switch (tag >> 4) {
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    ++utf8len;
                    break;
                    case 12:
                    case 13:
                    stream->skip(1);
                    utf8len += 2;
                    break;
                    case 14:
                    stream->skip(2);
                    utf8len += 3;
                    break;
                    case 15:
                    stream->skip(3);
                    utf8len += 4;
                    ++i;
                    break;
                    default:
                    throw Php::Exception("bad utf-8 encoding");
                    break;
                }
            }
            stream->reset();
            stream->unmark();
            std::string s = stream->read(utf8len);
            stream->skip(1);
            return s;
        }
        Php::Value &readRef(Php::Value &value) {
            value = refer->read(stream->readint(TagSemicolon));
            return value;
        }
    public:
        StringStream *stream;
        Reader() {};
        Reader(StringStream &stream, bool simple = false) : stream(&stream) {
            init_refer(simple);
        }
        virtual ~Reader() {
            reset();
            delete refer;
        }
        inline void reset() {
            classref.clear();
            refer->reset();
        }
        Php::Value &readIntegerWithoutTag(Php::Value &value) {
            value = std::stoll(stream->readuntil(TagSemicolon));
            return value;
        }
        Php::Value &readLongWithoutTag(Php::Value &value) {
            value = stream->readuntil(TagSemicolon);
            return value;
        }
        Php::Value &readDoubleWithoutTag(Php::Value &value) {
            value = std::stod(stream->readuntil(TagSemicolon));
            return value;
        }
        Php::Value &readInfinityWithoutTag(Php::Value &value) {
            value = (stream->getchar() == TagNeg) ? -INFINITY : INFINITY;
            return value;
        }
        Php::Value &readDateWithoutTag(Php::Value &value) {
            int32_t year = std::stoi(stream->read(4));
            int32_t month = std::stoi(stream->read(2));
            int32_t day = std::stoi(stream->read(2));
            char tag = stream->getchar();
            if (tag == TagTime) {
                int32_t hour = std::stoi(stream->read(2));
                int32_t minute = std::stoi(stream->read(2));
                int32_t second = std::stoi(stream->read(2));
                int32_t microsecond = 0;
                tag = stream->getchar();
                if (tag == TagPoint) {
                    microsecond = std::stoi(stream->read(3)) * 1000;
                    tag = stream->getchar();
                    if ((tag >= '0') && (tag <= '9')) {
                        microsecond += (tag - '0') * 100 + std::stoi(stream->read(2));
                        tag = stream->getchar();
                        if ((tag >= '0') && (tag <= '9')) {
                            stream->skip(2);
                            tag = stream->getchar();
                        }
                    }
                }
                value = Php::Object("HproseDateTime", new DateTime(year, month, day, hour, minute, second, microsecond, tag == TagUTC));
            }
            else {
                value = Php::Object("HproseDate", new Date(year, month, day, tag == TagUTC));
            }
            refer->set(value);
            return value;
        }
        Php::Value &readTimeWithoutTag(Php::Value &value) {
            int32_t hour = std::stoi(stream->read(2));
            int32_t minute = std::stoi(stream->read(2));
            int32_t second = std::stoi(stream->read(2));
            int32_t microsecond = 0;
            char tag = stream->getchar();
            if (tag == TagPoint) {
                microsecond = std::stoi(stream->read(3)) * 1000;
                tag = stream->getchar();
                if ((tag >= '0') && (tag <= '9')) {
                    microsecond += (tag - '0') * 100 + std::stoi(stream->read(2));
                    tag = stream->getchar();
                    if ((tag >= '0') && (tag <= '9')) {
                        stream->skip(2);
                        tag = stream->getchar();
                    }
                }
            }
            value = Php::Object("HproseTime", new Time(hour, minute, second, microsecond, tag == TagUTC));
            refer->set(value);
            return value;
        }
        Php::Value &readBytesWithoutTag(Php::Value &value) {
            int32_t count = stream->readint(TagQuote);
            value = stream->read(count);
            stream->skip(1);
            refer->set(value);
            return value;
        }
        Php::Value &readUTF8CharWithoutTag(Php::Value &value) {
            char buf[4];
            int32_t i = 1;
            buf[0] = stream->getchar();
            if ((buf[0] & 0xE0) == 0xC0) {
                buf[1] = stream->getchar();
                ++i;
            }
            else if ((buf[0] & 0xF0) == 0xE0) {
                buf[1] = stream->getchar();
                buf[2] = stream->getchar();
                i += 2;
            }
            else if (buf[0] > 0x7F) {
                throw Php::Exception("bad utf-8 encoding");
            }
            value = std::string(buf, i);
            return value;
        }
        Php::Value &readStringWithoutTag(Php::Value &value) {
            value = _readStringWithoutTag();
            refer->set(value);
            return value;
        }
        Php::Value &readGuidWithoutTag(Php::Value &value) {
            stream->skip(1);
            value = stream->read(36);
            stream->skip(1);
            refer->set(value);
            return value;
        }
        Php::Value &readListWithoutTag(Php::Value &list) {
            refer->set(list.setType(Php::Type::Array).setRef());
            int32_t count = stream->readint(TagOpenbrace);
            for (int32_t i = 0; i < count; ++i) {
                Php::Value value;
                list.set(i, unserialize(value));
            }
            stream->skip(1);
            return list;
        }
        Php::Value &readMapWithoutTag(Php::Value &map) {
            refer->set(map.setType(Php::Type::Array).setRef());
            int32_t count = stream->readint(TagOpenbrace);
            for (int32_t i = 0; i < count; ++i) {
                Php::Value key, value;
                map.set(unserialize(key), unserialize(value));
            }
            stream->skip(1);
            return map;
        }
        Php::Value &unserialize(Php::Value &value) {
            char tag = stream->getchar();
            switch (tag) {
                case '0': value = 0; return value;
                case '1': value = 1; return value;
                case '2': value = 2; return value;
                case '3': value = 3; return value;
                case '4': value = 4; return value;
                case '5': value = 5; return value;
                case '6': value = 6; return value;
                case '7': value = 7; return value;
                case '8': value = 8; return value;
                case '9': value = 9; return value;
                case TagInteger: return readIntegerWithoutTag(value);
                case TagLong: return readLongWithoutTag(value);
                case TagDouble: return readDoubleWithoutTag(value);
                case TagNull: value = nullptr; return value;
                case TagEmpty: value = ""; return value;
                case TagTrue: value = true; return value;
                case TagFalse: value = false; return value;
                case TagNaN: value = NAN; return value;
                case TagInfinity: return readInfinityWithoutTag(value);
                case TagDate: return readDateWithoutTag(value);
                case TagTime: return readTimeWithoutTag(value);
                case TagBytes: return readBytesWithoutTag(value);
                case TagUTF8Char: return readUTF8CharWithoutTag(value);
                case TagString: return readStringWithoutTag(value);
                case TagList: return readListWithoutTag(value);
                case TagMap: return readMapWithoutTag(value);
                case TagRef: return readRef(value);
                default:
                break;
            }
            return value;
        }
        Php::Value unserialize() {
            Php::Value value;
            return unserialize(value);
        }
        // -----------------------------------------------------------
        // for PHP
        void __construct(Php::Parameters &params) {
            stream = (StringStream *)params[0].implementation();
            bool simple = false;
            if (params.size() > 1) {
                simple = params[1];
            }
            init_refer(simple);
        }
    };

    inline void publish_reader(Php::Extension &ext) {
        Php::Class<Reader> c("HproseReader");
        c.method("__construct",
                 &Hprose::Reader::__construct,
                 {
                     Php::ByRef("stream", "HproseStringStream"),
                     Php::ByVal("simple", Php::Type::Bool, false)
                 })
        .method("unserialize",
                &Hprose::Reader::unserialize)
        .method("reset", &Hprose::Reader::reset);
        ext.add(std::move(c));
    }
}

#endif /* HPROSE_READER_H_ */