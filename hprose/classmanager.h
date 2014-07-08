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
 * hprose/classmanager.h                                  *
 *                                                        *
 * hprose classmanager class for php-cpp.                 *
 *                                                        *
 * LastModified: Jul 6, 2014                              *
 * Author: Ma Bingyao <andot@hprose.com>                  *
 *                                                        *
\**********************************************************/

#ifndef HPROSE_CLASSMANAGER_H_
#define HPROSE_CLASSMANAGER_H_

#include <phpcpp.h>
#ifdef ZTS
#include <mutex>
#endif

namespace Hprose {

    class ClassManager: public Php::Base {
    private:
        static std::map<std::string, std::string> classCache1;
        static std::map<std::string, std::string> classCache2;
#ifdef ZTS
        static std::mutex mutex;
#endif
        static std::string _get_alias(const std::string &cls) {
#ifdef ZTS
            std::lock_guard<std::mutex> lock(mutex);
#endif
            auto iter = classCache2.find(cls);
            if (iter != classCache2.end()) {
                return iter->second;
            }
            return "";
        }
        static std::string _get_class(const std::string &alias) {
#ifdef ZTS
            std::lock_guard<std::mutex> lock(mutex);
#endif
            auto iter = classCache1.find(alias);
            if (iter != classCache1.end()) {
                return iter->second;
            }
            return "";
        }
    public:
        static void register_class(const std::string &cls, const std::string &alias) {
#ifdef ZTS
            std::lock_guard<std::mutex> lock(mutex);
#endif
            classCache1[alias] = cls;
            classCache2[cls] = alias;
        }
        static std::string get_alias(const std::string &cls) {
            std::string alias = _get_alias(cls);
            if (alias.empty()) {
                alias = cls;
                size_t pos = 0;
                while ((pos = alias.find('\\', pos)) != std::string::npos) {
                    alias[pos] = '_';
                }
            }
            return alias;
        }
        static std::string get_class(const std::string &alias) {
            std::string cls = _get_class(alias);
            if (!cls.empty()) return cls;
            if (!Php::class_exists(alias)) {
                cls = alias;
                size_t pos = 0;
                while ((pos = cls.find('_', pos)) != std::string::npos) {
                    cls[pos] = '\\';
                }
                if (Php::class_exists(cls)) {
                    register_class(cls, alias);
                    return cls;
                }
                Php::eval("class " + alias + " {}");
            }
            return alias;
        }
        // -----------------------------------------------------------
        // for PHP
        static void registerClass(Php::Parameters &params) {
            register_class(params[0].stringValue(), params[1].stringValue());
        }
        static Php::Value getClassAlias(Php::Parameters &params) {
            return get_alias(params[0].stringValue());
        }
        static Php::Value getClass(Php::Parameters &params) {
            return get_class(params[0].stringValue());
        }
    };
    inline void publish_classmanager(Php::Extension &ext) {
        Php::Class<Hprose::ClassManager> c("HproseClassManager");
        c.method("__construct", Php::Private )
         .method("register",
                 &Hprose::ClassManager::registerClass,
                 Php::Static | Php::Public,
                 {
                     Php::ByVal("class", Php::Type::String),
                     Php::ByVal("alias", Php::Type::String)
                 })
         .method("getClassAlias",
                 &Hprose::ClassManager::getClassAlias,
                 Php::Static | Php::Public,
                 {
                     Php::ByVal("class", Php::Type::String)
                 })
         .method("getClass",
                 &Hprose::ClassManager::getClass,
                 Php::Static | Php::Public,
                 {
                     Php::ByVal("alias", Php::Type::String)
                 });
        ext.add(std::move(c));
    }

}

#endif /* HPROSE_CLASSMANAGER_H_ */
