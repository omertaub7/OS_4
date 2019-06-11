#include <unistd.h>
#include <stdlib.h>
//#include <stdio.h>

void* malloc (size_t size) {
    if (size == 0 || size >= 100000000) return NULL;
  
   void* ptr=sbrk(0);
   if(ptr==NULL||ptr == (void*)-1) return NULL;
 
   if(brk(ptr+size)== -1) return NULL;
   
   return ptr;
}

//
// Created by omert on 6/10/2019.
//

