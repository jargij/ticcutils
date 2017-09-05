/*
  Copyright (c) 2006 - 2017
  CLST  - Radboud University
  ILK   - Tilburg University

  This file is part of ticcutils

  ticcutils is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  ticcutils is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.

  For questions and suggestions, see:
      https://github.com/LanguageMachines/ticcutils/issues
  or send mail to:
      lamasoftware (at ) science.ru.nl

*/

#include <string>
#include <cstdlib>
#include "config.h"
#include <iostream>
#include <thread>
#include <unistd.h>
#include <stdexcept>

#include "ticcutils/StringOps.h"
#include "ticcutils/LogStream.h"

using namespace std;
using namespace TiCC;

class Sub1 {
public:
  Sub1( LogStream& log ){
    ls = new LogStream( log, "-SUB1", StampMessage );
    *Log(ls) << "created a sub1 " << endl;
  }
  LogStream *ls;
};

void exec1( Sub1& s, int i ){
  *Log(s.ls) << i << endl;
  int sleeps = rand()%(i+1) + 1;
  sleep(sleeps);
  *Log(s.ls) << i << "-" << sleeps << endl;
}

class Sub2 : public Sub1 {
public:
  Sub2( LogStream& l ): Sub1(l){
    ls = new LogStream( l, "-SUB2", StampMessage );
    *Log(ls) << "created a sub2 " << endl;
  }
};

class Sub3 : public Sub1 {
public:
  Sub3( LogStream& l ): Sub1(l){
    ls = new LogStream( l, "-SUB3", StampMessage );
    *Log(ls) << "created a sub3 " << endl;
  }
};

void exec3( Sub3& s, int i ){
  *Log(s.ls) << i << endl;
  int sleeps = rand()%(int(3*(i+1))) + 1;
  sleep(sleeps);
  *Log(s.ls) << i << "---" << sleeps << endl;
}

int main(){
  LogStream the_log( "main-log" );
  Sub1 sub1( the_log );
  Sub2 sub2( the_log );
  Sub3 sub3( the_log );
  for ( int i = 0; i < 10; ++i ){
    *Log(the_log) << "start loop " << i << endl;
    thread t1( exec1, std::ref(sub1), 1 );
    thread t2( exec1, std::ref(sub2), 2 );
    thread t3( exec3, std::ref(sub3), 3 );
    thread t4( exec1, std::ref(sub1), 4 );
    thread t5( exec1, std::ref(sub2), 5 );
    thread t6( exec3, std::ref(sub3), 6 );
    thread t7( exec1, std::ref(sub1), 7 );
    thread t8( exec1, std::ref(sub2), 8 );
    thread t9( exec3, std::ref(sub3), 9 );
    thread t10( exec3, std::ref(sub3), 10 );
    *Log(the_log) << "wait for join " << endl;
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();
    t9.join();
    t10.join();
    *Log(the_log) << "all joined " << endl;
  }
}
