/**
 * @file
 * @author Rahimian, Abtin <arahimian@acm.org>
 * @date   2014-08-26 17:49
 *
 * @brief Error handling class (singleton) and macros
 */

/*
 * Copyright (c) 2014, Abtin Rahimian
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _ERROR_H_
#define _ERROR_H_

#include "Enums.h"

class ErrorEvent
{
  public:
    /// lightweight error event that can be handled by
    /// ErrorHandler
    ErrorEvent(const Error_t &err, const char* fun,
        const char* file, int line);

    Error_t err_;
    string  funname_;
    string  filename_;
    int     linenumber_;

    operator bool(){return bool(err_);}
};

std::ostream& operator<<(std::ostream& output,
    const ErrorEvent &ee);

/// Singleton class for error handling
class ErrorHandler
{
  public:
    /// Callback function pointer type
    typedef Error_t (*ErrorCallBack)(const Error_t &);

    /// Updates the currecnt callback and returns the old one
    static ErrorCallBack setErrorCallBack(ErrorCallBack call_back);

    /// Raise an error with optional callback
    static ErrorEvent submitError( ErrorEvent ee,
        ErrorCallBack call_back_in = NULL);
    static ErrorEvent submitError(const Error_t &err, const char* fun,
        const char* file, int line, ErrorCallBack call_back_in = NULL);

    // Error stack lookup
    static bool errorStatus();
    static ErrorEvent peekLastError();
    static void popLastError();
    static void clearErrorHist();
    static void printErrorLog();

  private:
    static Error_t ringTheCallBack(ErrorEvent &ee,
        ErrorCallBack cb = NULL);

    static ErrorCallBack call_back_;
    static stack<ErrorEvent> ErrorStack_;
};

// Error macros
/// CHK(err) with err as Error_t enum, raises an error if err is not Succes.
#define CHK(err) (                 \
        err &&                     \
    ErrorHandler::submitError(err, \
        __FUNCTION__,              \
        __FILE__,                  \
        __LINE__                   \
                              ))

#define CHK_CB(err,callback) (     \
        err &&                     \
    ErrorHandler::submitError(err, \
        __FUNCTION__,              \
        __FILE__,                  \
        __LINE__,                  \
        callback                   \
                              ))

#define SET_ERR_CALLBACK(cb) ( ErrorHandler::setErrorCallBack(cb) )
#define ERRORSTATUS() ( ErrorHandler::errorStatus() )
#define BREAKONERROR() ( {if( !ErrorHandler::errorStatus() ) break;} )
#define PRINTERRORLOG() (ErrorHandler::printErrorLog() )
#define CLEARERRORHIST() (ErrorHandler::clearErrorHist() )

#endif //_ERROR_H_
