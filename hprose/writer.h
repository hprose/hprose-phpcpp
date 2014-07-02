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
 * LastModified: Jul 2, 2014                              *
 * Author: Ma Bingyao <andot@hprose.com>                  *
 *                                                        *
\**********************************************************/

#ifndef HPROSE_WRITER_H_
#define HPROSE_WRITER_H_

#include <phpcpp.h>

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

    inline std::string obj_id(const Php::Value &val) {
        return Php::call("spl_object_hash", val);
    }

    struct ComparePhpValue {
        bool operator()(const Php::Value &k1, const Php::Value &k2) const {
            Php::Type t1 = k1.type();
            Php::Type t2 = k2.type();
            if (t1 != t2) {
                return t1 < t2;
            }
            switch (t1) {
                case Php::Type::Array: return !k1.refequals(k2);
                case Php::Type::Object: return obj_id(k1) < obj_id(k2);
                default: return k1 < k2;
            }
        }
    };

    class RealWriterRefer : public WriterRefer {
    private:
        std::map<const Php::Value, int32_t, ComparePhpValue> ref;
        int32_t refcount;
        StringStream *stream;
    public:
        RealWriterRefer(StringStream *stream):stream(stream) {
            reset();
        }
        virtual ~RealWriterRefer() {}
        inline virtual void set(const Php::Value &value) override {
            ref[value] = refcount++;
        }
        inline virtual bool write(const Php::Value &value) override {
            auto it = ref.find(value);
            if (it != ref.end()) {
                stream->write(TagRef + std::to_string(it->second) + TagSemicolon);
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
        std::vector<std::map<std::string, std::string>> fieldsref;
        WriterRefer *refer;
    public:
        StringStream *stream;
        Writer() {};
        Writer(StringStream &stream, bool simple = false) : stream(&stream) {
            if (simple) {
                refer = new FakeWriterRefer();
            }
            else {
                refer = new RealWriterRefer(&stream);
            }
        }
        virtual ~Writer() {
            delete refer;
        }
        void reset() {
            classref.clear();
            fieldsref.clear();
            refer->reset();
        }
    };
}

#endif /* HPROSE_WRITER_H_ */