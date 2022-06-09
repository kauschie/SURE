#pragma once

#ifndef __TEMPERATURE_H__
#define __TEMPERATURE_H__

#include <iostream>
#include <fstream> // to read files
#include <ctime> // for time library
#include <cmath> // for fabs

using namespace std;

class Temp
{

private:

	float c_temp;
	tm * curr_time;	
    time_t rawtime;
    char tm_str[11]; 

	void _get_time()
	{
		//time_t rawtime;
		// struct tm * timeinfo;

		time(&rawtime);
		// timeinfo = localtime(&rawtime);
		curr_time = localtime(&rawtime);
        strftime(tm_str, 21, "%T", curr_time); // get time as string in time_buff
	}
	
//    int _get_temperature(int x) { return 69.69; }   // stub for testing when not on linux

	float _get_temperature()
	{
		static ifstream fin;
		float temp;

		fin.open(filename);
		if (fin.fail())
		{
			cout << endl << "********************" << endl;
			cout <<         "Could not open file"  << endl;
			cout <<         "Quitting now..."      << endl;
			cout << endl << "********************" << endl;

			return 1;
		}
		
		fin >> temp;
		fin.close();

		return temp*.001;
	}


public:

    static string filename; 

    // default constructor
    // reads temp from locatin specified in (( filename ))
	Temp()
	{

		c_temp = _get_temperature();
		_get_time();
	}

    
    // parameterized constructor
    // Usage:
    //      used to create a Temperature data point from the test's threshold temperature 
    //      which also holds the start time of the test
    //      this is then used as a comparison to see if temps have reached the threshold
    //      and it will be used anywhere that the start_time of the test run is needed
    //  Args: used when a float is passed in when the object is created
    Temp(float t)
    {
       c_temp = t;
       _get_time();

    }

	~Temp()
	{
	}

    // mutator to change when threshold was reached
    void set_time(time_t * raw)
    {
        rawtime = *raw;
        curr_time = localtime(&rawtime);
        strftime(tm_str, 21, "%T", curr_time); // get time as string in time_buff
    }
	
    // getter functions for times
    time_t & get_rawtime() { return rawtime; } // return a reference so that it can be used as lvalue
    string get_tm_str() { return tm_str; } // outputs "HH:MM:SS"
    
    // getter functions for temp
	float cel()
	{
		//c_temp = _get_temperature();
//		c_temp = _get_temperature(1); // stub function
		return c_temp;
	}

	float fah()
	{
		return ( ( cel() * 1.8 ) + 32);
	}

    // overloaded operators
    bool operator == (const Temp & rhs) { return ( fabs( c_temp - rhs.c_temp ) <= 0.001 );  }
    bool operator != (const Temp & rhs) { return ( fabs( c_temp - rhs.c_temp ) > 0.001 );  }
    bool operator >= (const Temp & rhs) { return ( c_temp - rhs.c_temp ) >= 0.001 ; }
    bool operator <= (const Temp & rhs) { return ( c_temp - rhs.c_temp ) <= 0.001 ; }
    bool operator > (const Temp & rhs) { return ( c_temp - rhs.c_temp ) > 0.001 ; }
    bool operator < (const Temp & rhs) { return ( c_temp - rhs.c_temp ) < 0.001 ; }

};

string Temp::filename = "/sys/class/thermal/thermal_zone0/temp";
//string Temp::filename = "test_temp";

#endif
