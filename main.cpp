//
// Created by Alon Lib on 06/06/2019.
//

#include <cstdio>
#include <assert.h>
#include "malloc_2.cpp"

int main() {
    printf("Hi there!\n");
    //printf("%l",(long)sizeof(allocated_meta_data));

    assert(_num_free_blocks() == 0);
    assert(_num_free_bytes() == 0);
    assert(_num_allocated_blocks() == 0);
    assert(_num_allocated_bytes() == 0);
    assert(_num_meta_data_bytes() == 0);
  
    int* arr=(int*)malloc(10*sizeof(int));
    printf("arr addr : %p \n",arr);
    if(arr==(void*)-1 || arr==NULL)return 1;
    for (int i=0;i<10;i++){
    arr[i]=i;
    }
    for (int i=0;i<10;i++){
     printf("%d \n",arr[i] );
    }
 /*
  int* i =(int*)malloc(sizeof(int));
    printf("i addr : %p \n",i);
     printf("num allocated blocks : %d \n",(int)_num_free_blocks());
    *i=5;
     printf("i = %d \n",*i);
    */  
    /// run the tests each one separately because list uses global vars
    /// that means the test needs to run 4 times with different _TEST_NUMBER

  //  printf("Success for test: %d.\n", _TEST_NUMBER);
    return 0;
}