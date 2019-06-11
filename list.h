//
// Created by omert on 6/11/2019.
//

struct list_node {
    bool is_free;
   // size_t meta_size;
    size_t allocated_size;
    size_t used_size;
    list_node* next;
    list_node* prev;
   // void* memory_pointer;
};


//returns the meta-data node that indicates a block that can contain the
//requested allocation size
//if returned null : need to alloc at the end of the list
list_node* find_next_place (list_node* head, size_t to_alloc) {
    if(head == NULL ) return NULL;
     list_node *curr=head;
    while(curr->next!= NULL){
        if(curr->is_free && curr-> allocated_size >= to_alloc){
            return curr;
        }
        curr=curr->next;
    }
    return NULL;
}

#ifndef OSHW4_LIST_H
#define OSHW4_LIST_H

#endif //OSHW4_LIST_H
