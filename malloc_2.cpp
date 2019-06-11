#include <unistd.h>
#include <stdlib.h>
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

size_t _num_free_bytes () {
    return num_free_bytes;
}

size_t _num_allocated_blocks() {
    return num_allocated_blocks;
}

size_t _num_allocated_bytes() {
    return num_allocated_bytes;
}

size_t _num_meta_data_bytes() {
    return num_meta_data_bytes;
}

size_t _size_meta_data() {
    return sizeof(list_node);
}


void* do_alloc (size_t size) {
    if (size == 0 || size >= 100000000) return NULL;
  
   void* ptr=sbrk(0);
   if(ptr==NULL||ptr == (void*)-1) return NULL;
 
   if(brk(ptr+size)== -1) return NULL;
   
   return ptr;
}

void* malloc(size_t size) {
  
 if (size <= 0 || size >= 100000000) return NULL;
    if (!list_head|| _num_free_blocks()==0 ) {
        
        size_t to_alloc = sizeof(list_node);
        list_head = (list_node*)do_alloc(to_alloc);
        if (!list_head) return NULL;
        
        void* ret = do_alloc(size);
        if (!ret){
          list_head=NULL;
          do_alloc(-to_alloc);
          return NULL;
         }
        list_head->is_free = false;
      //  list_head->meta_size = sizeof(list_node);
        list_head->used_size = size;
        list_head->allocated_size = size;
        list_head->next = NULL;
        list_head->prev = NULL;
        list_tail = list_head;
        num_allocated_blocks++;
        num_allocated_bytes+=size;
        num_meta_data_bytes+=sizeof(list_node);

        return ret;
    }
    
    list_node* place_to_alloc = find_next_place (list_head, size);
    if (place_to_alloc) {
        num_free_blocks--;
        num_free_bytes -= size; // Check this case if we dont reuse full block
        place_to_alloc->is_free = false;
        place_to_alloc->used_size = size;
        return place_to_alloc+sizeof(list_node);
    }
    
    size_t to_alloc = sizeof(list_node);
    list_tail->next = (list_node*)do_alloc(to_alloc);
    if (!(list_tail->next )){
      do_alloc(-to_alloc);
     return NULL;
     }
    (list_tail->next)->prev=list_tail;
    list_tail = list_tail->next;
    list_tail->is_free = false;
   // list_head->meta_size = sizeof(list_node);
     void* ret =do_alloc(size);
     if (!ret){
         
          do_alloc(-to_alloc);
          return NULL;
         }
    list_tail->used_size = size;
    list_tail->allocated_size = size;
    list_tail->next = NULL;
    
    
    num_allocated_blocks++;
    num_allocated_bytes+=size;
    num_meta_data_bytes+=sizeof(list_node);
    return ret;
}

void* calloc (size_t num, size_t size) {
    void* block = malloc(size*num);
    if (!block) return NULL;
    std::memset(block, 0, size*num);
    return block;
}


void* realloc (void* oldp, size_t size) {
    if (!oldp || size <=0 || size >= 100000000) return NULL;
    list_node* node =(list_node*) oldp-sizeof(list_node);
    if (node->allocated_size >= size) {
        node->used_size = size;
        return oldp;
    } else {
        void* new_block = malloc(size);
        if (!new_block) return NULL;
        std::memcpy(new_block, oldp , node->used_size);
        if(new_block!= oldp)free(oldp);
        return new_block;
    }
}
void free (void* p) {
    list_node* node=list_head;
    while(node!=NULL && ((void*)(node+1))!=p){
    
    node=node->next;
    }
    if(!node)return;
    
   // list_node* node =(list_node*) (p-sizeof(list_node));
    node->is_free = true;
    node->used_size = 0;
    num_free_blocks ++;
    num_free_bytes += node->allocated_size;
}




//
// Created by omert on 6/11/2019.
//

