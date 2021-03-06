#pragma once

#ifndef __MENU_H__
#define __MENU_H__

#include <iostream>
#include <iomanip>
#include <cstdio>
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

        ~Menu()
        {
            system("pidof infinity | xargs kill 2>/dev/null");
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

        void set_filename() 
        { 
            string fn;
            cout << "Enter the desired filename (alphanumeric only): ";
            cin >> fn; 

            for (size_t phase = 0; phase < 2; phase++)
            {
                test->filenames[phase]  = ( fn +  ((phase == 0)?"_normal":"_attack" ) +  ".txt");
            }
        }


        void set_temp_threshold()
        {
            cout << "When the \"attack\" variable is enabled, the computer will add "
                << test->attack_temp << "\n(attack_emp) degrees Celsius to the running temperature. \n"
                << "The GPIO connected fan will activate when it reaches the threshold temperature\n"
                << "to attempt to cool the system.\n"
                << "Enter the threshold temperature (in degrees celcius): ";
            cin >> test->temp_threshold;

            while ( (test->temp_threshold < 25) && (test->temp_threshold > 90) )
            {
                cout << "Enter a number between 25 and 90 (degC): " << endl;
                cin >> test->temp_threshold;
            }
        }

        void set_start_temp()
        {
            cout << "The program will wait to start until the CPU "
                << "reaches this temperature: ";
            cin >> test->start_temp;

            while ( (test->start_temp < 25) && (test->start_temp > 90) )
            {
                cout << "Enter a number between 25 and 90 (degC): " << endl;
                cin >> test->start_temp;
            }
        }

        void set_attack_temp()
        {
            cout << "The program will add this amount to the\n"
                << "CPU temperature when it is under attack.\n";
            cout << "Enter an amount between 0 and 20: ";
            cin >> test->attack_temp;

            while ( (test->attack_temp < 0) && (test->attack_temp > 20) )
            {
                cout << "Enter a number between 0 and 20 (degC): " << endl;
                cin >> test->attack_temp;
            }
        }

        void show_menu(bool select_part = true)
        {
            cout << "<!>     CPU Temperature Reader     <!>" << endl;
            cout << setw(25) << right << "Test Duration:" 
                << setw(7) << right << ( test->test_duration ) << " minutes\n";

            cout << setw(25) << right << "Start Temperature:" 
                << setw(7) << right << ( test->start_temp ) << " (degC)\n";


            cout << setw(25) << right << "Threshold Temperature:"
                << setw(7) << right << ( test->temp_threshold ) << " (degC)\n";

            cout << setw(25) << right << "Attack Temp:"
                << setw(7) << right << ( test->attack_temp ) << " (degC)\n";

            cout << setw(25) << right << "Attack Mode:"
                << setw(7) << right << ( test->is_attacked ? "On" : "Off" ) << endl; 



            cout << setw(26) << right << "Output Filename (Normal):";
            cout << " " << left << test->filenames[0] << endl;
            cout << setw(26) << right << "Output Filename (Attack):";
            cout << " " << left << test->filenames[1] << endl << endl;



            if (select_part)
            {
                cout << "<!>     ~~~~~    MENU   ~~~~~      <!>" << endl;
                cout << "     1. Change Length of Test" << endl;
                cout << "     2. Set the start temp" << endl;
                cout << "     3. Set threshold temp" << endl;
                cout << "     4. Set attack temp" << endl;
                cout << "     5. Change Output FileName" << endl;
                cout << "     6. Begin Test " << endl;
                cout << "     7. End Program" << endl;
            }

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
                        set_start_temp();
                        break;

                    case '3':
                        system("clear");
                        set_temp_threshold();
                        break;

                    case '4':
                        system("clear");
                        set_attack_temp();
                        break;

                    case '5':
                        system("clear");
                        set_filename();
                        break;

                    case '6':
                        test->run(0);   // normal 
                        test->run(1);   // attack
                        break;

                    case '7':
                        break;

                    default:
                        is_bad_input = true;

                }


            } while (choice != '7' && choice !='6');

        }

};

#endif
