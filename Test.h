#pragma once

#ifndef __TEST_H__
#define __TEST_H__

#include <iostream> // for cout 
#include <fstream> // for ofstream obj
#include <iomanip> // for cout formatting
#include <ctime>    // for time_t, tm, localtime(), time()
#include <sstream> // for ostringstream
#include <vector>   // for vector container
#include <stdio.h>  // for getchar()
#include "Stats.h"  // for doing stats on our data set

using namespace std;

class Test
{
    private:
        /////////////////////////
        ///////// vars  //////////
        /////////////////////////

        // test parameters
        int test_duration;  // in minutes
        float temp_threshold; // in degrees celcius
        string filename;
        bool is_stressed;
        bool output_mode;

        // data containers
        vector<Temp> data; // holds all temperature data points
        Stats stats;        // holds all stats from temp data points

        //////////////////////////
        /////// functions ////////
        // //////////////////////
        // Usage: creates a string of the current date to safe the datafile to by default
        // input: none
        string _get_date()
        {

            time_t rawtime = time(&rawtime); 
            tm * timeinfo = localtime(&rawtime);
            // tm * timeinfo = localtime(time(nullptr)) maybe shorter?

            ostringstream temp;
            temp << 1900 + (timeinfo->tm_year) 
                << setw(2) << setfill('0') << timeinfo->tm_mon 
                << setw(2) << setfill('0') << timeinfo->tm_mday;


            return temp.str();
        }

    public:

        /////////////
        // friends //
        /////////////
        friend class Stats;
        friend class Menu;



        //////////////////
        // constructor //
        /////////////////
        Test(bool opm, int td = 10, string fn = "", float thresh = 80.000)
        {
            test_duration = td;
            temp_threshold = thresh;
            output_mode = opm;

            filename  = (fn != "") ? fn : ( _get_date() + "test.txt");
        }



        /////////////////////////
        ///////   run()   ///////
        /////////////////////////
        // Usage: runs the test under the set conditions
        //
        // Output file format:
        //
        // ~~~~Header~~~~~~
        // ~~~~End Header ~~~
        //
        // ~~~~Column Labels~~~~,   <---- first comma for CSV files 
        // xx:xx:xx,temp,xx:xx:x(x+1),temp,(...)
        //
        //
        // Args: None
        // Returns: None
        void run()
        {
            bool fan_on = false;

            ////////////////////////////////////////
            ///// open log file to write data //////
            ////////////////////////////////////////

            ofstream out;
            out.open(filename, ios::app);

            ////////////////////////////////
            // set initial time variables //
            ////////////////////////////////

            // creates Temp object that has start time and THRESHOLD temp
            // REMEMBER: this is not the starting temp, the starting temp is data[0]

            Temp threshold(temp_threshold); 
            Temp fan_shutoff(50.000f);
            signed int count = 0;   // number of data points


            ///////////////////////
            // output log header // 
            ///////////////////////

            out << "Start Time: " << ctime( &(threshold.get_rawtime()) ) << endl << endl;
            out << "Test Duration: " << test_duration << endl; 
            out << "Threshold Temp: " << temp_threshold << endl; 
            out << "Output Mode: " << ( (output_mode == true) ? "verbose\n" : "quiet\n" ) ;
            out << "Under Stress: " << boolalpha << is_stressed << endl << endl;

            // column headings
            out << "Timestamp (HH:MM:SS)  Temperature (C)" << endl;


            // output text to screen if applicable
            if (output_mode)
                cout << "Start Time: " << ctime( &(threshold.get_rawtime() ) ) << endl << endl; 

            //////////////////////////
            // Data Collection Loop // 
            /////////////////////////

            while (true)
            {
                // temp reading also stores time the reading was taken directly in data point
                // creation of new temp object collects the data point including time
                data.emplace_back();


                // write to log
                out << "," << data[count].get_tm_str() << "," << data[count].cel() << endl;


                // output data to screen
                if (output_mode)
                {
                    cout << "at " << data[count].get_tm_str() << " the cpu is " 
                        << data[count].cel() << " degrees celcius\n";
                }



                /////////////////////////////////////////////////////
                //  Checks if it needs to respond to Stress signal //
                /////////////////////////////////////////////////////

                //
                if (is_stressed)
                {
                    if( data[count] >= threshold )
                    {
                        if ( !fan_on )
                        {
                            // TODO put code here to turn on the pin for the GPIO fan

                            out << ",threshold_reached," << threshold.cel() << endl;
                            if (output_mode)
                                cout << "Threshold reached -- turning fan on" << endl;
                        }

                        if (output_mode)
                            cout << "<!> ";

                    }

                    if ( data[count] <= fan_shutoff )
                    {
                            // TODO code to turn fan off
                            out << ",fan_shutoff reached," << threshold.cel() << endl;
                            if (output_mode)
                                cout << "Fan_Shutoff reached -- turning fan off" << endl;
                    }

                }
                
                if ( difftime(data[count].get_rawtime(), threshold.get_rawtime())  >=  (test_duration * 60 ))
                    break;

//                else // timed test
 //               {
                    // check if done with test (compare start timer with current time)
//                }

                count++;

            }


            stats.get_Stats(data);                  // collect stats i want
            cout << stats.to_string() << endl;      // write stats to screen


            cout << "End of test\n\n";
            cout << "Press any key to continue...\n";
            cin.ignore();
            getchar();

        }



};

#endif
