#include "mkfs1.c"

int main(){
    int size = 1024*10;
    FILE * fp = fopen("test.txt", "w+");
    fseek(fp, size, SEEK_SET);
    fputc('\n', fp);
    fseek(fp, 0L, SEEK_END);
    printf("created file of size %ld\n", ftell(fp));
    fclose(fp);
    mkfs("test.txt");
    return 0;
}
