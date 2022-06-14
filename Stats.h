#pragma once

#ifndef __STATS_H__
#define __STATS_H__

#include "Test.h"
#include <vector>
#include <algorithm>
#include <cassert>
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
        float avg;
        float delta_max;
        float delta_min;
        float start_temp;
        double thresh_time;
        float total_fan_time;

    public:
        Stats()
        {
            min = NOT_DEFINED;
            max = NOT_DEFINED;
            avg = NOT_DEFINED;
            delta_max = NOT_DEFINED;
            delta_min = NOT_DEFINED;
            start_temp = NOT_DEFINED;            
            thresh_time = NOT_DEFINED;
            total_fan_time = NOT_DEFINED;
       }

        void get_Stats(vector<Temp> & data, float & temp_threshold, bool & attack, 
                vector<time_t> & off_times, vector<time_t> & on_times, float attack_temp)
        {
            // starting temperature
            start_temp = data[0].cel();

            // min of all data
            min = (*(min_element(data.begin(),data.end()))).cel();

            // max of all data
            max = (*(max_element(data.begin(),data.end()))).cel();

            // avg
            avg = ar_average(data);

            // delta from max 
            delta_max = max - avg;

            // delta from min
            delta_min = avg - min;

            // total fan activation time
            total_fan_time = get_total_fan_time(off_times, on_times);
            
            // time to reach threshold temperature
            thresh_time = get_threshold_time(data, temp_threshold, attack, attack_temp);

        }

        double get_total_fan_time(vector<time_t> & _off_times, vector<time_t> & _on_times) const
        {
            double sum = 0;
            auto on_it = _on_times.begin();
            auto off_it = _off_times.begin();

            // fan was turned on at least once
            if (!_on_times.empty())
            {
                for (; on_it != _on_times.end(); on_it++, off_it++)
                {
                    sum += ( (*off_it) - (*on_it) );
                }
                
            }

            return sum;

        }



        double get_threshold_time(vector<Temp> & data, float & temp_threshold, bool & attk, float atk_t)
        {
            auto it = data.begin(); 

            while (it != data.end())
            {
                if ( (*it).cel() >= ( temp_threshold - (attk?atk_t:0) ) )
                    return ( difftime( (*it).get_rawtime(), data[0].get_rawtime() ) );
                it++;
            }


            return -1;

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

        // returns all vars back as a string
        string to_string()
        {
            ostringstream temp;

            temp << setw(6) << setfill('0');
            temp << "oOo--->  Stats  <---oOo" << endl;
            temp << "Starting temperature: " << start_temp << endl;
            temp << "Time(sec) before threshold was reached: ";

            if (thresh_time == -1)
                temp << "Never reached temp" << endl;
            else
                temp << thresh_time << endl;

            temp << "Total time(sec) the fan was on: " << total_fan_time << endl;
            temp << "Min from total data set: " << min << endl;
            temp << "Max from total data set: " << max << endl;
            temp << "Avg from total data set: " << avg << endl;
            temp << "Delta from Max to Avg: " << delta_max << endl;
            temp << "Delta from Avg to Min: " << delta_min << endl;

            return temp.str();

        }

};

#endif
