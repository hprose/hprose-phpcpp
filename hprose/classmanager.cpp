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
 * hprose/classmanager.cpp                                *
 *                                                        *
 * hprose classmanager class for php-cpp.                 *
 *                                                        *
 * LastModified: Jun 30, 2014                             *
 * Author: Ma Bingyao <andot@hprose.com>                  *
 *                                                        *
\**********************************************************/

#include "classmanager.h"

namespace Hprose {
    std::map<std::string, std::string> ClassManager::classCache1;
    std::map<std::string, std::string> ClassManager::classCache2;
#ifdef ZTS
    std::mutex ClassManager::mutex;
#endif

}
