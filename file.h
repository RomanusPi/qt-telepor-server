#ifndef FILE_H
#define FILE_H
#include <iostream>
#include <fstream>
#include <string>
#include "data.h"


using namespace std;

class File{

public:
void fileTermOpen(void) //open the file
 {
   FileReadDes.open(InputFile, ios::in | ios::out);
   if (FileReadDes.fail()){
      cout << "error open the file : " << InputFile << endl<<"error with terms  file: "<<flush; //clean the buffer
   }
   cout << "open base test file: " << InputFile << endl;

 }

void file_close( fstream &file_des )
 {
        cout << endl << "close  file:" << endl;
        file_des.close();
 }


void write_to_file(QMap < QString,rdata> &ReservedMap)
 {

    QString strDate;
    string stdstrDate;
    QDate qdate;
    QMapIterator <QString,rdata> Iter(ReservedMap);
    while (Iter.hasNext())
    {
      Iter.next();
      strDate = Iter.key();stdstrDate = strDate.toStdString();
      if (stdstrDate != "")
        FileReadDes << stdstrDate << " " << ReservedMap[strDate].isReserved <<'\n';
    }
 }

void read_from_file(QMap <QString,rdata> &ReservedMap)
 {
    QString strDate = "";
    string stdstrDate = "", boolvalue = "0";
    while (!FileReadDes.eof())//work until descryptor riches  the end of file//
    {
        FileReadDes >> stdstrDate; FileReadDes >> boolvalue;
        strDate = QString::fromStdString(stdstrDate);
        if (boolvalue == "1") ReservedMap[strDate].isReserved = true;
        else ReservedMap[strDate].isReserved = false;
    }

 }

public:
  fstream FileReadDes;//descryptor to file
  string InputFile = "terminy.txt";//name of the file to read write

};



#endif // FILE_H
