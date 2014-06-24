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
 * hprose/hprose.cpp                                      *
 *                                                        *
 * hprose for php-cpp.                                    *
 *                                                        *
 * LastModified: Jun 24, 2014                             *
 * Author: Ma Bingyao <andot@hprose.com>                  *
 *                                                        *
\**********************************************************/

#include "hprose.h"

extern "C" {

    PHPCPP_EXPORT void *get_module() {

        static Php::Extension extension("hprose", "1.4.0");

        Php::Namespace hprose("Hprose");

        Hprose::publish_tags(hprose);
        Hprose::publish_date(hprose);

        // extension.add("hprose\\serialize", hprose_serialize, {
        //     Php::ByRef("val", Php::Type::Null)
        // });

        extension.add(std::move(hprose));

        return extension;
    }
}
