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
 * LastModified: Jul 1, 2014                              *
 * Author: Ma Bingyao <andot@hprose.com>                  *
 *                                                        *
\**********************************************************/

#include "hprose.h"

extern "C" {

    PHPCPP_EXPORT void *get_module() {

        static Php::Extension extension("hprose", "1.4.0");

        Hprose::publish_tags(extension);
        Hprose::publish_datetime(extension, Hprose::publish_date(extension));
        Hprose::publish_time(extension);
        Hprose::publish_stringstream(extension);
        Hprose::publish_classmanager(extension);
        Hprose::publish_resultmode(extension);
        Hprose::publish_filter(extension);
        Hprose::publish_common(extension);
        Hprose::publish_writer(extension);
        Hprose::publish_rawreader(extension);
        Hprose::publish_reader(extension);

        // extension.add("hprose\\serialize", hprose_serialize, {
        //     Php::ByRef("val", Php::Type::Null)
        // });

        return extension;
    }
}
