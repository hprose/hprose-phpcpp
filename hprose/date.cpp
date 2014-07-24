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
 * hprose/date.cpp                                        *
 *                                                        *
 * hprose date class for php-cpp.                         *
 *                                                        *
 * LastModified: Jun 24, 2014                             *
 * Author: Ma Bingyao <andot@hprose.com>                  *
 *                                                        *
\**********************************************************/

#include "date.h"

namespace Hprose {
#ifdef WIN32
    const long timezone_diff_secs = _timezone_diff_secs();
#endif
    const int32_t Date::days_to_month_365[] = { 0,
                                                31,
                                                59,
                                                90,
                                                120,
                                                151,
                                                181,
                                                212,
                                                243,
                                                273,
                                                304,
                                                334,
                                                365 };
    const int32_t Date::days_to_month_366[] = { 0,
                                                31,
                                                60,
                                                91,
                                                121,
                                                152,
                                                182,
                                                213,
                                                244,
                                                274,
                                                305,
                                                335,
                                                366 };
}
