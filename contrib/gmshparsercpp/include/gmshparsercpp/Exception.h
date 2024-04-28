// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <exception>
#include <string>
#include <vector>
#ifdef GMSHPARSERCPP_WITH_FMT
    #include "fmt/format.h"
#endif

namespace gmshparsercpp {

/// Exception thrown from our code, so that application code can determine where problems originated
/// from
class Exception : public std::exception {
public:
#ifdef GMSHPARSERCPP_WITH_FMT
    template <typename... T>
    Exception(fmt::format_string<T...> format, T... args)
    {
        this->msg = fmt::format(format, std::forward<T>(args)...);
    }
#else
    template <typename... T>
    Exception(const std::string & what, T... args)
    {
        this->msg = what;
    }
#endif

    /// Get the exception message
    const char * what() const noexcept override;

private:
    /// Error message
    std::string msg;
};

} // namespace gmshparsercpp
