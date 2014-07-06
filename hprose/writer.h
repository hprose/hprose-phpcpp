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
 * hprose/writer.h                                        *
 *                                                        *
 * hprose writer class for php-cpp.                       *
 *                                                        *
 * LastModified: Jul 6, 2014                              *
 * Author: Ma Bingyao <andot@hprose.com>                  *
 *                                                        *
\**********************************************************/

#ifndef HPROSE_WRITER_H_
#define HPROSE_WRITER_H_

#include <unordered_map>
#include <phpcpp.h>
#include <math.h>

namespace Hprose {
    class WriterRefer {
    public:
        inline virtual void set(const Php::Value &value) = 0;
        inline virtual bool write(const Php::Value &value) = 0;
        inline virtual void reset() = 0;
        WriterRefer() {}
        virtual ~WriterRefer() {}
    };

    class FakeWriterRefer : public WriterRefer {
    public:
        inline virtual void set(const Php::Value &value) override {}
        inline virtual bool write(const Php::Value &value) override {
            return false;
        };
        inline virtual void reset() override {};
        FakeWriterRefer() {}
        virtual ~FakeWriterRefer() {}
    };

    struct HashPhpValue {
        size_t operator()(const Php::Value &key) const {
            return key.hash();
        }
    };
    struct EqualPhpValue {
        bool operator()(const Php::Value &k1, const Php::Value &k2) const {
            Php::Type t1 = k1.type();
            Php::Type t2 = k2.type();
            if (t1 != t2) return false;
            switch (t1) {
                case Php::Type::Array: return k1.refequals(k2);
                case Php::Type::Object: return k1.id() == k2.id();
                default: return k1 == k2;
            }
        }
    };

    class RealWriterRefer : public WriterRefer {
    private:
        std::unordered_map<Php::Value, int32_t, HashPhpValue, EqualPhpValue> ref;
        int32_t refcount;
        StringStream *stream;
    public:
        RealWriterRefer(StringStream *stream):stream(stream) {
            reset();
        }
        virtual ~RealWriterRefer() {}
        inline virtual void set(const Php::Value &value) override {
            //if (value.isArray()) {
                ref[value.ref()] = refcount++;
            //}
            //else {
            //    ref[value] = refcount++;
           // }
        }
        inline virtual bool write(const Php::Value &value) override {
            auto it = ref.find(value);
            if (it != ref.end()) {
                stream->write(TagRef).write(it->second).write(TagSemicolon);
                return true;
            }
            return false;
        };
        inline virtual void reset() override {
            ref.clear();
            refcount = 0;
        };
    };

