//#include "mkfs1.h"
//#include "fsutil.c"
#include "layer2.h"
//#include "fs_func.c"

int main(){
    int size = 1024*100;
    FILE * fp = fopen("test.txt", "w+");
    fseek(fp, size, SEEK_SET);
    fputc('\n', fp);
    fseek(fp, 0L, SEEK_END);
    printf("created file of size %ld\n", ftell(fp));
    fclose(fp);
    mkfs("test.txt");

    fp = fopen("test.txt", "rw+");
    /*long num;
    int i;
    for(i = 1; i < 50; i++){
        if(i%11==0) free_block(fp, i-3);
        num = get_free_block_num(fp);
        printf("FREE BLOCK:%ld\n",num);
    }*/

    struct inode node;

    /*for(i = 1; i < 50; i++){
        if(i%6==0) free_inode(fp, i-4);
        num = get_free_inode(fp, &node);
        if(num) printf("\tGOT FREE INODE NUM:%ld\n", num);
        else printf("\tNO INODE GIVEN\n.");
    }*/
    mode_t mode;
    struct fuse_file_info;
    char text[] = "Hello world";
printf("11th:%c...\n", text[11]);
    fs_create_dir("/", mode, fp); // Move creation of root dir to mkfs
    fs_create("/tt.txt", mode, fp );
    fs_write("/tt.txt", 0, text, 11, fp);
    char read_buff[15];
    fs_read("/tt.txt", read_buff, 11, 0, fp);
    printf("FILE CONTENTS:%s\n\n\n", read_buff);

    char text1[] = "I am back!";
    fs_create("/ss.txt", mode, fp );
    fs_write("/ss.txt", 0, text1, 11, fp);
    char read_buff1[15];
    fs_read("/ss.txt", read_buff1, 11, 0, fp);
    printf("FILE CONTENTS:%s\n\n\n", read_buff1);

    strcpy(read_buff, "Garbage!!");
    fs_read("/tt.txt", read_buff, 11, 0, fp);
    printf("FILE CONTENTS:%s\n\n\n", read_buff);

    fclose(fp);

    return 0;
}
