#include "Temperature.h"
#include "Test.h"
#include "Menu.h"
#include <iostream>
#include <stdio.h>
#include <cstring>

using namespace std;

// flags     purpose
// -v       verbose  causes the test output to be sent to the screen
// if no parameter is passed, or if false is passed, 
int main(int argc, char* argv[])
{
    Test * test;
    /*
    cout << "argc = " << argc << endl;
    for (int i = 0; i < argc; i++)
    {
        cout << "argv = " << argv[i] << endl;
    }
    */

    if ( (argc > 2) || 
            ( (argc == 2) && (strcmp(argv[1],"-v" )) ) )
    {
        cout << "Program could not be started.. Correct usage is: \n";
        cout << "./main       -- or  --        ./main -v\n\n";
        cout << "Relaunch program correctly. Quitting now...\n";
        return 1;
    }

    if (argc == 2)
    {
        if (strcmp(argv[1],"-v") == 0)
        {
            cout << "Launching in (( verbose )) mode\n";
            cin.ignore();
            test = new Test(true);

        }
        else
        {
            cout << "Bad argument: " << argv[1] << endl;
            cout << "Quitting now..." << endl;}
    }
    else
    {
        cout << "Launching in (( quiet )) mode\n";
        cin.ignore();
        test = new Test(false);
    }


    Menu menu(test);

    menu.menu_select();

    cout << endl;

	
	delete test;

	return 0;
}

