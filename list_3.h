//
// Created by omert on 6/13/2019.
//

#ifndef OSHW4_LIST_3_H
#define OSHW4_LIST_3_H

struct list_node {
    bool is_free;
    // size_t meta_size;
    size_t allocated_size;
    size_t used_size;
    list_node* next;
    list_node* prev;
    void* memory_pointer;
};

void init_node(list_node* node ,bool is_free,size_t allocated_size,size_t used_size,list_node* next,list_node* prev,void* memory_pointer ){
    if(node==NULL) return;

    node->is_free=is_free;
    node->allocated_size=allocated_size;
    node->used_size=used_size;
    node->next=next;
    node->prev=prev;
    node->memory_pointer=memory_pointer;


}


//returns the meta-data node that indicates a block that can contain the
//requested allocation size
//if returned null : need to alloc at the end of the list
list_node* find_next_place (list_node* head, size_t to_alloc) {

    // printf("\n looking for node with place of  %d or more \n",(int)to_alloc);
    if(head == NULL ) return NULL;
    list_node *curr=head;
    while(curr!= NULL){
        // printf("current node is free?  %d and it has a place of %d \n",(int)(curr->is_free),(int )curr->allocated_size );
        if(curr->is_free && (curr->allocated_size >= to_alloc || !curr->next)){
            return curr;
        }
        curr=curr->next;
    }
    return NULL;
}


void print_node(list_node* node){

    if(node==NULL)return;
    printf(".......................................................\n.........................node addr : %p   \n node->is_free= %d  .......... addr : %p   \n node->allocated size= %d .... addr : %p   \n node->used size= %d  ........ addr : %p   \n node->memory pointer= %p .... addr : %p   \n ---total node size : %d \n .........................................................\n " , node  ,(int)(node->is_free),&(node->is_free),  (int)(node->allocated_size),&(node->allocated_size), (int)(node->used_size),&(node->used_size),   node->memory_pointer,&(node->memory_pointer),  (int)sizeof(list_node) );

}

void print_list (list_node* head) {
    list_node* curr = head;
    while (curr) {
        print_node(curr);
        curr=curr->next;
    }
}
#endif //OSHW4_LIST_3_H
