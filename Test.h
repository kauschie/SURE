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
#include <unistd.h> // for usleep
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
        float attack_temp;
        bool output_mode;
        float start_temp; // in deg C

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
        Test(bool opm, int td = 10, string fn = "", float thresh = 80.000, float st_t = 52, float atk_t = 15)
        {
            test_duration = td;
            temp_threshold = thresh;
            output_mode = opm;
            start_temp = st_t;
            attack_temp = atk_t;

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
            system("pidof infinity | xargs kill 2> /dev/null");
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
            vector<time_t> fan_times[2]; // 0 are fan off times, 1 are fan on times
            size_t power_readings = 0;
            


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
            Temp current;
            unsigned int count = 0;   // number of data points



           ///////////////////////////////////////////////////////////////////
           /// check that the program has reached the starting temperature ///
           ///////////////////////////////////////////////////////////////////
            if (fabs(current.cel() - start_temp) > 1)
                cout << "Waiting for CPU to reach the starting temperature...\n" << endl;
            while ( current.cel() - start_temp > 1 )
            {
                usleep(2000000); // sleep for 1 second intervals and then check again until temp reaches start temp
                current.retake_temp();
            }
        
            
             // start inifinite loops in background processes for test
            cout << "Starting Infinit loops on cores 1,2,3" << endl;
            system("taskset -c 1 ./infinity &"); 
            system("taskset -c 2 ./infinity &"); 
            system("taskset -c 3 ./infinity &"); 

           
            if (fabs(start_temp - current.cel()) > 1 )
                cout << "Bringing the CPU up to the starting temperature...\n" << endl;
            while ( start_temp - current.cel() > 1 )
            {
                usleep(500000); // sleep for 0.5 second intervals and then check again until temp reaches start temp
                current.retake_temp();
            }

            cout << "CPU has reached starting temperature...\n";
            cout << "Data collection beginning at " << current.get_tm_str() << endl;

            ///////////////////////
            // output log header // 
            ///////////////////////

            out << endl << endl;
            out << "Output Mode: " << ( (output_mode == true) ? "Verbose\n" : "Quiet\n" ) ;
            out << "Start Time: " << ctime( &(current.get_rawtime()) ) << endl << endl;
            out << "Test Duration: " << test_duration << endl; 
            out << "Start Temp: " << start_temp << endl;
            out << "Threshold Temp: " << temp_threshold << endl; 
            out << "Attack Temp: " <<  attack_temp << endl;
            out << "Under Attack: " << boolalpha << is_attacked << endl << endl;

            // column headings
            out << "Timestamp (HH:MM:SS)  Temperature (C)" << endl;
            // output text to screen if applicable
            // only need the one line becaue it'll have the menu there
            if (output_mode)
                cout << "Start Time: " << ctime(&(current.get_rawtime())) << endl << endl; 


            //////////////////////////
            // Data Collection Loop // 
            /////////////////////////

            while (true)
            {
                // temp reading also stores time the reading was taken directly in data point
                // creation of new temp object collects the data point including time
                data.emplace_back();


                // write to log
                out << "," << data[count].get_tm_str() << "," 
                    << ( data[count].cel() + ( (is_attacked)?attack_temp:0 ) )
                    << " (" << data[count].cel() << ")\n";


                // output data to screen
                if (output_mode)
                {
                    cout << "at " << data[count].get_tm_str() << " the cpu is " 
                        << data[count].cel() + ( (is_attacked)?attack_temp:0 ) << " (" << data[count].cel()
                        << ") degrees celcius\n";
                }

                // Notify operator to take manual reading
                if ( static_cast<int>(difftime( data[count].get_rawtime(), current.get_rawtime())) % 60 == 0 )
                {
                    cout << "<!> <!>    Take Power Reading Now : " << data[count].get_tm_str() 
                        << "      <!> <!>" << endl;
                }

                if ( static_cast<int>(difftime( data[count].get_rawtime(), current.get_rawtime())) % 60 == 58 )
                {
                    cout << ":::       GET READY TO TAKE NEXT READING        :::" << endl;       
                }


                ////////////////////////////
                //      Fan Control      //
                ////////////////////////////

                // check if fan needs to be turned on
                if( ( data[count].cel() + ( (is_attacked)?attack_temp:0 ) ) >= (temp_threshold) )
                {
                    // turn on fan if it's not on and the temp has crossed the threshold
                    if ( !fan_on )
                    {
                        // if fan is being turned on the first time
                        if (fan_times[1].empty())
                        { 
                            digitalWrite(FAN,HIGH); 
                            fan_on = true;
                            threshold.set_time(&(data[count].get_rawtime()));
                            fan_times[1].push_back(data[count].get_rawtime());   // record time that fan turns on 

                            out << ",threshold,reached" << threshold.get_tm_str() << endl;

                            //cout << "Threshold reached -- turning fan on" << endl;
                        }
                        else if (difftime( data[count].get_rawtime(), fan_times[0].back()) >= 10)  
                        {
                            
                            digitalWrite(FAN,HIGH); 
                            fan_on = true;
                            threshold.set_time(&(data[count].get_rawtime()));
                            fan_times[1].push_back(data[count].get_rawtime());   // record time that fan turns on 
                            out << ",threshold,reached" << threshold.get_tm_str() << endl;
                            //cout << "Threshold reached -- turning fan on" << endl;

                        }
                    }

                }

                else        // turn fan off when the temp is below the threshold
                {
                    if (fan_on)
                    {
                        
                       if (difftime( data[count].get_rawtime(), fan_times[1].back()) >= 10)
                        {
                            digitalWrite(FAN,LOW);
                            fan_on = false;
                            out << ",fan_shutoff, reached" << endl;
                            //cout << "Fan_Shutoff reached -- turning fan off" << endl;   
                            fan_times[0].push_back(data[count].get_rawtime());
                        }
                    }

                }


                ///////////////////////////////////////////////////////////////////
                // program ends no matter what if the length of test is exceeded //
                ///////////////////////////////////////////////////////////////////

                if ( difftime(data[count].get_rawtime(), data[0].get_rawtime())  >=  (test_duration * 60 ))
                {
                    // kill fan if still running and mark down the final time 
                    if ( fan_on )
                    {
                        fan_on = false;
                        digitalWrite(FAN,LOW);
                        fan_times[0].push_back(data[count].get_rawtime());
                    }

                    break;


                }

                count++;

            }

            // kill infinite loops when finished
            cout << "Killing Infinit loops on cores 2,3,4" << endl;
            system("pidof infinity | xargs kill 2>/dev/null");

            // run some basic data analysis
            cout << "Calculating Stats...\n\n";

            // collect stats i want
            stats.get_Stats(data, temp_threshold, is_attacked, fan_times[0], fan_times[1], attack_temp); 
            out << endl << endl << stats.to_string() << endl; // write stats to log
            cout << stats.to_string() << endl;      // write stats to screen

            out.close();                        // close the data file
            data.clear();                       // clear the vector for the next phase

            /*
            if (!is_attacked)
            {
                cout << "Phase one finished. Press any key to continue to Phase 2... ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getchar();
            }
            */

            // kill infinite loops on the cores
            if (is_attacked)
            {
                cout << "\n\nEnd of test\n\n";
                cout << "Press any key to continue...\n";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getchar();
            }


        }



};

#endif
