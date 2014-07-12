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
 * hprose/httpserver.h                                    *
 *                                                        *
 * hprose http server class for php-cpp.                  *
 *                                                        *
 * LastModified: Jul 11, 2014                             *
 * Author: Ma Bingyao <andot@hprose.com>                  *
 *                                                        *
\**********************************************************/

#ifndef HPROSE_HTTPSERVER_H_
#define HPROSE_HTTPSERVER_H_

/*

#include <unordered_map>
#include <regex>
#include <phpcpp.h>

namespace Hprose {

    const int32_t E_ERROR             = 1;
    const int32_t E_WARNING           = 2;
    const int32_t E_PARSE             = 4;
    const int32_t E_NOTICE            = 8;
    const int32_t E_CORE_ERROR        = 16;
    const int32_t E_CORE_WARNING      = 32;
    const int32_t E_COMPILE_ERROR     = 64;
    const int32_t E_COMPILE_WARNING   = 128;
    const int32_t E_USER_ERROR        = 256;
    const int32_t E_USER_WARNING      = 512;
    const int32_t E_USER_NOTICE       = 1024;
    const int32_t E_STRICT            = 2048;
    const int32_t E_RECOVERABLE_ERROR = 4096;
    const int32_t E_DEPRECATED        = 8192;
    const int32_t E_USER_DEPRECATED   = 16384;

#if PHP_VERSION_ID >= 50400
    const int32_t E_ALL               = 32767;
#elif PHP_VERSION_ID >= 50300
    const int32_t E_ALL               = 30719;
#elif PHP_VERSION_ID >= 50200
    const int32_t E_ALL               = 6143;
#else
    const int32_t E_ALL               = 2047;
#endif

    inline std::string error_message(int32_t error_code) {
        switch (error_code) {
            case E_ERROR: return "Error";
            case E_WARNING: return "Warning";
            case E_PARSE: return "Parse Error";
            case E_NOTICE: return "Notice";
            case E_CORE_ERROR: return "Core Error";
            case E_CORE_WARNING: return "Core Warning";
            case E_COMPILE_ERROR: return "Compile Error";
            case E_COMPILE_WARNING: return "Compile Warning";
            case E_USER_ERROR: return "User Error";
            case E_USER_WARNING: return "User Warning";
            case E_USER_NOTICE: return "User Notice";
            case E_STRICT: return "Run-time Notice";
            case E_RECOVERABLE_ERROR: return "Recoverable Error";
            case E_DEPRECATED: return "Deprecated";
            case E_USER_DEPRECATED: return "User Deprecated";
            default: return "Unknown Error";
        }
    }

    const char *magic_methods[] = {
        "__construct",
        "__destruct",
        "__call",
        "__callStatic",
        "__get",
        "__set",
        "__isset",
        "__unset",
        "__sleep",
        "__wakeup",
        "__toString",
        "__invoke",
        "__set_state",
        "__clone"
    };


    class HttpServer : public Php::Base {
    private:
        std::unordered_map<std::string, Php::Value> functions;
        std::unordered_map<std::string, std::string> funcnames;
        std::unordered_map<std::string, int32_t> resultmodes;
        std::unordered_map<std::string, Php::Value> simplemodes;
        std::vector<Php::Value> filters;
        std::set<std::string> origins;
        bool debug = false;
        bool crossdomain = false;
        bool p3p = false;
        bool get = true;
        bool simple = false;
        int32_t error_type = E_ALL & ~E_NOTICE;
        std::string error = "";
        Php::Value &inputFilter(Php::Value &data) {
            int32_t count = (int32_t)filters.size();
            for (int32_t i = count - 1; i >= 0; --i) {
                data = filters[i].call("inputFilter", data, this);
            }
            return data;
        }
        Php::Value &outputFilter(Php::Value &data) {
            int32_t count = (int32_t)filters.size();
            for (int32_t i = 0; i < count; ++i) {
                data = filters[i].call("outputFilter", data, this);
            }
            return data;
        }
    public:
        Php::Value __filterHandler(Php::Parameters &params) {
            Php::Value &data = params[0];
            std::regex regex("<b>.*? error<\\/b>:(.*?)<br");
            std::smatch match;
            if (std::regex_match(data.stringValue(), match, regex)) {
                std::string error = std::regex_replace(match[1].str(),
                                                       std::regex(debug ?
                                                                  "<.*?>" :
                                                                  " in <b>.*<\\/b>$"),
                                                       "");
                data = TagError + serialize_string(trim(error)) + TagEnd;
            }
            return outputFilter(data);
        }
        Php::Value __errorHandler(Php::Parameters &params) {
            int32_t err_no = params[0];
            std::string err_str = params[1];
            std::string err_file = params[2];
            std::string err_line = params[3];
            if (debug) err_str += " in " + err_file + " on line " + err_line;
            error = error_message(err_no) + ": " + err_str;
            sendError();
            Php::exit();
        }
    };
}

*/

#endif /* HPROSE_HTTPSERVER_H_ */
