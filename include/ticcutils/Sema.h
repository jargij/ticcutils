#ifndef SEMA_H
#define SEMA_H

#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>

class SemaGroup {
  bool valid;
  int Size;
  int sem_id;
  struct sembuf *P;
  struct sembuf *V;
  bool InitGroup( const std::string&, int );
public:
  SemaGroup( const std::string&, int );
  ~SemaGroup();
  static bool DeleteSemaGroup( int );
  bool Lock( int );
  bool UnLock( int );
  bool Valid() const { return valid; };
  int Id() const { return ( valid ? sem_id : -1 ); };
};

#endif // SEMA_H
