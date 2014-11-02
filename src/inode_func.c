#include <stdio.h>
#include <fuse.h>

#define USER_R_SET (1<<6)
#define USER_W_SET (1<<7)
#define USER_X_SET (1<<8)

int main(){
    int x=0;
    printf("\n%d",x);
    x |= USER_R_SET;

    printf("\n%d",x);
}

struct inode* inode_get(char * path){
    
}

void inode_put(){

}

