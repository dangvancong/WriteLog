#include<iostream>
#include <sys/stat.h>
#include<fstream>
#include<iomanip>
#include<ctime>
#include <limits.h>
#include <unistd.h>
#include "stdio.h"
#include "string.h"
/**********************************************************/
/* Function : Save_Log(void)                              */
/* Decription : write data of arrival time                */
/* Return value: int                                      */
/*      0: Write success                                  */
/*      1: Eror can not get path of the executable file   */
/*      2: Eror can not creat new directory               */
/*      3: Eror can not creat or open csv file            */
/**********************************************************/
int clStaff::Save_Log(void)
{
	tm ltime = *localtime(&this->arrival_time);
	int hh = ltime.tm_hour;
	int mm = ltime.tm_min;
  int dd = ltime.tm_mday;
	int month = ltime.tm_mon + 1;
 
  static int dayCnt;
 
  //  get the path of the executable file
  char result[ PATH_MAX ];
  string path;
  ssize_t linksize = readlink( "/proc/self/exe", result, PATH_MAX );
  if(linksize > 0)
  {
    string fullpath = string( result, linksize);
    string filename(strrchr(&fullpath[0],'/'));
    path = fullpath.substr(0,fullpath.length() - filename.length());
  }
  else
    return 1;
    
  // Check directory exist 
  string dir = path + "/../Log/2020";
  string mon = "";
  if(month < 10)
    mon = "0" + to_string(month);
  else
    mon = to_string(month);
  dir = dir + "2020" + mon;
      
  struct stat info;
  if( stat( &dir[0], &info ) != 0 )
  {
    if(mkdir(&dir[0], 0777) == -1)
      return 2;
  }
      
  // Check csv file exist     
  string csvfile = dir + "/" + "2020" + mon + ".csv";
  ifstream ifile(csvfile);
  if(!ifile)
  {
    ofstream ofile;
    ofile.open(csvfile);
    if(ofile.is_open())
    {
      ofile << "id,hoten" << setw(220) << left << "," << endl;
      ofile.close();
      dayCnt = 0;
    }
    else
      return 3;
  }
  else
  {
    ifile.close();
  }
  
  // write log to csv file
  int count = 0;
  int len,len1;
  string line, str1, str2;
  string breakday;
  static string breaktime;
  fstream file;
  // Write column header of day
  if(id == 1)
  {
    file.open(csvfile);
    if(file.is_open())
    {
      dayCnt ++;
      getline(file , line);
      str1 = strstr(&line[0],",  ");
      len = line.length();
      len1 = str1.length();
      str2 = line.substr(0,len-len1+1);      
      file.seekp(-len-1, ios::cur);
      breakday = "";
      breaktime = "";
      if(dd > dayCnt)
      {
        int day = dd - dayCnt;
        for(int i = 0; i < day; i++)
        {
          breakday = breakday  + (((dayCnt) < 10 )? (" 0" + to_string(dayCnt)) : (" " + to_string(dayCnt))) + "/" + mon + ",";
          breaktime = breaktime + " - ,";
          dayCnt++;
        }
        
      }
      if(dd < 10)
        file << str2 << breakday << " 0" << to_string(dd) << "/" << mon << ",";
      else
        file << str2 << breakday << " " << to_string(dd) << "/" << mon << ",";
      file.close();
    }
    else 
      return 3;
  } 
  
  // Write time to column of day
  file.open(csvfile);
  if(file.is_open())
  {
    while(getline(file , line))
    {   
      count ++;
      if(count == (id +1))
      {
        str1 = strstr(&line[0],",  ");
        len = line.length();
        len1 = str1.length();
        str2 = line.substr(0,len-len1+1);      
        file.seekp(-len-1, ios::cur);
        if(status == 0)
          file << str2 << breaktime << "Off" << "," ;
        else
        {
          string min, hour;
          if(mm < 10)
            min = "0" + to_string(mm);
          else
            min = to_string(mm);
          if(hh < 10)
            hour = "0" + to_string(hh);
          else
            hour = to_string(hh);
          file << str2 << breaktime << " " << hour << ":" << min << "," ;
        }
        break;
      } 
    }
    file.close();
  }
  else
    return 3;
  
  // Write time of first day to column of day
  if(count < (id +1))
  {
    file.open(csvfile, ios::app);
    if(file.is_open())
    {
      string comma = "";
      for(int i = 1; i < dayCnt; i++)
        comma = comma + " - ,"; 
      if(status == 0)
        file << id << "," << name << ","<< comma  << " Off" << setw(220) << left << "," << endl;
      else
      {
        string min, hour;
        if(mm < 10)
          min = "0" + to_string(mm);
        else
          min = to_string(mm);
        if(hh < 10)
          hour = "0" + to_string(hh);
        else
          hour = to_string(hh); 
        file << id << "," << name << "," << comma  << " " << hour << ":" << min << setw(220) << left << "," << endl;
        file.close();
      }
    }
    else 
      return 3;   
  }
  
  return 0;
}