// Copyright (C) 2011  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#ifndef DLIB_TImING_Hh_
#define DLIB_TImING_Hh_

#include <sys/time.h>
#include "misc_api.h"
#include <cstring>
#include "string.h"

#include <iostream>

#include <stdio.h>

#include "print.h"

// ----------------------------------------------------------------------------------------

/*!A timing

    This set of functions is useful for determining how much time is spent
    executing blocks of code.  Consider the following example:

    int main()
    {
        using namespace dlib::timing;
        for (int i = 0; i < 10; ++i)
        {
            // timing block #1
            start(1,"block #1");
            dlib::sleep(500);
            stop(1);

            // timing block #2
            start(2,"block #2");
            dlib::sleep(1000);
            stop(2);
        }

        print();
    }

    This program would output:
        Timing report: 
            block #1: 5.0 seconds
            block #2: 10.0 seconds

    So we spent 5 seconds in block #1 and 10 seconds in block #2



    Additionally, note that you can use an RAII style timing block object.  For
    example, if we wanted to find out how much time we spent in a loop a convenient
    way to do this would be as follows:

    int main()
    {
        using namespace dlib::timing;
        for (int i = 0; i < 10; ++i)
        {
            block tb(1, "main loop");

            dlib::sleep(1500);
        } 

        print();
    }

    This program would output:
        Timing report: 
            block main loop: 15.0 seconds

!*/

// ----------------------------------------------------------------------------------------

namespace dlib
{
    namespace timing
    {
        const int TIME_SLOTS = 500;
        const int NAME_LENGTH = 40;

        inline uint64* time_buf()
        {
            static uint64 buf[TIME_SLOTS] = {0};
            return buf;
        }

        inline char* name_buf(int i, const char* name)
        {
            static char buf[TIME_SLOTS][NAME_LENGTH] = {{0}};
            // if this name buffer is empty then copy name into it
            if (buf[i][0] == '\0')
            {
                using namespace std;
                strncpy(buf[i], name, NAME_LENGTH-1);
                buf[i][NAME_LENGTH-1] = '\0';
            }
            // return the name buffer
            return buf[i];
        }

        inline timestamper& ts()
        {
            static timestamper ts_;
            return ts_;
        }

        inline void start(int i )
        {
            time_buf()[i] -= ts().get_timestamp();
        }

        inline void start(int i, const char* name)
        {
            time_buf()[i] -= ts().get_timestamp();
            name_buf(i,name);
        }

        inline void stop(int i)
        {
            time_buf()[i] += ts().get_timestamp();
        }

        inline void print()
        {
            using namespace std;
            cout << "Timing report: " << endl;

            // figure out how long the longest name is going to be.
            unsigned long max_name_length = 0;
            for (int i = 0; i < TIME_SLOTS; ++i)
            {
                string name;
                // Check if the name buffer is empty.  Use the name it contains if it isn't.
                if (name_buf(i,"")[0] != '\0')
                    name = cast_to_string(i) + ": " + name_buf(i,"");
                else 
                    name = cast_to_string(i);
                max_name_length = std::max<unsigned long>(max_name_length, name.size());
            }

            for (int i = 0; i < TIME_SLOTS; ++i)
            {
                if (time_buf()[i] != 0)
                {
                    double time = time_buf()[i]/1000.0;
                    string name;
                    // Check if the name buffer is empty.  Use the name it contains if it isn't.
                    if (name_buf(i,"")[0] != '\0')
                        name = cast_to_string(i) + ": " + name_buf(i,"");
                    else 
                        name = cast_to_string(i);

                    // make sure the name is always the same length.  Do so by padding with spaces
                    if (name.size() < max_name_length)
                        name += string(max_name_length-name.size(),' ');

                    if (time < 1000)
                        cout << "  " << name << ": " << time << " milliseconds" << endl;
                    else if (time < 1000*1000)
                        cout << "  " << name << ": " << time/1000.0 << " seconds" << endl;
                    else if (time < 1000*1000*60)
                        cout << "  " << name << ": " << time/1000.0/60.0 << " minutes" << endl;
                    else
                        cout << "  " << name << ": " << time/1000.0/60.0/60.0 << " hours" << endl;
                }
            }
        }

        inline void clear()
        {
            for (int i = 0; i < TIME_SLOTS; ++i)
            {
                // clear timing buffer
                time_buf()[i] = 0;
                // clear name buffer
                name_buf(i,"")[0] = '\0';
            }
        }

        struct block
        {
            /*!
                WHAT THIS OBJECT REPRESENTS
                    This is an RAII tool for calling start() and stop()
            !*/

            block(int i):idx(i) {start(idx);}
            block(int i, const char* str):idx(i) {start(idx,str);}
            ~block() { stop(idx); }
            const int idx;
        };

        inline uint64_t get_time(void)
        {
#ifdef __linux__
            struct timespec ts;
            clock_gettime(CLOCK_MONOTONIC, &ts);

            return ((uint64_t)ts.tv_sec) * 1000000000ULL + (uint64_t)ts.tv_nsec;
#else
            struct timeval tv;
            gettimeofday(&tv, NULL);

            return ((uint64_t)tv.tv_sec) * 1000000000ULL + (uint64_t)tv.tv_usec * 1000;
#endif
        }

        struct timer
        {
            timer() { start_ = get_time(); }
            timer(const char *name)
            {
                start_ = get_time();
                name_ = name;
            }

            void end()
            {
                end_ = get_time();

                uint64_t duration = end_ - start_;
                DLIB_PRINT("Timer: %s took %.3f%s",
                           name_,
                           get_duration_ns_print_scale(duration),
                           get_duration_ns_print_scale_suffix(duration));
            }

            ~timer()
            {
                if (!end_)
                    end();
            }

            private:
                uint64_t start_;
                uint64_t end_ = 0;
                const char *name_ = { "" };

                const char *
                get_duration_ns_print_scale_suffix(uint64_t duration_ns)
                {
                    if (duration_ns > 1000000000)
                        return "s";
                    else if (duration_ns > 1000000)
                        return "ms";
                    else if (duration_ns > 1000)
                        return "us";
                    else
                        return "ns";
                }

                float
                get_duration_ns_print_scale(uint64_t duration_ns)
                {
                    if (duration_ns > 1000000000)
                        return duration_ns / 1e9;
                    else if (duration_ns > 1000000)
                        return duration_ns / 1e6;
                    else if (duration_ns > 1000)
                        return duration_ns / 1e3;
                    else
                        return duration_ns;
                }
        };
    }
}


#endif // DLIB_TImING_Hh_

