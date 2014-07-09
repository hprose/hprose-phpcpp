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
 * LastModified: Jul 8, 2014                              *
 * Author: Ma Bingyao <andot@hprose.com>                  *
 *                                                        *
\**********************************************************/

#ifndef HPROSE_READER_H_
#define HPROSE_READER_H_

#include <phpcpp.h>
#include <math.h>

namespace Hprose {
    class ReaderRefer {
    public:
        inline virtual void set(const Php::Value &value) = 0;
        inline virtual const Php::Value &read(int32_t index) = 0;
        inline virtual void reset() = 0;
        ReaderRefer() {}
        virtual ~ReaderRefer() {}
    };

    class FakeReaderRefer : public ReaderRefer {
    public:
        inline virtual void set(const Php::Value &value) override {}
        inline virtual const Php::Value &read(int32_t index) override {
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
        inline virtual void set(const Php::Value &value) override {
            ref.push_back(value);
        }
        inline virtual const Php::Value &read(int32_t index) override {
            return ref[index];
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
        Php::Value readRef() {
            return refer->read(stream->readint(TagSemicolon));
        }
        void readClass() {
            std::string classname = ClassManager::get_class(_readStringWithoutTag());
            int32_t count = stream->readint(TagOpenbrace);
            std::vector<std::string> fields;
            for (int32_t i = 0; i < count; ++i) {
                fields.push_back(readString());
            }
            stream->skip(1);
            classref.push_back(std::move(std::make_pair(classname, fields)));
        }
    public:
        Reader() : RawReader() {};
        Reader(StringStream &stream, bool simple = false) : RawReader(stream) {
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
        Php::Value readIntegerWithoutTag() {
            return stream->readint(TagSemicolon);
        }
        Php::Value readInteger() {
            char tag = stream->getchar();
            switch (tag) {
                case '0': return 0;
                case '1': return 1;
                case '2': return 2;
                case '3': return 3;
                case '4': return 4;
                case '5': return 5;
                case '6': return 6;
                case '7': return 7;
                case '8': return 8;
                case '9': return 9;
                case TagInteger: return readIntegerWithoutTag();
                default: unexpectedTag(tag); break;
            }
            return nullptr;
        }
        Php::Value readLongWithoutTag() {
            return stream->readuntil(TagSemicolon);
        }
        Php::Value readLong() {
            char tag = stream->getchar();
            switch (tag) {
                case '0': return 0;
                case '1': return 1;
                case '2': return 2;
                case '3': return 3;
                case '4': return 4;
                case '5': return 5;
                case '6': return 6;
                case '7': return 7;
                case '8': return 8;
                case '9': return 9;
                case TagInteger:
                case TagLong: return readLongWithoutTag();
                default: unexpectedTag(tag); break;
            }
            return nullptr;
        }
        Php::Value readDoubleWithoutTag() {
            return std::stod(stream->readuntil(TagSemicolon));
        }
        Php::Value readInfinityWithoutTag() {
            return (stream->getchar() == TagNeg) ? -INFINITY : INFINITY;
        }
        Php::Value readDouble() {
            char tag = stream->getchar();
            switch (tag) {
                case '0': return 0;
                case '1': return 1;
                case '2': return 2;
                case '3': return 3;
                case '4': return 4;
                case '5': return 5;
                case '6': return 6;
                case '7': return 7;
                case '8': return 8;
                case '9': return 9;
                case TagInteger:
                case TagLong:
                case TagDouble: return readDoubleWithoutTag();
                case TagNaN: return NAN;
                case TagInfinity: return readInfinityWithoutTag();
                default: unexpectedTag(tag); break;
            }
            return nullptr;
        }
        Php::Value readDateWithoutTag() {
            Php::Value value;
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
        Php::Value readDate() {
            char tag = stream->getchar();
            switch (tag) {
                case TagNull: return nullptr;
                case TagDate: return readDateWithoutTag();
                case TagRef: return readRef();
                default: unexpectedTag(tag); break;
            }
            return nullptr;
        }
        Php::Value readTimeWithoutTag() {
            Php::Value value;
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
        Php::Value readTime() {
            char tag = stream->getchar();
            switch (tag) {
                case TagNull: return nullptr;
                case TagTime: return readTimeWithoutTag();
                case TagRef: return readRef();
                default: unexpectedTag(tag); break;
            }
            return nullptr;
        }
        Php::Value readBytesWithoutTag() {
            int32_t count = stream->readint(TagQuote);
            Php::Value value = stream->read(count);
            stream->skip(1);
            refer->set(value);
            return value;
        }
        Php::Value readBytes() {
            char tag = stream->getchar();
            switch (tag) {
                case TagNull: return nullptr;
                case TagEmpty: return "";
                case TagBytes: return readBytesWithoutTag();
                case TagRef: return readRef();
                default: unexpectedTag(tag); break;
            }
            return nullptr;
        }
        Php::Value readUTF8CharWithoutTag() {
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
            return Php::Value(buf, i);
        }
        Php::Value readStringWithoutTag() {
            Php::Value value = _readStringWithoutTag();
            refer->set(value);
            return value;
        }
        Php::Value readString() {
            char tag = stream->getchar();
            switch (tag) {
                case TagNull: return nullptr;
                case TagEmpty: return "";
                case TagUTF8Char: return readUTF8CharWithoutTag();
                case TagString: return readStringWithoutTag();
                case TagRef: return readRef();
                default: unexpectedTag(tag); break;
            }
            return nullptr;
        }
        Php::Value readGuidWithoutTag() {
            stream->skip(1);
            Php::Value value = stream->read(36);
            stream->skip(1);
            refer->set(value);
            return value;
        }
        Php::Value readGuid() {
            char tag = stream->getchar();
            switch (tag) {
                case TagNull: return nullptr;
                case TagGuid: return readGuidWithoutTag();
                case TagRef: return readRef();
                default: unexpectedTag(tag); break;
            }
            return nullptr;
        }
        Php::Value readListWithoutTag() {
            Php::Value list(Php::Type::Array);
            refer->set(list.ref());
            int32_t count = stream->readint(TagOpenbrace);
            for (int32_t i = 0; i < count; ++i) {
                list.set(i, unserialize());
            }
            stream->skip(1);
            return list;
        }
        Php::Value readList() {
            char tag = stream->getchar();
            switch (tag) {
                case TagNull: return nullptr;
                case TagList: return readListWithoutTag();
                case TagRef: return readRef();
                default: unexpectedTag(tag); break;
            }
            return nullptr;
        }
        Php::Value readMapWithoutTag() {
            Php::Value map(Php::Type::Array);
            refer->set(map.ref());
            int32_t count = stream->readint(TagOpenbrace);
            for (int32_t i = 0; i < count; ++i) {
                map.set(unserialize(), unserialize());
            }
            stream->skip(1);
            return map;
        }
        Php::Value readMap() {
            char tag = stream->getchar();
            switch (tag) {
                case TagNull: return nullptr;
                case TagMap: return readMapWithoutTag();
                case TagRef: return readRef();
                default: unexpectedTag(tag); break;
            }
            return nullptr;
        }
        Php::Value readObjectWithoutTag() {
            auto pair = classref[stream->readint(TagOpenbrace)];
            std::string classname = pair.first;
            std::vector<std::string> fields = pair.second;
            Php::Value object = Php::Object(classname.c_str());
            refer->set(object);
            int32_t count = (int32_t)fields.size();
            for (int32_t i = 0; i < count; ++i) {
                object.set(fields[i], unserialize());
            }
            stream->skip(1);
            return object;
        }
        Php::Value readObject() {
            char tag = stream->getchar();
            switch (tag) {
                case TagNull: return nullptr;
                case TagClass: readClass(); return readObject();
                case TagObject: return readObjectWithoutTag();
                case TagRef: return readRef();
                default: unexpectedTag(tag); break;
            }
            return nullptr;
        }
        Php::Value unserialize() {
            char tag = stream->getchar();
            switch (tag) {
                case '0': return 0;
                case '1': return 1;
                case '2': return 2;
                case '3': return 3;
                case '4': return 4;
                case '5': return 5;
                case '6': return 6;
                case '7': return 7;
                case '8': return 8;
                case '9': return 9;
                case TagInteger: return readIntegerWithoutTag();
                case TagLong: return readLongWithoutTag();
                case TagDouble: return readDoubleWithoutTag();
                case TagNull: return nullptr;
                case TagEmpty: return "";
                case TagTrue: return true;
                case TagFalse: return false;
                case TagNaN: return NAN;
                case TagInfinity: return readInfinityWithoutTag();
                case TagDate: return readDateWithoutTag();
                case TagTime: return readTimeWithoutTag();
                case TagBytes: return readBytesWithoutTag();
                case TagUTF8Char: return readUTF8CharWithoutTag();
                case TagString: return readStringWithoutTag();
                case TagList: return readListWithoutTag();
                case TagMap: return readMapWithoutTag();
                case TagClass: readClass(); return readObject();
                case TagObject: return readObjectWithoutTag();
                case TagRef: return readRef();
                case TagError: throw Php::Exception(readString());
                default: unexpectedTag(tag); break;
            }
            return nullptr;
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
        .method("readIntegerWithoutTag",
                &Hprose::Reader::readIntegerWithoutTag)
        .method("readInteger",
                &Hprose::Reader::readInteger)
        .method("readLongWithoutTag",
                &Hprose::Reader::readLongWithoutTag)
        .method("readLong",
                &Hprose::Reader::readLong)
        .method("readDoubleWithoutTag",
                &Hprose::Reader::readDoubleWithoutTag)
        .method("readDouble",
                &Hprose::Reader::readDouble)
        .method("readInfinityWithoutTag",
                &Hprose::Reader::readInfinityWithoutTag)
        .method("readDateWithoutTag",
                &Hprose::Reader::readDateWithoutTag)
        .method("readDate",
                &Hprose::Reader::readDate)
        .method("readTimeWithoutTag",
                &Hprose::Reader::readTimeWithoutTag)
        .method("readTime",
                &Hprose::Reader::readTime)
        .method("readBytesWithoutTag",
                &Hprose::Reader::readBytesWithoutTag)
        .method("readBytes",
                &Hprose::Reader::readBytes)
        .method("readUTF8CharWithoutTag",
                &Hprose::Reader::readUTF8CharWithoutTag)
        .method("readStringWithoutTag",
                &Hprose::Reader::readStringWithoutTag)
        .method("readString",
                &Hprose::Reader::readString)
        .method("readGuidWithoutTag",
                &Hprose::Reader::readGuidWithoutTag)
        .method("readGuid",
                &Hprose::Reader::readGuid)
        .method("readListWithoutTag",
                &Hprose::Reader::readListWithoutTag)
        .method("readList",
                &Hprose::Reader::readList)
        .method("readMapWithoutTag",
                &Hprose::Reader::readMapWithoutTag)
        .method("readMap",
                &Hprose::Reader::readMap)
        .method("readObjectWithoutTag",
                &Hprose::Reader::readObjectWithoutTag)
        .method("readObject",
                &Hprose::Reader::readObject)
        .method("reset", &Hprose::Reader::reset);
        ext.add(std::move(c));
    }
}

#endif /* HPROSE_READER_H_ */