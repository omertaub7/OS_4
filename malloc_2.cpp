#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include "list.h"

using namespace std;

list_node* list_head = NULL;
list_node* list_tail = NULL;
int num_free_blocks = 0;
int num_free_bytes = 0;
int num_allocated_blocks = 0;
int num_allocated_bytes = 0;
int num_meta_data_bytes = 0;

size_t _num_free_blocks() {
    return num_free_blocks;
}

size_t _num_free_bytes () { //number of free bytes in the system excluding meta-data
    return num_free_bytes;
}

size_t _num_allocated_blocks() { //all alocated blocks (free or not)
    return num_allocated_blocks;
}

size_t _num_allocated_bytes() {//free and used excluding meta da
    return num_allocated_bytes;
}

size_t _num_meta_data_bytes() { //all meta data bytes in heap
    return num_meta_data_bytes;
}

size_t _size_meta_data() {
    return sizeof(list_node);
}


void print_node(list_node* node){

if(node==NULL)return;
printf(".......................................................\n.........................node addr : %p   \n node->is_free= %d  .......... addr : %p   \n node->allocated size= %d .... addr : %p   \n node->used size= %d  ........ addr : %p   \n node->memory pointer= %p .... addr : %p   \n ---total node size : %d \n .........................................................\n " , node  ,(int)(node->is_free),&(node->is_free),  (int)(node->allocated_size),&(node->allocated_size), (int)(node->used_size),&(node->used_size),   node->memory_pointer,&(node->memory_pointer),  (int)sizeof(list_node) );

}



void* do_alloc (size_t size) {
    if (size == 0 || size >= 100000000) return NULL;
  
   void* ptr=sbrk(0);
   if(ptr==NULL||ptr == (void*)-1) return NULL;
 
   if(brk(ptr+size)== -1) {
   
   return NULL;
   }
   
   return ptr;
}

void* malloc(size_t size) {
  
   if (size <= 0 || size >= 100000000) return NULL;
   list_node* new_node;
   if(!list_head){
     new_node=(list_node*)do_alloc(sizeof(list_node));
     init_node(new_node,false,size,size,NULL,NULL,NULL);
     new_node->memory_pointer=do_alloc(size);
     if(!(new_node->memory_pointer)){  //if alloc falils 
       do_alloc(-sizeof(list_node));   // free last meta alloc
       return NULL;
      }
      list_head=new_node;
      list_tail=list_head;
      
      num_allocated_blocks++;
      num_allocated_bytes+=(int)size;
      num_meta_data_bytes+=sizeof(list_node);
      
      return new_node->memory_pointer;
      
   }
   else{
     new_node=find_next_place(list_head,size);
     if (!new_node){      //there is no already existing place for alloc. must allocate a new block
         new_node=(list_node*)do_alloc(sizeof(list_node));
         init_node(new_node,false,size,size,NULL,list_tail,NULL);
         new_node->memory_pointer=do_alloc(size);
         if(!(new_node->memory_pointer)){    //if alloc falils 
             do_alloc(-sizeof(list_node));   // free last meta alloc
             return NULL;
           }
           list_tail->next=new_node;
           list_tail=new_node;
            num_allocated_blocks++;
            num_allocated_bytes+=(int)size;
            num_meta_data_bytes+=sizeof(list_node);
      
          return new_node->memory_pointer;
           
     
     }
     else{         //there is an already existing place for alloc block.
         new_node->is_free=false;
         new_node->used_size=size;
         num_free_blocks--;
         num_free_bytes-=(new_node->allocated_size);
         return new_node->memory_pointer;
            
     }
         // should not get here:
       
         return NULL;
   
      }
         
}

void* calloc (size_t num, size_t size) {
    void* block = malloc(size*num);
    if (!block) return NULL;
    std::memset(block, 0, size*num);
    return block;
}


void* realloc (void* oldp, size_t size) {
    if ( size <=0 || size >= 100000000) return NULL;
    if(oldp == NULL){
      return malloc(size);
    }
    list_node* node=(list_node*)(oldp-sizeof(list_node));
  //  printf("\n \n  ------------------starting to realloc mem from %p \n", oldp);
  //  print_node(node);
    if (node->allocated_size >= size) {
   //   printf("current size fits\n");
        node->used_size = size;
        return oldp;
    } else {
  //  printf("current size didn't fit\n");
        void* new_block = malloc(size);
        if (!new_block) return NULL;
        std::memcpy(new_block, oldp , node->used_size);
        if(new_block!= oldp)free(oldp);
        return new_block;
    }
}







void free (void* p) {
if(p == NULL || p<=(void*)0 ) return ;
   /*
    list_node* node=list_head;
    while(node!=NULL && ((void*)(node+1))!=p){
    
      node=node->next;
    
    }
    */
 //   printf("addr to free: %p\n",p);
    
    list_node* node=(list_node*)(p-sizeof(list_node));
    
        
    if(!node){
  //  printf("what?! didnt find block to free?!?!?\n");
    return;
    }
    
   // list_node* node =(list_node*) (p-sizeof(list_node));
    node->is_free = true;
    node->used_size = 0;
    num_free_blocks ++;
    num_free_bytes += node->allocated_size;
  //  print_node(node);
}




//
// Created by omert on 6/11/2019.
//

