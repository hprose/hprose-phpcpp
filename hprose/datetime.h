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
 * hprose/datetime.h                                      *
 *                                                        *
 * hprose datetime class for php-cpp.                     *
 *                                                        *
 * LastModified: Jun 29, 2014                             *
 * Author: Ma Bingyao <andot@hprose.com>                  *
 *                                                        *
\**********************************************************/

#ifndef HPROSE_DATETIME_H_
#define HPROSE_DATETIME_H_

#include <phpcpp.h>
#include <sys/time.h>
#include "date.h"
#include "time.h"

namespace Hprose {

    class DateTime: public Date {
    private:
        inline void init_time(int32_t hour, int32_t minute, int32_t second, int32_t microsecond = 0) {
            if (is_valid_time(hour, minute, second, microsecond)) {
                timebuf.tm_hour = hour;
                timebuf.tm_min = minute;
                timebuf.tm_sec = second;
                usec = microsecond;
            }
            else {
                throw Php::Exception("Unexpected arguments");
            }
        }
    public:
        inline static bool is_valid_time(int32_t hour,
                                         int32_t minute,
                                         int32_t second,
                                         int32_t microsecond = 0) {
            return Time::is_valid_time(hour, minute, second, microsecond);
        }
        DateTime(): Date() {
            is_datetime = true;
        }
        DateTime(const time_t *timer): Date() {
            init(*timer);
            usec = 0;
            is_datetime = true;
        }
        DateTime(int32_t year, int32_t month, int32_t day, bool utc = false) :
            Date(year, month, day, utc) {
            init_time(0, 0, 0, 0);
            is_datetime = true;
        }
        DateTime(int32_t year, int32_t month, int32_t day,
                 int32_t hour, int32_t minute, int32_t second,
                 int32_t microsecond = 0, bool utc = false) :
        Date(year, month, day, utc) {
            init_time(hour, minute, second, microsecond);
            is_datetime = true;
        }
        virtual ~DateTime() {}
        inline bool add_microseconds(int64_t microseconds) {
            if (microseconds == 0) return true;
            microseconds += microsecond();
            int32_t usec = microseconds % 1000000;
            if (usec < 0) usec += 1000000;
            int64_t sec = (microseconds - usec) / 1000000;
            if (add_seconds(sec)) {
                set_microsecond(usec);
                return true;
            }
            return false;
        }
        inline bool add_seconds(int64_t seconds) {
            if (seconds == 0) return true;
            seconds += second();
            int32_t sec = seconds % 60;
            if (sec < 0) sec += 60;
            int64_t min = (seconds - sec) / 60;
            if (add_minutes(min)) {
                set_second(sec);
                return true;
            }
            return false;
        }
        inline bool add_minutes(int64_t minutes) {
            if (minutes == 0) return true;
            minutes += minute();
            int32_t min = minutes % 60;
            if (min < 0) min += 60;
            int32_t h = (int32_t)(minutes - min) / 60;
            if (add_hours(h)) {
                set_minute(min);
                return true;
            }
            return false;
        }
        inline bool add_hours(int32_t hours) {
            if (hours == 0) return true;
            hours += hour();
            int32_t h = hours % 24;
            if (h < 0) h += 24;
            int32_t d = (hours - h) / 24;
            if (add_days(d)) {
                set_hour(h);
                return true;
            }
            return false;
        }
        inline bool after(const DateTime *when) const {
            if (utc != when->utc) return time() > when->time();
            if (year() < when->year()) return false;
            if (year() > when->year()) return true;
            if (month() < when->month()) return false;
            if (month() > when->month()) return true;
            if (day() < when->day()) return false;
            if (day() > when->day()) return true;
            if (hour() < when->hour()) return false;
            if (hour() > when->hour()) return true;
            if (minute() < when->minute()) return false;
            if (minute() > when->minute()) return true;
            if (second() < when->second()) return false;
            if (second() > when->second()) return true;
            if (microsecond() < when->microsecond()) return false;
            if (microsecond() > when->microsecond()) return true;
            return false;
        }
        inline bool before(const DateTime *when) const {
            if (utc != when->utc) return time() < when->time();
            if (year() < when->year()) return true;
            if (year() > when->year()) return false;
            if (month() < when->month()) return true;
            if (month() > when->month()) return false;
            if (day() < when->day()) return true;
            if (day() > when->day()) return false;
            if (hour() < when->hour()) return true;
            if (hour() > when->hour()) return false;
            if (minute() < when->minute()) return true;
            if (minute() > when->minute()) return false;
            if (second() < when->second()) return true;
            if (second() > when->second()) return false;
            if (microsecond() < when->microsecond()) return true;
            if (microsecond() > when->microsecond()) return false;
            return false;
        }
        inline bool equals(const DateTime *when) const {
            if (utc != when->utc) return time() == when->time();
            return (year() == when->year() &&
                    month() == when->month() &&
                    day() == when->day() &&
                    hour() == when->hour() &&
                    minute() == when->minute() &&
                    second() == when->second() &&
                    microsecond() == when->microsecond());
        }
        inline double time() const {
            static tm tb = timebuf;
            return ((double)(utc ? timegm(&tb) : mktime(&tb)) +
                    (double)microsecond() / 1000000.0);
        }
        inline int format(char *str, bool fullformat = true) const {
            const char *format;
            int n;
            if (usec == 0) {
                format = fullformat ? "%04d-%02d-%02dT%02d:%02d:%02d" : "%04d%02d%02dT%02d%02d%02d";
                n = sprintf(str, format, year(), month(), day(), hour(), minute(), second());
            }
            else if (usec % 1000 == 0) {
                format = fullformat ? "%04d-%02d-%02dT%02d:%02d:%02d.%03d" : "%04d%02d%02dT%02d%02d%02d.%03d";
                n = sprintf(str, format, year(), month(), day(), hour(), minute(), second(), usec / 1000);
            }
            else {
                format = fullformat ? "%04d-%02d-%02dT%02d:%02d:%02d.%06d" : "%04d%02d%02dT%02d%02d%02d.%06d";
                n = sprintf(str, format, year(), month(), day(), hour(), minute(), second(), usec);
            }
            if (utc) {
                str[n++] = 'Z';
                str[n] = 0;
            }
            return n;
        }
        inline std::string to_string(bool fullformat = true) const {
            char buffer[32];
            int n = format(buffer, fullformat);
            return std::string(buffer, n);
        }
        // -----------------------------------------------------------
        // for PHP
        void __construct(Php::Parameters &params) {
            is_datetime = true;
            size_t n = params.size();
            switch (n) {
                case 0: {
                    init(::time(NULL));
                    struct timeval tv;
                    gettimeofday(&tv, NULL);
                    usec = tv.tv_usec;
                    break;
                }
                case 1: {
                    Php::Value &val = params[0];
                    if (val.isNumeric()) {
                        init(val.numericValue());
                        usec = 0;
                    }
                    else if (val.isString()) {
                        init(Php::call("strtotime", val).numericValue());
                        usec = 0;
                    }
                    else if (val.isArray()) {
                        init(val.get("year", 4),
                             val.get("mon", 3),
                             val.get("mday", 4),
                             false);
                        init_time(val.get("hours", 5),
                                  val.get("minutes", 7),
                                  val.get("seconds", 7));
                    }
                    else if (val.isObject()) {
                        if (Php::call("is_a", val, "HproseDateTime")) {
                            DateTime *datetime = (DateTime *)val.implementation();
                            timebuf = datetime->timebuf;
                            usec = datetime->usec;
                        }
                        else if (Php::call("is_a", val, "HproseDate")) {
                            Date *date = (Date *)val.implementation();
                            timebuf = date->timebuf;
                            init_time(0, 0, 0, 0);
                        }
                        else if (Php::call("is_a", val, "HproseTime")) {
                            Time *time = (Time *)val.implementation();
                            init(1970, 1, 1, time->utc);
                            init_time(time->hour, time->minute, time->second, time->microsecond);
                        }
                        else {
                            throw Php::Exception("Unexpected arguments");
                        }
                    }
                    else {
                        throw Php::Exception("Unexpected arguments");
                    }
                    break;
                }
                case 2: {
                    Php::Value &v1 = params[0], &v2 = params[1];
                    if (v1.isObject() && v2.isObject() &&
                        Php::call("is_a", v1, "HproseDate") &&
                        Php::call("is_a", v2, "HproseTime")) {
                        Date *date = (Date *)v1.implementation();
                        timebuf = date->timebuf;
                        utc = date->utc;
                        Time *time = (Time *)v2.implementation();
                        init_time(time->hour, time->minute, time->second, time->microsecond);
                    }
                    else {
                        throw Php::Exception("Unexpected arguments");
                    }
                    break;
                }
                case 3: {
                    init(params[0], params[1], params[2], false);
                    init_time(0, 0, 0, 0);
                    break;
                }
                case 4: {
                    init(params[0], params[1], params[2], params[3]);
                    init_time(0, 0, 0, 0);
                    break;
                }
                case 6: {
                    init(params[0], params[1], params[2], false);
                    init_time(params[3], params[4], params[5], 0);
                    break;
                }
                case 7: {
                    init(params[0], params[1], params[2], false);
                    init_time(params[3], params[4], params[5], params[6]);
                    break;
                }
                case 8: {
                    init(params[0], params[1], params[2], params[7]);
                    init_time(params[3], params[4], params[5], params[6]);
                    break;
                };
                default: {
                    throw Php::Exception("Unexpected arguments");
                }
            }
        }
        Php::Value addMicroseconds(Php::Parameters &params) {
            Php::Value &val = params[0];
            if (val.isNumeric()) {
                return add_microseconds(val);
            }
            return false;
        }
        Php::Value addSeconds(Php::Parameters &params) {
            Php::Value &val = params[0];
            if (val.isNumeric()) {
                return add_seconds(val);
            }
            return false;
        }
        Php::Value addMinutes(Php::Parameters &params) {
            Php::Value &val = params[0];
            if (val.isNumeric()) {
                return add_minutes(val);
            }
            return false;
        }
        Php::Value addHours(Php::Parameters &params) {
            Php::Value &val = params[0];
            if (val.isNumeric()) {
                return add_hours(val);
            }
            return false;
        }
        Php::Value timestamp() const {
            return time();
        }
        Php::Value after(Php::Parameters &params) const {
            Php::Value &val = params[0];
            if (!val.isObject() || !Php::call("is_a", val, "HproseDateTime")) {
                val = Php::Object("HproseDateTime", val);
            }
            return after((DateTime *)val.implementation());
        }
        Php::Value before(Php::Parameters &params) const {
            Php::Value &val = params[0];
            if (!val.isObject() || !Php::call("is_a", val, "HproseDateTime")) {
                val = Php::Object("HproseDateTime", val);
            }
            return before((DateTime *)val.implementation());
        }
        Php::Value equals(Php::Parameters &params) const {
            Php::Value &val = params[0];
            if (!val.isObject() || !Php::call("is_a", val, "HproseDateTime")) {
                val = Php::Object("HproseDateTime", val);
            }
            return equals((DateTime *)val.implementation());
        }
        Php::Value toString(Php::Parameters &params) const {
            bool fullformat = true;
            if (params.size() > 0) {
                fullformat = params[0];
            }
            Php::Value result;
            char *buffer = result.reserve(32);
            int n = format(buffer, fullformat);
            result.reserve(n);
            return result;
        }
        Php::Value __toString() const {
            Php::Value result;
            char *buffer = result.reserve(32);
            int n = format(buffer);
            result.reserve(n);
            return result;
        }
    };
    inline void publish_datetime(Php::Extension &ext, Php::Class<Hprose::Date> p) {
        Php::Class<Hprose::DateTime> c("HproseDateTime");
        c.extends(p)
        .method("__construct",
                &Hprose::DateTime::__construct,
                {
                     Php::ByVal("arg1", Php::Type::Null, false),
                     Php::ByVal("arg2", Php::Type::Numeric, false),
                     Php::ByVal("arg3", Php::Type::Numeric, false),
                     Php::ByVal("arg4", Php::Type::Null, false),
                     Php::ByVal("arg5", Php::Type::Numeric, false),
                     Php::ByVal("arg6", Php::Type::Numeric, false),
                     Php::ByVal("arg7", Php::Type::Numeric, false),
                     Php::ByVal("arg8", Php::Type::Bool, false)
                })
        .method("addMicroseconds",
                &Hprose::DateTime::addMicroseconds,
                { Php::ByVal("microseconds", Php::Type::Numeric) })
        .method("addSeconds",
                &Hprose::DateTime::addSeconds,
                { Php::ByVal("seconds", Php::Type::Numeric) })
        .method("addMinutes",
                &Hprose::DateTime::addMinutes,
                { Php::ByVal("minutes", Php::Type::Numeric) })
        .method("addHours",
                &Hprose::DateTime::addHours,
                { Php::ByVal("hours", Php::Type::Numeric) })
        .method("after",
                &Hprose::DateTime::after,
                { Php::ByVal("when", Php::Type::Null) })
        .method("before",
                &Hprose::DateTime::before,
                { Php::ByVal("when", Php::Type::Null) })
        .method("equals",
                &Hprose::DateTime::equals,
                { Php::ByVal("when", Php::Type::Null) })
        .method("timestamp", &Hprose::DateTime::timestamp)
        .method("toString",
                &Hprose::DateTime::toString,
                { Php::ByVal("fullformat", Php::Type::Bool, false) })
        .method("__toString", &Hprose::DateTime::__toString)
        .method("isValidTime",
                &Hprose::Time::isValidTime,
                Php::Public | Php::Static,
                {
                    Php::ByVal("hour", Php::Type::Numeric),
                    Php::ByVal("minute", Php::Type::Numeric),
                    Php::ByVal("second", Php::Type::Numeric),
                    Php::ByVal("microsecond", Php::Type::Numeric, false)
                });
        ext.add(std::move(c));
    }

}
#endif /* HPROSE_DATETIME_H_ */
