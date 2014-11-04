#include "mkfs1.c"
#include "fsutil.c"

int main(){
    int size = 1024*10;
    FILE * fp = fopen("test.txt", "w+");
    fseek(fp, size, SEEK_SET);
    fputc('\n', fp);
    fseek(fp, 0L, SEEK_END);
    printf("created file of size %ld\n", ftell(fp));
    fclose(fp);
    mkfs("test.txt");

    fp = fopen("test.txt", "rw+");
    long num;
    int i;
    for(i = 1; i < 50; i++){
        if(i%11==0) free_block(fp, i-3);
        num = get_free_block_num(fp);
        printf("FREE BLOCK:%ld\n",num);
    }

    struct inode node;

    for(i = 1; i < 50; i++){
        if(i%6==0) free_inode(fp, i-4);
        num = get_free_inode(fp, &node);
        if(num) printf("\tGOT FREE INODE NUM:%ld\n", num);
        else printf("\tNO INODE GIVEN\n.");
    }

    fclose(fp);

    return 0;
}
