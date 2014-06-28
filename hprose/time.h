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
 * hprose/time.h                                          *
 *                                                        *
 * hprose time class for php-cpp.                         *
 *                                                        *
 * LastModified: Jun 28, 2014                             *
 * Author: Ma Bingyao <andot@hprose.com>                  *
 *                                                        *
\**********************************************************/

#ifndef HPROSE_TIME_H_
#define HPROSE_TIME_H_

#include <phpcpp.h>
#include <time.h>
#include <sys/time.h>

namespace Hprose {

    class Time: public Php::Base {
    public:
        int32_t hour;
        int32_t minute;
        int32_t second;
        int32_t microsecond;
        bool utc;
        inline void init(const time_t timer, int32_t usec = 0) {
            struct tm timebuf;
            localtime_r(&timer, &timebuf);
            hour = timebuf.tm_hour;
            minute = timebuf.tm_min;
            second = timebuf.tm_sec;
            microsecond = usec;
            utc = false;
        }
        inline void init(int32_t hour,
                         int32_t minute,
                         int32_t second,
                         int32_t microsecond = 0,
                         bool utc = false) {
            if (is_valid_time(hour, minute, second, microsecond)) {
                this->hour = hour;
                this->minute = minute;
                this->second = second;
                this->microsecond = microsecond;
                this->utc = utc;
            }
            else {
                throw Php::Exception("Unexpected arguments");
            }
        }
        inline static bool is_valid_time(int32_t hour,
                                         int32_t minute,
                                         int32_t second,
                                         int32_t microsecond = 0) {
            return !((hour < 0) || (hour > 23) ||
                     (minute < 0) || (minute > 59) ||
                     (second < 0) || (second > 60) ||
                     (microsecond < 0) || (microsecond > 999999));
        }
        Time() {}
        Time(int32_t hour, int32_t minute, int32_t second,
             int32_t microsecond = 0, bool utc = false) :
         hour(hour), minute(minute), second(second), microsecond(microsecond), utc(utc) {}
        virtual ~Time() {}
        inline double time() {
            struct tm timebuf;
            timebuf.tm_year = 70;
            timebuf.tm_mon = 0;
            timebuf.tm_mday = 1;
            timebuf.tm_hour = hour;
            timebuf.tm_min = minute;
            timebuf.tm_sec = second;
            return (double)(utc ? timegm(&timebuf) : mktime(&timebuf)) + (double)microsecond / 1000000.0;
        }
        inline int format(char *str, bool fullformat = true) const {
            const char *format;
            int n;
            if (microsecond == 0) {
                format = fullformat ? "%02d:%02d:%02d" : "%02d%02d%02d";
                n = sprintf(str, format, hour, minute, second);
            }
            else if (microsecond % 1000 == 0) {
                format = fullformat ? "%02d:%02d:%02d.%03d" : "%02d%02d%02d.%03d";
                n = sprintf(str, format, hour, minute, second, microsecond / 1000);
            }
            else {
                format = fullformat ? "%02d:%02d:%02d.%06d" : "%02d%02d%02d.%06d";
                n = sprintf(str, format, hour, minute, second, microsecond);
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
            size_t n = params.size();
            switch (n) {
                case 0: {
                    struct timeval tv;
                    gettimeofday(&tv, NULL);
                    init(tv.tv_sec, tv.tv_usec);
                    break;
                }
                case 1: {
                    Php::Value &val = params[0];
                    if (val.isNumeric()) {
                        init(val.numericValue());
                    }
                    else if (val.isString()) {
                        init(Php::call("strtotime", val).numericValue());
                    }
                    else if (val.isArray()) {
                        init(val.get("hours", 5),
                             val.get("minutes", 7),
                             val.get("seconds", 7));
                    }
                    else if (val.isObject() && Php::call("is_a", val, "Hprose\\Time")) {
                        Time *time = val.implementation<Time>();
                        init(time->hour,
                             time->minute,
                             time->second,
                             time->microsecond,
                             time->utc);
                    }
                    else {
                        throw Php::Exception("Unexpected arguments");
                    }
                    break;
                }
                case 3: {
                    init(params[0], params[1], params[2]);
                    break;
                }
                case 4: {
                    init(params[0], params[1], params[2], params[3]);
                    break;
                }
                case 5: {
                    init(params[0], params[1], params[2], params[3], params[4]);
                    break;
                }
            }
        }
        Php::Value getHour() const {
            return hour;
        }
        void setHour(const Php::Value &hour) {
            this->hour = hour;
        }
        Php::Value getMinute() const {
            return minute;
        }
        void setMinute(const Php::Value &minute) {
            this->minute = minute;
        }
        Php::Value getSecond() const {
            return second;
        }
        void setSecond(const Php::Value &second) {
            this->second = second;
        }
        Php::Value getMicroSecond() const {
            return microsecond;
        }
        void setMicroSecond(const Php::Value &microsecond) {
            this->microsecond = microsecond;
        }
        Php::Value getUtc() const {
            return utc;
        }
        void setUtc(const Php::Value &utc) {
            this->utc = utc;
        }
        Php::Value timestamp() {
            return time();
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
        static Php::Value isValidTime(Php::Parameters &params) {
            if (params.size() == 3) {
                return is_valid_time(params[0], params[1], params[2]);
            }
            else {
                return is_valid_time(params[0], params[1], params[2], params[3]);
            }
        }
    };
    inline void publish_time(Php::Extension &ext) {
        Php::Class<Hprose::Time> time("HproseTime");
        time.method("__construct",
                    &Hprose::Time::__construct,
                    { Php::ByVal("arg1", Php::Type::Null, false),
                      Php::ByVal("arg2", Php::Type::Numeric, false),
                      Php::ByVal("arg3", Php::Type::Numeric, false),
                      Php::ByVal("arg4", Php::Type::Numeric, false),
                      Php::ByVal("arg5", Php::Type::Bool, false) });
        time.property("hour", &Hprose::Time::getHour, &Hprose::Time::setHour);
        time.property("minute", &Hprose::Time::getMinute, &Hprose::Time::setMinute);
        time.property("second", &Hprose::Time::getSecond, &Hprose::Time::setSecond);
        time.property("utc", &Hprose::Time::getUtc, &Hprose::Time::setUtc);
        time.method("timestamp", &Hprose::Time::timestamp);
        time.method("toString",
                    &Hprose::Time::toString,
                    { Php::ByVal("fullformat", Php::Type::Bool, false) });
        time.method("__toString", &Hprose::Time::__toString);
        time.method("isValidTime",
                    &Hprose::Time::isValidTime,
                    Php::Public | Php::Static,
                    { Php::ByVal("hour", Php::Type::Numeric),
                      Php::ByVal("minute", Php::Type::Numeric),
                      Php::ByVal("second", Php::Type::Numeric),
                      Php::ByVal("microsecond", Php::Type::Numeric, false) });
        ext.add(std::move(time));
    }
}

#endif /* HPROSE_TIME_H_ */
