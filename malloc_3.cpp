#include <unistd.h>
//#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include "list_3.h"

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

size_t _num_allocated_bytes() {//free and used excluding meta data
    return num_allocated_bytes;
}

size_t _num_meta_data_bytes() { //all meta data bytes in heap
    return num_meta_data_bytes;
}

size_t _size_meta_data() {
    return sizeof(list_node);
}

void print_mem(){
    printf("\n ----------------------MEM--------------------------------------\n");
    printf( "-alloc'd blocks  : %d \n",(int)_num_allocated_blocks());
    printf( "-alloc'd bytes   : %d \n",(int)_num_allocated_bytes());
    printf( "-free blocks     : %d \n",(int)_num_free_blocks());
    printf( "-free bytes      : %d \n",(int)_num_free_bytes());
    printf( "-metaData bytes  : %d \n",(int)_num_meta_data_bytes());
    printf( "-metaData size   : %d \n",(int)_size_meta_data());
    printf("\n ----------------------------------------------------------------\n");

}



void* do_alloc (size_t size) {
    if (size == 0 || size >= 100000000) return NULL;

    void* ptr=sbrk(0);
    if(ptr==NULL||ptr == (void*)-1) return NULL;

    if(brk((void*)((long)ptr+(long)size))== -1) {

        return NULL;
    }

    return ptr;
}


void merge_adjacent (list_node* node) {

    if(node->next && node->next->is_free){
        list_node* to_remove = node->next;
        node->next = to_remove->next;
        if (to_remove->next) to_remove->next->prev = node;
        node->allocated_size+=sizeof(list_node) + to_remove->allocated_size;
        num_free_blocks--;
        num_free_bytes+=sizeof(list_node);
        num_allocated_blocks--;
        num_allocated_bytes+=sizeof(list_node);
        num_meta_data_bytes-=sizeof(list_node);
    }
    if(node->prev && node->prev->is_free){
        list_node* to_remove = node;
        if (to_remove->next) to_remove->next->prev = to_remove->prev;
        to_remove->prev->next = to_remove->next;
        node->prev->allocated_size+=sizeof(list_node) + to_remove->allocated_size;
        num_free_blocks--;
        num_free_bytes+=sizeof(list_node);
        num_allocated_blocks--;
        num_allocated_bytes+=sizeof(list_node);
        num_meta_data_bytes-=sizeof(list_node);
    }

}


void* malloc(size_t size) {

    if (size <= 0 || size >= 100000000) return NULL;
    if (size%4) {
        size+=(4-(size%4));
    }
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
     //   print_node(new_node);

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
        else{//there is an already existing place for alloc block.
            if (!(new_node->next )&& new_node->allocated_size<size) {// Wilderness
                do_alloc(size-new_node->allocated_size);
                int extra_size=size-new_node->allocated_size;
                num_allocated_bytes+=(extra_size);
                num_free_blocks--;
                num_free_bytes-=new_node->allocated_size;


                new_node->allocated_size = size;
                new_node->used_size = size;
                new_node->is_free = false;

                return new_node->memory_pointer;
           }
            new_node->is_free=false;
            new_node->used_size=size;
            num_free_blocks--;
            num_free_bytes-=(new_node->allocated_size);


            int remain_size= (int)(new_node->allocated_size) - (int)(size + sizeof(list_node) );
            if ( remain_size >= 128 ){         // reallocation of this block is waistful -> split the block
               void* new_addr=(void*)((long)(new_node->memory_pointer)+(long)(size));
                list_node* split_node=(list_node*)(new_addr);
                init_node(split_node ,true ,remain_size,0,new_node->next,new_node,NULL ) ;
                split_node->memory_pointer=&(split_node->memory_pointer)+1;
                new_node->next=split_node;
                new_node->allocated_size=size;

                num_free_blocks++;
                num_allocated_blocks++;
                num_allocated_bytes-=(int)sizeof(list_node);
                num_meta_data_bytes+=(int)sizeof(list_node);
                num_free_bytes+=(int)(split_node->allocated_size);
            }
            }
            return new_node->memory_pointer;

        }

}


void free (void* p) {
    if(p == NULL  ) return ;

    list_node* node=(list_node*)((long)p-(long)sizeof(list_node));

    if(!node) return;
    if(node->is_free==true)return; //don't free a free block
    node->is_free = true;
    node->used_size = 0;
    num_free_blocks ++;
    num_free_bytes += node->allocated_size;

    merge_adjacent(node);

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
    list_node* node=(list_node*)((long)oldp-(long)sizeof(list_node));
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






//
// Created by omert on 6/11/2019.
//

