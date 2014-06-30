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
 * hprose/resultmode.h                                    *
 *                                                        *
 * hprose resultmode library for php-cpp.                 *
 *                                                        *
 * LastModified: Jun 30, 2014                             *
 * Author: Ma Bingyao <andot@hprose.com>                  *
 *                                                        *
\**********************************************************/

#ifndef HPROSE_RESULTMODE_H_
#define HPROSE_RESULTMODE_H_

#include <phpcpp.h>

namespace Hprose {
    const int32_t Normal        = 0;
    const int32_t Serialized    = 1;
    const int32_t Raw           = 2;
    const int32_t RawWithEndTag = 3;

    class ResultMode : public Php::Base {};

    inline void publish_resultmode(Php::Extension &ext) {
        Php::Class<ResultMode> c("HproseResultMode");

        c.property("Normal",        Hprose::Normal,        Php::Const)
         .property("Serialized",    Hprose::Serialized,    Php::Const)
         .property("Raw",           Hprose::Raw,           Php::Const)
         .property("RawWithEndTag", Hprose::RawWithEndTag, Php::Const);

        ext.add(std::move(c));
    }
}
#endif /* HPROSE_RESULTMODE_H_ */
