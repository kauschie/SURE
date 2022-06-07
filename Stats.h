#pragma once

#ifndef __STATS_H__
#define __STATS_H__

#include "Test.h"
#include <vector>
#include <algorithm>
#include <assert.h>
#include <sstream>
#include <iomanip>

#define NOT_DEFINED -1

using namespace std;

class Test;

class Stats
{

    private:
        float min;
        float max;
        float max_10;
        float min_10;
        float avg;
        float avg_10;
        float delta_max;
        float delta_min;
        float delta_max_10;
        float delta_min_10;

    public:
        Stats()
        {
            min = NOT_DEFINED;
            max = NOT_DEFINED;
            max_10 = NOT_DEFINED;
            min_10 = NOT_DEFINED;
            avg = NOT_DEFINED;
            avg_10 = NOT_DEFINED;
            delta_max = NOT_DEFINED;
            delta_min = NOT_DEFINED;
            delta_max_10 = NOT_DEFINED;
            delta_min_10 = NOT_DEFINED;
        }

        void get_Stats(vector<Temp> & data)
        {

            // min of all data
            min = (*(min_element(data.begin(),data.end()))).cel();

            // max of all data
            max = (*(max_element(data.begin(),data.end()))).cel();

            // max of last 10
            max_10 = max_last_10(data);

            // min of last 10
            min_10 = min_last_10(data);

            // avg
            avg = ar_average(data);

            // avg of last ten
            avg_10 = avg_last_10(data);

            // delta from max 
            delta_max = max - avg;

            // delta from min
            delta_min = avg - min;

            // delta from max of last ten
            delta_max_10 = max_10 - avg_10;

            // delta from min of last ten
            delta_min_10 = avg_10 - min_10;
        }


        // gets arithmetic average of data set
        float ar_average(vector<Temp> & data)
        {
            float sum = 0;

            for (vector<Temp>::iterator it = data.begin(); it != data.end(); it++)
            {
                sum += (*it).cel();
            }

            return ( sum / data.size());

        }

        // returns avg of last 10 or all elements if total data points < 10
        float avg_last_10(vector<Temp> & data)
//        float avg_last_10()
        {
            float sum = 0;
            size_t denominator = ( (data.size() < 10) ? data.size() : 10 );
            assert(denominator > 0);

            for (auto it = data.rbegin(); it != (data.rbegin() + denominator); it++)
            {
                sum += (*it).cel();
            }

            return ( sum / denominator );
        }

        // returns max of last 10 or all elements if total data points < 10
        float max_last_10(vector<Temp> & data)
//        float max_last_10()
        {
            size_t range = ( (data.size() < 10) ? data.size() : 10 );

            return (*(max_element(data.rbegin(), (data.rbegin() + range) ))).cel() ;
        }

        // returns min of last 10 or min of all elements if total data points < 10
        float min_last_10(vector<Temp> & data)
//        float min_last_10()
        {
            size_t range = ( (data.size() < 10) ? data.size() : 10 );

            return (*(min_element(data.rbegin(), (data.rbegin() + range) ) )).cel() ;
        }

        // returns all vars back as a string
        string to_string()
        {
            ostringstream temp;

            temp << setw(6) << setfill('0');
            temp << "oOo--->  Stats  <---oOo" << endl;
            temp << "Min from total data set: " << min << endl;
            temp << "Max from total data set: " << max << endl;
            temp << "Max from last 10 data points: " << max_10 << endl;
            temp << "Min from last 10 data points: " << min_10 << endl;
            temp << "Avg from total data set: " << avg << endl;
            temp << "Avg from last 10 data points: " << avg_10 << endl;
            temp << "Delta from Max to Avg: " << delta_max << endl;
            temp << "Delta from Avg to Min: " << delta_min << endl;
            temp << "Delta from Max_10 to Avg_10: " << delta_max_10 << endl;
            temp << "Delta from Avg_10 to Min_10: " << delta_min_10 << endl;

            return temp.str();

        }
        
        float delta_last_10(vector<Temp> & data)
//        float delta_last_10()
        {
            float avg = avg_last_10(data);
            float d_max = max_last_10(data) - avg;
            float d_min = avg - min_last_10(data);
            return (d_max > d_min) ? d_max : d_min;
        }
};

#endif
