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

#ifndef HPROSE_FILTER_H_
#define HPROSE_FILTER_H_

#include <phpcpp.h>

namespace Hprose {

    inline void publish_filter(Php::Extension &ext) {
        Php::Interface i("HproseFilter");
        i.method("inputFilter",
                 {
                     Php::ByVal("data", Php::Type::String),
                     Php::ByVal("context", Php::Type::Null)
                 })
         .method("outputFilter",
                {
                    Php::ByVal("data", Php::Type::String),
                    Php::ByVal("context", Php::Type::Null)
                });
        ext.add(std::move(i));
    }
}
#endif /* HPROSE_FILTER_H_ */
