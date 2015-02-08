/**
 * @file
 * @author Rahimian, Abtin <arahimian@acm.org>
 * @date   2014-08-26 16:26
 *
 * @brief Logger singleton class as well as logging and printing macros
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

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <iostream>
#include <iomanip>  //for setpercision
#include <map>
#include <omp.h>
#include <stack>
#include <string>
#include <cassert>
// #include <sys/time.h>

#include "ves3d_common.h"

//! A log event recorded by the Logger class.
struct LogEvent
{
    std::string       fun_name;
    unsigned long int num_calls;
    double            time;
    double            flop;
    double            flop_rate;
};

//! The print function for the LogEvent class.
template<typename T>
void PrintLogEvent(const std::pair<T, LogEvent>& ev);

//! The enum type for the format of the report generated by
//! Logger::Report().
enum ReportFormat {SortFunName, SortNumCalls, SortTime,
                   SortFlop, SortFlopRate};

//! A singleton class handling the logging.
class Logger
{
  public:
    //! Returns the current wall-time.
    static double Now();

    //! Gets the current wall-time, saves it in a stack and also
    //! returns it as output.
    static double Tic();

    //! Gets the current wall-time, pops the corresponding Tic() value
    //! form the stack, i.e. the last Tic(), and returns the difference.
    static double Toc();

    //! Records accumulative log events corresponding to functions
    //! calling this method.
    static void Record(std::string fun_name, std::string prefix, double time,
        double flop);

    //! Reports all accumulative data corresponding to all the calls to
    //! the Record() function.
    static void Report(enum ReportFormat rf);

    //! The setter function of the log file.
    static void SetLogFile(std::string file_name);

    //! The method to log events, it writes the event to the file
    //! log_file.
    static void Log(const char *event);

    //! Log event to file
    static void Log(std::ostream &event);

    //! Clears the slate of profiler
    static void PurgeProfileHistory();

    static double GetFlops();

  private:
    //! The constructor, since the class is a singleton class, the
    //! constructor is set private to avoid instantiation of the
    //! object.
    Logger();

    //! The map holding the data corresponding the Record() method.
    static std::map<std::string, LogEvent> PrflMap;

    //! The stack used by Tic() and Toc().
    static std::stack<double> TicStack;

    //! The stack to have cumulative
    static std::stack<double> FlopStack;

    //! The file name for the logger.
    static std::string log_file;
};

/*
 * Profiling Macros
 */
#ifdef PROFILING
#define PROFILESTART() (Logger::Tic())
#define PROFILEEND(str,flps) (                                  \
        Logger::Record(__FUNCTION__, str, Logger::Toc(), flps))
#define PROFILECLEAR() (Logger::PurgeProfileHistory())
#define PROFILEREPORT(format) (Logger::Report(format))
#define PROFILEING_EXPR(expr) (expr)
#else
#define PROFILESTART()
#define PROFILEEND(str,flps)
#define PROFILECLEAR()
#define PROFILEREPORT(format)
#define PROFILEING_EXPR(expr)
#endif //PROFILING

/*
 * stream manipulator (should check TERM)
 */
static int alert_xalloc(std::ios_base::xalloc());
static int emph_xalloc(std::ios_base::xalloc());

std::ostream& alert(std::ostream& os);
std::ostream& emph(std::ostream& os);


/*
 * Debugging macros
 */
#ifndef NDEBUG
// defineing assert_expr to avoid multiple evaluation of expr
static bool assert_expr(false);
#define ASSERT(expr,msg) (                                              \
        assert_expr=expr,                                               \
        (assert_expr) ?                                                 \
        assert(assert_expr) :                                           \
        CERR_LOC(msg,"",assert(assert_expr)))

#else  //NDEBUG

#define ASSERT(expr,msg)

#endif //NDEBUG

/*
 * Printing macro
 */
#define SCI_PRINT_FRMT std::scientific<<std::setprecision(4)

#ifdef VERBOSE
#define COUTDEBUG(str) (std::cout<<"[DEBUG]["<<__FUNCTION__<<"] "<<str<<std::endl)
#define WHENVERBOSE(expr) expr
#else
#define COUTDEBUG(str)
#define WHENVERBOSE(expr)
#endif //VERBOSE

#define COUT(str) (std::cout<<str<<std::endl)
#define COUTMASTER(str) (std::cout<<str<<std::endl)
#ifdef QUIET
#define INFO(str)
#define WHENQUIET(expr)
#else
#define WHENQUIET(expr) expr
#define INFO(str) (std::cout<<"[INFO]["<<__FUNCTION__<<"] "<<str<<std::endl)
#endif //QUIET

#define LOG(msg) (Logger::Log(msg))

#define CERR(msg) (std::cerr<<alert<<"[ERROR] "<<msg<<alert<<std::endl)
#define WARN(msg) (std::cerr<<alert<<"[WARNING] "<<msg<<alert<<std::endl)
#define CERR_LOC(pre_msg,post_msg,action) (                             \
        std::cerr<<alert<<"[ERROR] "<<pre_msg                           \
        <<"\n    File           : "<< __FILE__                          \
        <<"\n    Line           : "<< __LINE__                          \
        <<"\n    Function       : "<< __FUNCTION__                      \
        <<"\n    Ves3D Version  : "<< VERSION                           \
        <<"\n"<<post_msg                                                \
        <<alert<<std::endl,                                             \
        action                                                          \
                                                                        )
//Timing macro
#define GETSECONDS()(omp_get_wtime())

// #define GETSECONDS()(                                       \
//         gettimeofday(&tt, &ttz),                            \
//         (double)tt.tv_sec + (double)tt.tv_usec / 1000000.0)

#endif //_LOGGER_H_
