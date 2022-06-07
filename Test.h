#pragma once

#ifndef __MENU_H__
#define __MENU_H__

#include <iostream> // for cout 
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
        // vars
        int reading_interval; // in seconds
        int test_duration;  // in minutes
        string filename;
        vector<Temp> data;
        Stats stats;

        // functions
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
        // friends
        friend class Stats;

        // constructor
        Test(int ri = 1, int ti = 1, string fn = "") 
        {
            reading_interval = ri;
            test_duration = ti;

            filename  = (fn != "") ? fn : ( _get_date() + "test.txt");

        }

        // setter functions
        void set_filename() 
        { 
            cout << "Enter the desired filename (alphanumeric only): ";
            cin >> filename;
        }

        void set_read_interval()
        {
            system("clear");
            int time = 1;


            do {

                if (time <= 0 || time > 60)
                    cout << "That time lies outside the range\n\n";

                cout << "Enter the time between temperature readings (0-60sec): ";
                cin >> time;

            } while (time <= 0 || time > 60);

            reading_interval = time; 
        }


        void set_test_length()
        {
            system("clear");
            int time = 1;

            do {

                if (time <= 0 || time > 10)
                    cout << "That time lies outside the range\n\n";

                cout << "Enter the test duration in minutes (1-10minutes): ";
                cin >> time;

            } while (time <= 0 || time > 10);

            test_duration = time;
        }

        // getter functions
        int get_ri() { return reading_interval; }
        int get_ti() { return test_duration; }
        string get_fn() { return filename; }


        void show_menu()
        {
            // system("clear");
            cout << "<!>     CPU Temperature Reader     <!>" << endl;
            cout << setw(25) << right << "Current Reading Interval:" 
                << setw(7) << right << ( get_ri() ) << " seconds\n";

            cout << setw(25) << right << "Current Length of Test:" 
                << setw(7) << right << ( get_ti() ) << " minutes\n";

            cout << setw(25) << right << "Output Filename:";
            cout << " " << left << get_fn() << endl << endl;

            cout << "<!>     ~~~~~    MENU   ~~~~~      <!>" << endl;
            cout << "     1. Change Time Between Temp Readings" << endl;
            cout << "     2. Change Length of Test" << endl;
            cout << "     3. Change Outpit FileName" << endl;
            cout << "     4. Begin Test " << endl;
            cout << "     5. End Program" << endl;

        }	

        void menu_select()
        {
            bool is_bad_input = false;
            char choice;

            do{
                system("clear");
                show_menu();

                if (is_bad_input)
                    cout << "I didn't understand << " << choice << " >>. Please try again.\n";


                cout << "\nEnter your selection: ";
                cin >> choice;

                switch(choice) {

                    case '1':
                        system("clear");
                        set_read_interval();
                        break;

                    case '2':
                        system("clear");
                        set_test_length();
                        break;

                    case '3':
                        system("clear");
                        set_filename();
                        break;

                    case '4':
                        run();
                        break;

                    case '5':
                        break;

                    default:
                        is_bad_input = true;

                }


            } while (choice != '5');

        }

        void write_data(Stats & s)
        {
            
            ofstream out(filename);
            
            out << "Timestamp (HH:MM:SS)" << "\t" << "Temperature (C)\n";
            
            for (auto it = data.begin(); it != data.end(); it++)
            {
                
            }


        }

        void run()
        {
            time_t start_time, last_time;
            unsigned int count = 0;
            bool is_active = true;

            time(&start_time); // set start time
            last_time = start_time; // assign last time as start time 
            tm * curr = localtime(&start_time);
            cout << "Start Time: " << setw(2) << right << setfill('0') << curr->tm_hour << ":" 
                << setw(2) << right << setfill('0') << curr->tm_min << ":" 
                << setw(2) << right << setfill('0') << curr->tm_sec << endl;
            
            //cout << "Last Time: " << last_time << endl;
            while (is_active)
            {
                //cout << "Start Time: " << start_time << endl;
                //cout << "Last Time: " << last_time << endl;
                if (difftime(time(NULL),last_time) >= reading_interval)
                {
                    //data.push_back(); // get temp reading and store
                    // temp reading also stores time the reading was taken directly in data point
                    data.emplace_back();
                    last_time = data[count].get_rawtime(); // set time last was taken   

                    // output data to screen
                    cout << setw(2) << setfill('0') << "at " << data[count].hr() << ":" 
                        << data[count].min() << ":" << data[count].sec() 
                        << " the cpu is " << data[count].cel() << " degrees celcius\n";

                    count++;
                }

                // check if done with test (compare start timer with current time)
                if ( difftime(time(NULL), start_time) >= (test_duration * 60 ) )
                {
                    //       cout << "Entering break statement..." << endl;
                    //      getchar();
                    is_active = false; 
                }

            }

            stats.get_Stats(data);
            cout << stats.to_string() << endl;

            cout << "End of test\n\n";
            cout << "Press any key to continue...\n";
            cin.ignore();
            getchar();

        }



};

#endif
