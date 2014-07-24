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
 * hprose/rawreader.h                                     *
 *                                                        *
 * hprose rawreader class for php-cpp.                    *
 *                                                        *
 * LastModified: Jul 2, 2014                              *
 * Author: Ma Bingyao <andot@hprose.com>                  *
 *                                                        *
\**********************************************************/

#ifndef HPROSE_RAWREADER_H_
#define HPROSE_RAWREADER_H_

#include <phpcpp.h>

namespace Hprose {
    class RawReader : public Php::Base {
    private:
        inline int32_t stoi(const std::string &str) {
            if (str.empty()) return 0;
            return std::stoi(str);
        }
        inline void readNumberRaw(StringStream &ostream) {
            ostream.write(stream->readuntil(TagSemicolon)).write(TagSemicolon);
        }
        inline void readDateTimeRaw(StringStream &ostream) {
            char buf[32];
            int i = 0;
            for (unsigned char tag = 0; i < 31 && tag != TagSemicolon && tag != TagUTC; ++i) {
                tag = stream->getchar();
                buf[i] = tag;
            }
            if (i >= 31) throw Php::Exception("incorrect serialization data");
            ostream.write(buf, i);
        }
        inline void readUTF8CharRaw(StringStream &ostream) {
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
            ostream.write(buf, i);
        }
        inline void readBytesRaw(StringStream &ostream) {
            std::string len = stream->readuntil(TagQuote);
            ostream.write(len + TagQuote + stream->read(stoi(len)) + TagQuote);
            stream->skip(1);
        }
        inline void readStringRaw(StringStream &ostream) {
            std::string len = stream->readuntil(TagQuote);
            stream->mark();
            int32_t length = stoi(len), utf8len = 0;
            for (int32_t i = 0; i < length; ++i) {
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
            ostream.write(len + TagQuote + stream->read(utf8len) + TagQuote);
            stream->skip(1);
        }
        inline void readGuidRaw(StringStream &ostream) {
            ostream.write(ostream.read(38));
        }
        inline void readComplexRaw(StringStream &ostream) {
            ostream.write(stream->readuntil(TagOpenbrace) + TagOpenbrace);
            char tag;
            while ((tag = stream->getchar()) != TagClosebrace) {
                readRaw(ostream, tag);
            }
            ostream.write(tag);
        }
    public:
        StringStream *stream;
        RawReader() {}
        RawReader(StringStream &stream) : stream(&stream) {}
        virtual ~RawReader() {}
        static void unexpectedTag(const char tag, const std::string expectTags = "") {
            if (tag && !expectTags.empty()) {
                throw Php::Exception(("Tags '" + expectTags + "' expected, but '").append(1, tag).append("' found in stream"));
            }
            else if (tag) {
                throw Php::Exception(std::string("Unexpected serialize tag '").append(1, tag).append("' in stream"));
            }
            else {
                throw Php::Exception("No byte found in stream");
            }
        }
        StringStream *readRaw() {
            StringStream *ostream = new StringStream();
            readRaw(*ostream);
            return ostream;
        }
        StringStream &readRaw(StringStream &ostream) {
            return readRaw(ostream, stream->getchar());
        }
        StringStream &readRaw(StringStream &ostream, const char tag) {
            ostream.write(tag);
            switch (tag) {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case TagNull:
                case TagEmpty:
                case TagTrue:
                case TagFalse:
                case TagNaN:
                    break;
                case TagInfinity:
                    ostream.write(stream->getchar());
                    break;
                case TagInteger:
                case TagLong:
                case TagDouble:
                case TagRef:
                    readNumberRaw(ostream);
                    break;
                case TagDate:
                case TagTime:
                    readDateTimeRaw(ostream);
                    break;
                case TagUTF8Char:
                    readUTF8CharRaw(ostream);
                    break;
                case TagBytes:
                    readBytesRaw(ostream);
                    break;
                case TagString:
                    readStringRaw(ostream);
                    break;
                case TagGuid:
                    readGuidRaw(ostream);
                    break;
                case TagList:
                case TagMap:
                case TagObject:
                    readComplexRaw(ostream);
                    break;
                case TagClass:
                    readComplexRaw(ostream);
                    readRaw(ostream);
                    break;
                case TagError:
                    readRaw(ostream);
                    break;
                default:
                    unexpectedTag(tag);
                    break;
            }
            return ostream;
        }
        // -----------------------------------------------------------
        // for PHP
        void __construct(Php::Parameters &params) {
            stream = (StringStream *)params[0].implementation();
        }
        Php::Value readRaw(Php::Parameters &params) {
            int n = (int)params.size();
            if (n >= 1) {
                return &readRaw(*(StringStream *)params[0].implementation());
            }
            return Php::Object("HproseStringStream", readRaw());
        }
        static void unexpectedTag(Php::Parameters &params) {
            int n = (int)params.size();
            if (n >= 1) {
                std::string tag = params[0].stringValue();
                std::string expectTags = "";
                if (n >= 2) {
                    expectTags = params[1].stringValue();
                }
                if (tag.empty()) {
                    unexpectedTag(0, expectTags);
                }
                else {
                    unexpectedTag(tag[0], expectTags);
                }
            }
            else {
                unexpectedTag(0);
            }
        }
    };

    inline void publish_rawreader(Php::Extension &ext) {
        Php::Class<RawReader> c("HproseRawReader");
        c.method("__construct",
                 &Hprose::RawReader::__construct,
                 { Php::ByVal("stream", "HproseStringStream") })
         .method("unexpectedTag",
                 &Hprose::RawReader::unexpectedTag,
                 {
                     Php::ByVal("tag", Php::Type::String, false),
                     Php::ByVal("expectTags", Php::Type::String, false)
                 })
         .method("readRaw",
                 &Hprose::RawReader::readRaw,
                 {
                     Php::ByVal("ostream", "HproseStringStream", false, false)
                 });
        ext.add(std::move(c));
    }
}
#endif /* HPROSE_RAWREADER_H_ */