    class Writer : public Php::Base {
    private:
        std::map<std::string, int32_t> classref;
        std::vector<std::vector<std::string>> fieldsref;
        WriterRefer *refer;
        inline void init_refer(bool simple) {
            if (simple) {
                refer = new FakeWriterRefer();
            }
            else {
                refer = new RealWriterRefer(stream);
            }
        }
    public:
        StringStream *stream;
        Writer() {};
        Writer(StringStream &stream, bool simple = false) : stream(&stream) {
            init_refer(simple);
        }
        virtual ~Writer() {
            reset();
            delete refer;
        }
        inline void reset() {
            classref.clear();
            fieldsref.clear();
            refer->reset();
        }
        inline void writeInteger(int32_t i) {
            stream->write(TagInteger).write(i).write(TagSemicolon);
        }
        inline void writeLong(int64_t i) {
            stream->write(TagLong).write(i).write(TagSemicolon);
        }
        inline void writeDouble(double d) {
            if (isnan(d)) {
                stream->write(TagNaN);
            }
            else if (isinf(d)) {
                writeInfinity(d > 0);
            }
            else {
                stream->write(TagDouble).write(d).write(TagSemicolon);
            }
        }
        inline void writeNaN() {
            stream->write(TagNaN);
        }
        inline void writeInfinity(bool positive) {
            stream->write(TagInfinity).write(positive ? TagPos : TagNeg);
        }
        inline void writeNull() {
            stream->write(TagNull);
        }
        inline void writeEmpty() {
            stream->write(TagEmpty);
        }
        inline void writeBoolean(bool boolean) {
            stream->write(boolean ? TagTrue : TagFalse);
        }
        void writeDateTime(const Php::Value &value) {
            refer->set(value);
            Php::Array getOffset;
            getOffset[0] = value;
            getOffset[1] = "getOffset";
            Php::Array format;
            format[0] = value;
            format[1] = "format";
            if (getOffset() == 0) {
                stream->write(format("\\DYmd\\THis.u\\Z").stringValue());
            }
            else {
                stream->write(format("\\DYmd\\THis.u;").stringValue());
            }
        }
        void writeDateTimeWithRef(const Php::Value &value) {
            if (!refer->write(value)) writeDateTime(value);
        }
        void writeHproseDateTime(const Php::Value &value) {
            refer->set(value);
            DateTime *date = (DateTime *)value.implementation();
            stream->write(TagDate).write(date->to_string(false));
            if (!date->utc) {
                stream->write(TagSemicolon);
            }
        }
        void writeHproseDateTimeWithRef(const Php::Value &value) {
            if (!refer->write(value)) writeHproseDateTime(value);
        }
        void writeHproseDate(const Php::Value &value) {
            refer->set(value);
            Date *date = (Date *)value.implementation();
            stream->write(TagDate).write(date->to_string(false));
            if (!date->utc) {
                stream->write(TagSemicolon);
            }
        }
        void writeHproseDateWithRef(const Php::Value &value) {
            if (!refer->write(value)) writeHproseDate(value);
        }
        void writeHproseTime(const Php::Value &value) {
            refer->set(value);
            Time *time = (Time *)value.implementation();
            stream->write(TagTime).write(time->to_string(false));
            if (!time->utc) {
                stream->write(TagSemicolon);
            }
        }
        void writeHproseTimeWithRef(const Php::Value &value) {
            if (!refer->write(value)) writeHproseTime(value);
        }
        void writeBytes(const Php::Value &value) {
            refer->set(value);
            std::string bytes = (value.className() == "HproseBytes" ?
                                 ((Bytes *)value.implementation())->value :
                                 value.stringValue());
            int32_t len = (int32_t)bytes.size();
            stream->write(TagBytes);
            if (len > 0) stream->write(len);
            stream->write(TagQuote).write(bytes).write(TagQuote);
        }
        void writeBytesWithRef(const Php::Value &value) {
            if (!refer->write(value)) writeBytes(value);
        }
        void writeUTF8Char(const std::string c) {
            stream->write(TagUTF8Char).write(c);
        }
        void writeString(const Php::Value &value) {
            refer->set(value);
            int32_t len = ustrlen(value);
            stream->write(TagString);
            if (len > 0) stream->write(len);
            stream->write(TagQuote).write(value.stringValue()).write(TagQuote);
        }
        void writeStringWithRef(const Php::Value &value) {
            if (!refer->write(value)) writeString(value);
        }
        void writeList(const Php::Value &value) {
            refer->set(value);
            int32_t count = value.size();
            stream->write(TagList);
            if (count > 0) stream->write(count);
            stream->write(TagOpenbrace);
            for (int32_t i = 0; i < count; ++i) {
                serialize(value[i]);
            }
            stream->write(TagClosebrace);
        }
        void writeListWithRef(const Php::Value &value) {
            if (!refer->write(value)) writeList(value);
        }
        void writeMap(const Php::Value &value) {
            refer->set(value);
            const Php::Value &val = (value.className() == "HproseMap" ?
                                     ((Map *)value.implementation())->value :
                                     value);
            std::vector<Php::Value> keys = val.keys();
            int32_t count = (int32_t)keys.size();
            stream->write(TagMap);
            if (count > 0) stream->write(count);
            stream->write(TagOpenbrace);
            for (int32_t i = 0; i < count; ++i) {
                serialize(keys[i]);
                serialize(value.get(keys[i]));
            };
            stream->write(TagClosebrace);
        }
        void writeMapWithRef(const Php::Value &value) {
            if (!refer->write(value)) writeMap(value);
        }
        int32_t writeClass(const std::string alias, const std::vector<std::string> &fields) {
            stream->write(TagClass).write((int32_t)alias.size()).write(TagQuote).write(alias).write(TagQuote);
            int32_t count = (int32_t)fields.size();
            if (count > 0) stream->write(count);
            stream->write(TagOpenbrace);
            for (int32_t i = 0; i < count; ++i) {
                std::string field = fields[i];
                if (field[0] == '\0') {
                    field = field.substr(field.find('\0', 1) + 1);
                }
                writeString(field);
            }
            stream->write(TagClosebrace);
            int32_t index = (int32_t)fieldsref.size();
            fieldsref.push_back(fields);
            classref[alias] = index;
            return index;
        }
        void writeObject(const Php::Value &value) {
            std::string classname = value.className();
            std::string alias = ClassManager::get_alias(classname);
            std::vector<std::string> fields;
            int32_t index;
            auto find = classref.find(alias);
            if (find == classref.end()) {
                fields = value.properties(false);
                index = writeClass(alias, fields);
            }
            else {
                index = classref[alias];
                fields = fieldsref[index];
            }
            refer->set(value);
            int32_t count = (int32_t)fields.size();
            stream->write(TagObject).write(index).write(TagOpenbrace);
            for (int32_t i = 0; i < count; ++i) {
                serialize(value.get(fields[i]));
            }
            stream->write(TagClosebrace);
        }
        void writeObjectWithRef(const Php::Value &value) {
            if (!refer->write(value)) writeObject(value);
        }
        void serialize(const Php::Value &value) {
            switch (value.type()) {
                case Php::Type::Null:
                    writeNull();
                    break;
                case Php::Type::Numeric: {
                    int64_t i = value.numericValue();
                    if (i >= 0 && i <= 9) {
                        stream->write((char)('0' + i));
                    }
                    else if (i >= INT32_MIN && i <= INT32_MAX) {
                        writeInteger((int32_t)i);
                    }
                    else {
                        writeLong(i);
                    }
                    break;
                }
                case Php::Type::Float:
                    writeDouble(value.floatValue());
                    break;
                case Php::Type::Bool:
                    writeBoolean(value.boolValue());
                    break;
                case Php::Type::String:
                    if (value.size() == 0) {
                        writeEmpty();
                    }
                    else if ((value.size() < 4) && is_utf8(value) && (ustrlen(value) == 1)) {
                        writeUTF8Char(value);
                    }
                    else if (is_utf8(value)) {
                        writeStringWithRef(value);
                    }
                    else {
                        writeBytesWithRef(value);
                    }
                    break;
                case Php::Type::Array:
                    if (value.isList()) {
                        writeListWithRef(value);
                    }
                    else {
                        writeMapWithRef(value);
                    }
                    break;
                case Php::Type::Object: {
                    std::string className = value.className();
                    if (className == "stdClass") {
                        writeMapWithRef(value);
                    }
                    else if (className == "DateTime") {
                        writeDateTimeWithRef(value);
                    }
                    else if (className == "HproseDateTime") {
                        writeHproseDateTimeWithRef(value);
                    }
                    else if (className == "HproseDate") {
                        writeHproseDateWithRef(value);
                    }
                    else if (className == "HproseTime") {
                        writeHproseTimeWithRef(value);
                    }
                    else if (className == "HproseBytes") {
                        writeBytesWithRef(value);
                    }
                    else if (className == "HproseMap") {
                        writeMapWithRef(value);
                    }
                    else {
                        writeObjectWithRef(value);
                    }
                    break;
                }
                default:
                    throw Php::Exception("Not support to serialize this data");
                    break;
            }
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
        void serialize(Php::Parameters &params) {
            serialize(params[0]);
        }
    };
    inline void publish_writer(Php::Extension &ext) {
        Php::Class<Writer> c("HproseWriter");
        c.method("__construct",
                 &Hprose::Writer::__construct,
                 {
                     Php::ByRef("stream", "HproseStringStream"),
                     Php::ByVal("simple", Php::Type::Bool, false)
                 })
        .method("serialize",
                &Hprose::Writer::serialize,
                {
                    Php::ByRef("var", Php::Type::Null)
                })
        .method("reset", &Hprose::Writer::reset);
        ext.add(std::move(c));
    }

}

#endif /* HPROSE_WRITER_H_ */