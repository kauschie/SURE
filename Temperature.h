#pragma once

#ifndef __TEMPERATURE_H__
#define __TEMPERATURE_H__

#include <iostream>
#include <fstream>
#include <time.h>


using namespace std;

class Temp
{

private:

	float c_temp;
	tm * curr_time;	
    time_t rawtime;

	void _get_time()
	{
		//time_t rawtime;
		// struct tm * timeinfo;

		time(&rawtime);
		// timeinfo = localtime(&rawtime);
		curr_time = localtime(&rawtime);
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
//    static num_temps; // used vector size() instead

	Temp()
	{
		c_temp = _get_temperature();
//        c_temp = _get_temperature(1);
		_get_time();
//        num_temps++;
	}

	~Temp()
	{

	}
	
    // getter functions for times
    time_t get_rawtime() { return rawtime; }
	int sec() { return curr_time->tm_sec; }
	int min() { return curr_time->tm_min; }
	int hr() { return curr_time->tm_hour; }
	int yr() { return curr_time->tm_year; }
	int mon() { return curr_time->tm_mon; }
	int day() { return curr_time->tm_mday; }


    // gette functions for temp
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
    bool operator == (const Temp & rhs) { return ( c_temp == rhs.c_temp ); }
    bool operator != (const Temp & rhs) { return ( c_temp != rhs.c_temp ); }
    bool operator >= (const Temp & rhs) { return ( c_temp >= rhs.c_temp ); }
    bool operator <= (const Temp & rhs) { return ( c_temp <= rhs.c_temp ); }
    bool operator > (const Temp & rhs) { return ( c_temp > rhs.c_temp ); }
    bool operator < (const Temp & rhs) { return ( c_temp < rhs.c_temp ); }

};


string Temp::filename = "/sys/class/thermal/thermal_zone0/temp";
//string Temp::filename = "test_temp";


#endif
