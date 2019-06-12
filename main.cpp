//
// Created by Alon Lib on 06/06/2019.
//

#include <cstdio>
#include <assert.h>
#include "malloc_2.cpp"

int main() {
    printf("Hi there!\n");
    //printf("%l",(long)sizeof(allocated_meta_data));
  /*
    assert(_num_free_blocks() == 0);
    assert(_num_free_bytes() == 0);
    assert(_num_allocated_blocks() == 0);
    assert(_num_allocated_bytes() == 0);
    assert(_num_meta_data_bytes() == 0);
 */
 
 /*
    int* arr=(int*)malloc(10*sizeof(int));
    int* arr2=(int*)malloc(10*sizeof(int));
    
    printf("arr addr : %p \n arr2 addr : %p \n ",arr,arr2);
    
    if(arr==(void*)-1 || arr==NULL|| arr2==NULL)return 1;
    
    for (int i=0;i<10;i++){
    arr[i]=i;
    arr2[i]=20+i;
    }
    for (int i=0;i<10;i++){
     printf("%d \n",arr[i] );
     
    }
    for (int i=0;i<10;i++){
     printf("%d \n",arr2[i] );
     
    }
       free(arr);
       free(arr2);
    printf("freed: %d\n",_num_free_blocks());   
    
    
*/
  int* i =(int*)malloc(sizeof(int));
  int* j =(int*)malloc(sizeof(int));
    printf(" i addr : %p \n j addr : %p \n",i,j);
    printf("num allocated blocks : %d \n",(int)_num_allocated_blocks());
    
    *i=5;
    *j=6;
    
     printf("i = %d  \n j = %d \n",*i,*j);
     printf( "alloc'd blocks : %d\n",_num_allocated_blocks());
      
     free(i);
     free(j);
    printf("freed: %d\n",_num_free_blocks());   
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int* arr1=(int*)malloc(10*sizeof(int));
    int* arr2=(int*)malloc(10*sizeof(int));
    
    printf("arr1 addr : %p \n arr2 addr : %p \n ",arr1,arr2);
    
    if(!arr1 || !arr2 )return 1;
    printf( "alloc'd blocks : %d\n",_num_allocated_blocks());
    
    for (int i=0;i<10;i++){
    arr1[i]=i;
    arr2[i]=20+i;
    }
    for (int i=0;i<10;i++){
     printf("%d      %d  \n",arr1[i],arr2[i] );
     
    }
  
       free(arr1);
       free(arr2);
    printf("freed: %d\n",_num_free_blocks());   
    //////////////////////////////////////////////////////try to reuse blocks//////////////////////////////////////////////////////////////////////
    
    int* arr3=(int*)calloc(5,sizeof(int));
    int* arr4=(int*)calloc(7,sizeof(int));
    
    
    printf("arr3 addr : %p \n arr3 addr : %p \n ",arr3,arr4);
    
    if(!arr3 || !arr4 )return 1;
    printf( "alloc'd blocks : %d\n",_num_allocated_blocks());
    
    for (int i=0;i<5;i++){   /* arr3[i]=30+i;*/    printf(" arr3[] : %d  \n",arr3[i]);  }
    for (int i=0;i<7;i++){ /*   arr4[i]=40+i;  */  printf(" arr4[] : %d  \n",arr4[i]);  }
    
  
  arr3=(int*)realloc(arr3,8*sizeof(int));
  
   for (int i=0;i<8;i++){   /* arr3[i]=30+i;*/    printf(" arr3[] : %d  \n",arr3[i]);  }
   arr3=(int*)realloc(arr3,11*sizeof(int));
   for (int i=0;i<11;i++){   /* arr3[i]=30+i;*/    printf(" arr3[] : %d  \n",arr3[i]);  }
   
       free(arr3);
       free(arr4);
    printf("freed: %d\n",_num_free_blocks());   
    
    
    
    
    
    
    
    
     
    /// run the tests each one separately because list uses global vars
    /// that means the test needs to run 4 times with different _TEST_NUMBER

  //  printf("Success for test: %d.\n", _TEST_NUMBER);
    return 0;
}