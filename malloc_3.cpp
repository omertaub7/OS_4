
#include <unistd.h>
#include <cstring>


using namespace std;

/** ........................................list struct .................................................**/
struct list_node {
    bool is_free;
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

    if(head == NULL ) return NULL;
    list_node *curr=head;
    while(curr!= NULL){
        if(curr->is_free && (curr->allocated_size >= to_alloc || !curr->next)){
            return curr;
        }
        curr=curr->next;
    }
    return NULL;
}

/** .....................................................................................................**/


/**..............................................Globals...................................................**/
list_node* list_head = NULL;
list_node* list_tail = NULL;
int num_free_blocks = 0;
int num_free_bytes = 0;
int num_allocated_blocks = 0;
int num_allocated_bytes = 0;
int num_meta_data_bytes = 0;

/** ............................................................................................... **/

/** ......................................... functions ........................................... **/
void free(void*);
void* malloc(size_t);
void* realloc(void*, size_t);
void* calloc (size_t , size_t );

list_node* split_block(list_node* , size_t );
void merge_adjacent (list_node* ) ;


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



/**-----------------------------------------------Helpers-------------------------------------------------**/

/**
 * do actual allocation of space with brk()
 * @param size to alloc
 * @return new block addr.
 */
void* do_alloc (size_t size) {
    if (size == 0 || size >= 100000000) return NULL;

    void* ptr=sbrk(0);
    if(ptr==NULL||ptr == (void*)-1) return NULL;

    if(brk((void*)((long)ptr+(long)size))== -1) {

        return NULL;
    }

    return ptr;
}


/**
 * check if a splitting of a block is necessary  and split accordingly
 * @param node- header of block to split
 * @param size - requested size to allocated on node
 * @return the address of the new node that is created on splitting. NULL if no split occurred
 */
list_node* split_block(list_node* node, size_t size){
    if(node==NULL) return NULL;
    int remain_size= (int)(node->allocated_size) - (int)(size + sizeof(list_node) );
    if ( remain_size >= 128 ) {         // reallocation of this block is waistful -> split the block
        void *new_addr = (void *) ((long) (node->memory_pointer) + (long) (size));
        list_node *split_node = (list_node *) (new_addr);
        init_node(split_node, false, remain_size, 0, node->next, node, NULL);
        split_node->memory_pointer = &(split_node->memory_pointer) + 1;
        node->next = split_node;
        free(split_node->memory_pointer);

        node->allocated_size = size;

        num_allocated_blocks++;
        num_allocated_bytes -= (int) sizeof(list_node);
        num_meta_data_bytes += (int) sizeof(list_node);
     //   num_free_bytes += (int) (split_node->allocated_size);
        return  split_node;

    }
    return NULL;
}

/**
 * check if two / three free adjacent blocks should be merged into one and do it
 * @param node
 */
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
/**------------------------------------------------------------------------------------------------------------**/

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



            split_block(new_node,size);
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
        split_block(node,size);
        return oldp;
    } else {// not enough space
        size_t next_free = (node->next&&node->next->is_free)?(node->next->allocated_size + sizeof(list_node)):0;
        size_t prev_free = (node->prev&&node->prev->is_free)?(node->prev->allocated_size + sizeof(list_node)):0;
        if(next_free && next_free+node->allocated_size >= size){ //current+next is enough
            if (node->next->next) node->next->next->prev = node;
            num_allocated_bytes+=sizeof(list_node);
            num_allocated_blocks--;
            num_free_blocks--;
            num_free_bytes-=node->next->allocated_size;
            num_meta_data_bytes-=sizeof(list_node);
            node->next = node->next->next;
            node->allocated_size +=next_free;
            node->used_size=size;


            split_block(node,size);

            return node->memory_pointer;
        }
        if(prev_free && prev_free+node->allocated_size >= size){ //current+prev is enough
            node->prev->next = node->next;
            if (node->next) node->next->prev = node->prev;
            num_free_bytes-=node->prev->allocated_size;
            node->prev->allocated_size+=sizeof(list_node)+node->allocated_size;
            node->prev->used_size=size;
            node->prev->is_free=false;
            std::memcpy(node->prev->memory_pointer, node->memory_pointer , node->used_size);
            num_allocated_bytes+=sizeof(list_node);
            num_allocated_blocks--;
            num_free_blocks--;

            num_meta_data_bytes-=sizeof(list_node);

            split_block(node->prev,size);

            return node->prev->memory_pointer;
        }
        if(prev_free && next_free && prev_free+next_free+node->allocated_size >= size){ // current+prev+next is enough
            list_node* merged_node = node->prev;

            num_free_bytes-=node->prev->allocated_size;

            num_free_bytes-=node->next->allocated_size;

            merged_node->next = node->next->next;
            if (node->next->next) node->next->next->prev = merged_node;
            merged_node->allocated_size+=(sizeof(list_node)+node->allocated_size+next_free);
            merged_node->used_size=size;
            merged_node->is_free=false;
            std::memcpy(merged_node->memory_pointer, node->memory_pointer , node->used_size);


            num_allocated_bytes+=(2*sizeof(list_node));
            num_allocated_blocks-=2;
            num_free_blocks-=2;

            num_meta_data_bytes-=2*sizeof(list_node);


            split_block(node->prev,size);
            return merged_node->memory_pointer;
        }
        int size_to_copy = node->used_size;
        free(oldp);
        void* new_block = malloc(size);
        if (!new_block) return NULL;
        std::memcpy(new_block, oldp , size_to_copy);
        return new_block;
    }
}


