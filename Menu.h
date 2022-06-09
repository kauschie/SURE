#pragma once

#ifndef __MENU_H__
#define __MENU_H__

#include <iostream>
#include <iomanip>
#include "Test.h"

using namespace std;

class Test;

class Menu
{
    private:
        Test *test;

    public:

        Menu(Test * t)
        {
            test = t;
        }

        void set_test_length()
        {
            do {

                if (test->test_duration <= 0 || test->test_duration > 30)
                    cout << "That time lies outside the range\n\n";

                cout << "Enter the test duration in minutes (1-30 minutes): ";
                cin >> test->test_duration;

            } while (test->test_duration <= 0 || test->test_duration > 30);
        }

        void set_stress()
        {
            char c = '0';
            cout << "When the \"Stress\" variable is enabled, the computer will turn on\n"
                << "The GPIO connected fan to attempt to cool the system.\n"
                << "Turn stress on (y/n)? ";
            while ( (c != 'y' ) && (c != 'Y') && (c != 'n') && (c != 'N') )
            {
                cin >> c; 
            }

            test->is_stressed = ( (c == 'y' || c == 'Y') ? true : false );
        }

        void set_filename() 
        { 
            cout << "Enter the desired filename (alphanumeric only): ";
            cin >> test->filename;
        }

        void set_temp_threshold()
        {
            cout << "When the \"Stress\" variable is enabled, the computer will turn on\n"
                << "The GPIO connected fan to attempt to cool the system.\n"
                << "Enter the threshold temperature (in degrees celcius): ";
            cin >> test->temp_threshold;
            
            while ( (test->temp_threshold < 25) && (test->temp_threshold > 90) )
            {
                cout << "Enter a number between 25 and 90 (degC): " << endl;
                cin >> test->temp_threshold;
            }
        }

        void show_menu()
        {
            // system("clear");
            cout << "<!>     CPU Temperature Reader     <!>" << endl;
            cout << setw(25) << right << "Test Duration:" 
                << setw(7) << right << ( test->test_duration ) << " minutes\n";

            cout << setw(25) << right << "Threshold Temperature: "
                << setw(7) << right << ( test->temp_threshold ) << " (degC)\n";

            cout << setw(25) << right << "Stress Mode: "
                << setw(7) << right << ( test->is_stressed ? "On" : "Off" ) << endl; 

            cout << setw(25) << right << "Output Filename:";
            cout << " " << left <<test->filename << endl << endl;

            

            cout << "<!>     ~~~~~    MENU   ~~~~~      <!>" << endl;
            cout << "     1. Change Length of Test" << endl;
            cout << "     2. Set stress on/off" << endl;
            cout << "     3. Set threshold temp" << endl;
            cout << "     4. Change Output FileName" << endl;
            cout << "     5. Begin Test " << endl;
            cout << "     6. End Program" << endl;

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
                        set_test_length();
                        break;

                    case '2':
                        system("clear");
                        set_stress();
                        break;

                    case '3':
                        system("clear");
                        set_temp_threshold();
                        break;

                    case '4':
                        system("clear");
                        set_filename();
                        break;

                    case '5':
                       test->run();
                        break;

                    case '6':
                        break;

                    default:
                        is_bad_input = true;

                }


            } while (choice != '6');

        }

};

#endif
