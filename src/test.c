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

    fp = fopen("test.txt", "r");
    long num = get_free_block_num(fp);
    int i;
    for(i = 0; i < 50; i++){
        printf("FREE BLOCK:%ld\n",num);
        num = get_free_block_num(fp);
    }

    return 0;
}
