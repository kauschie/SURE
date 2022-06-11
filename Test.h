#pragma once

#ifndef __TEST_H__
#define __TEST_H__

#include <iostream> // for cout 
#include <fstream> // for ofstream obj
#include <iomanip> // for cout formatting
#include <ctime>    // for time_t, tm, localtime(), time()
#include <sstream> // for ostringstream
#include <vector>   // for vector container
#include <cstdio>  // for getchar()
#include <wiringPi.h>   // to write to GPIO pins  **NOTE** Must also compile with -lwiringPi eg) ((( g++ -o main main.cpp -lwiringPi  )))
#include "Stats.h"  // for doing stats on our data set

#define FAN 23  // BCM pin number after using linux command gpio readall, locate physical pin and use BCM number

class Menu;

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
        string filenames[2];
        bool is_attacked;
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
        Test(bool opm, int td = 10, string fn = "", float thresh = 70.000)
        {
            test_duration = td;
            temp_threshold = thresh;
            output_mode = opm;

            for (size_t phase = 0; phase < 2; phase++)
            {
                filenames[phase]  = (fn != "") ? fn : ( _get_date() + ((phase == 0)?"_normal":"_attack" ) +  ".txt");
            }
        }

        //////////////////
        //  destructor  //
        /////////////////

        ~Test()
        {
            // kill all infinite loops in case test doesn't finish correctly
                system("pidof infinity | xargs kill");
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
        // Args: send in phase from menu
        //       test will run once in normal mode and then once in attack mode
        // Returns: None
        void run(bool phase)
        {
            bool fan_on = false;
            time_t fan_off_time;


            // changes the phase from no attack -> attack to automate the program
            is_attacked = phase;

            /////////////////////////////////////
            ///////    SETUP WIRINGPI    ////////
            /////////////////////////////////////

            wiringPiSetupGpio();
            pinMode(FAN, OUTPUT);

            ////////////////////////////////////////
            ///// open log file to write data //////
            ////////////////////////////////////////

            ofstream out;
            out.open(filenames[is_attacked], ios::app);

            ////////////////////////////////
            // set initial time variables //
            ////////////////////////////////

            // creates Temp object that has start time and THRESHOLD temp
            // REMEMBER: this is not the starting temp, the starting temp is data[0]

            Temp threshold(temp_threshold); 
            unsigned int count = 0;   // number of data points


            ///////////////////////
            // output log header // 
            ///////////////////////

            out << endl << endl;
            out << "Start Time: " << ctime( &(threshold.get_rawtime()) ) << endl << endl;
            out << "Test Duration: " << test_duration << endl; 
            out << "Threshold Temp: " << temp_threshold << endl; 
            out << "Output Mode: " << ( (output_mode == true) ? "verbose\n" : "quiet\n" ) ;
            out << "Under Attack: " << boolalpha << is_attacked << endl << endl;

            // column headings
            out << "Timestamp (HH:MM:SS)  Temperature (C)" << endl;
            // output text to screen if applicable
            // only need the one line becaue it'll have the menu there
            if (output_mode)
                cout << "Start Time: " << ctime( &(threshold.get_rawtime() ) ) << endl << endl; 




            // start inifinite loops in background processes for test
            cout << "Starting Infinit loops on cores 1,2,3" << endl;
            system("taskset -c 1 ./infinity &"); 
            system("taskset -c 2 ./infinity &"); 
            system("taskset -c 3 ./infinity &"); 


            //////////////////////////
            // Data Collection Loop // 
            /////////////////////////

            while (true)
            {
                // temp reading also stores time the reading was taken directly in data point
                // creation of new temp object collects the data point including time
                data.emplace_back();


                // write to log
                out << "," << data[count].get_tm_str() << "," << ( data[count].cel() + ( (is_attacked)?10:0 ) )
                    << " (" << data[count].cel() << ")\n";


                // output data to screen
                if (output_mode)
                {
                    cout << "at " << data[count].get_tm_str() << " the cpu is " 
                        << data[count].cel() + ( (is_attacked)?10:0 ) << " (" << data[count].cel()
                        << ") degrees celcius\n";
                }


                ////////////////////////////
                //      Fan Control      //
                ////////////////////////////

                // check if fan needs to be turned on
                if( ( data[count].cel() + ( (is_attacked)?10:0 ) ) >= (threshold.cel()) )
                {
                    if (output_mode)
                        cout << "<!> ";

                    // turn on fan if it's not on and the temp has crossed the threshold
                    if ( !fan_on )
                    {
                        if (difftime( data[count].get_rawtime(), fan_off_time) >= 1)  
                        {
                            digitalWrite(FAN,HIGH); 
                            fan_on = true;
                            threshold.set_time(&(data[count].get_rawtime()));

                            out << ",threshold_reached," << threshold.get_tm_str() << endl;

                            // if (output_mode) // TODO - Uncomment after testing
                            cout << "Threshold reached -- turning fan on" << endl;

                        }
                    }

                }
                else        // turn fan off when the temp is below the threshold
                {
                    if (fan_on)
                    {
                        digitalWrite(FAN,LOW);
                        fan_on = false;
                        out << ",fan_shutoff, reached" << endl;
                        cout << "Fan_Shutoff reached -- turning fan off" << endl;   
                        fan_off_time = data[count].get_rawtime();
                    }

                }


                ///////////////////////////////////////////////////////////////////
                // program ends no matter what if the length of test is exceeded //
                ///////////////////////////////////////////////////////////////////

                if ( difftime(data[count].get_rawtime(), data[0].get_rawtime())  >=  (test_duration * 60 ))
                {
                    // wait to exit until fan_timer has been reached
                    if ( fan_on )
                    {
                        fan_on = false;
                        digitalWrite(FAN,LOW);
                    }

                    break;


                }

                count++;

            }


            stats.get_Stats(data, temp_threshold);                  // collect stats i want
            out << endl << endl << stats.to_string() << endl; // write stats to log
            cout << stats.to_string() << endl;      // write stats to screen

            out.close();
            data.clear();                       // clear the vector for the next phase

            if (!is_attacked)
            {
                cout << "Phase one finished. Press any key to continue to Phase 2... ";
                cin.ignore();
                getchar();
            }


            // kill infinite loops on the cores
            if (is_attacked)
            {
                cout << "Killing Infinit loops on cores 2,3,4" << endl;
                system("pidof infinity | xargs kill");
                cout << "\n\nEnd of test\n\n";
                cout << "Press any key to continue...\n";
                cin.ignore();
                getchar();
            }


        }



};

#endif
