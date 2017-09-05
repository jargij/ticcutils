#include <cstring>
#include <iostream>
#include <string>
#include <cerrno>
#include <stdexcept>
// #if defined( __linux__ )
// #  if !defined __USE_XOPEN
// #    define __USE_XOPEN
// #  endif
// #endif
#include "ticcutils/Sema.h"

using std::cerr;
using std::endl;
using std::string;
using std::runtime_error;

//#define SEM_DEBUG

SemaGroup::SemaGroup( const string& key_str, int size ): sem_id(-1){
  valid = InitGroup( key_str, size );
  if ( !valid ){
    cerr << "Sema problems" << endl;
  }
#ifdef SEM_DEBUG
  else {
    cerr << "Sema created with id " << sem_id << endl;
  }
#endif
}

typedef union semun {
  int val;
  struct semid_ds *buf;
  unsigned short *array;
} semun;

SemaGroup::~SemaGroup(){
  if ( P ) delete [] P;
  if ( V ) delete [] V;
  // Don't delete the semaphore group, it is a survivor
}


bool SemaGroup::Lock( int num ){
  if ( !valid || num < 0 || num > Size ){
    cerr << "Lock problem: " << num << " - " << Size << " - " << valid << endl;
    return false;
  }
  else if ( semop( sem_id, &P[num], 1 ) < 0 ){
    throw runtime_error( string("Lock failed (") + strerror( errno ) + ")" );
    return false;
  }
  return true;
}

bool SemaGroup::UnLock( int num ){
  if ( !valid || num < 0 || num > Size ) {
    cerr << "UnLock problem: " << num << " - " << Size << " - " << valid << endl;
    return false;
  }
  else if ( semop( sem_id, &V[num], 1 ) < 0 ){
    throw runtime_error( string("UnLock failed (") + strerror( errno ) + ")" );
    return false;
  }
  return true;
}

bool SemaGroup::InitGroup( const string &key_name, int size ){
  if ( sem_id >= 0 ){
    cerr << "CreateSemGroup called twice!, Ignored" << endl;
    return true;
  }
  key_t key = ftok( key_name.c_str(), 'a' );
  if ( key < 0 ){
    cerr << "Problem getting key for:" << key_name
	 << " (" << strerror( errno ) << ")" << endl;
    return false;
  }
  else {
    if ( key < 0 ){
      cerr << "Invalid key for SemaphoreGroup" << endl;
    }
    else if ( size < 1 || size > 256 ){
      cerr << "Invalid size for SemaphoreGroup" << endl;
    }
    else {
      sem_id = semget( key, size, 0644 | IPC_CREAT );
      if ( sem_id < 0 ){
	cerr << "Problem getting semid (" << strerror( errno ) << ")" << endl;
	cerr << "key was: " << key << "Size was" << size << endl;
      }
#ifdef SEM_DEBUG
      else {
	cerr << "Created sem_id " << sem_id << " for key " << key << endl;
      }
#endif
      Size = size;
    }
    if ( sem_id < 0 ){
      return false;
    }
    else {
      semun semarg;
      P = new struct sembuf[size];
      V = new struct sembuf[size];
      semarg.val = 1;
      for( int i=0; i < size; ++i ){
	semctl( sem_id, i, SETVAL, semarg);
	P[i].sem_num =  i;
	P[i].sem_op  = -1;
	P[i].sem_flg = SEM_UNDO;
	V[i].sem_num =  i;
	V[i].sem_op  = 1;
	V[i].sem_flg = SEM_UNDO;
      }
      return true;
    }
  }
}

bool SemaGroup::DeleteSemaGroup( int id ){
  semun semarg;
  if ( id < 0 ){
    cerr << "Invalid key for SemaphoreGroup" << endl;
  }
  else if ( semctl( id, 0, IPC_RMID, semarg) >= 0 ){
#ifdef SEM_DEBUG
    cerr << "deleted Semaphore Group id = " << id << endl;
#endif
    return true;
  }
  else
    cerr << "Problem deleting Semaphore group ("
	 << strerror( errno ) << ")" << endl;
  return false;
}
