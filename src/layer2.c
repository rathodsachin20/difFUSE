//Implements system calls for the file system

#include "layer2.h"

/* Gives block number stored at 'index' in block list at indirect_block_no
*  Allocates new block is 'allocate' is true and block did not exist.
*/
block_num get_block_in_list(block_num indirect_block_no, int index, bool allocate){
        printf("--get_blokc_inList--%d--",index);
    struct block_list indirect_bl;
    read_block_list(&indirect_bl, indirect_block_no);
    block_num block_no = indirect_bl.list[index];
    printf("blockn0-%ld-",block_no);
    if(block_no == 0 && allocate){
        block_no = allocate_block();
        if(!block_no) return 0;
        indirect_bl.list[index] = block_no;
        write_block_list(&indirect_bl, indirect_block_no);
    }
    return block_no;
}

/* Returns a block number on disk for the given offset in a file, by travesing
*  direct and indirect blocks. 
*  If no block exists at given offset and if allocate is true, it will allocate
*  a new block. Otherwise, it returns 0.
*/
block_num get_file_block_num(int block_index, block_num inode_num, bool allocate){
    //return 6;
    struct inode node;
    read_inode(inode_num, &node);
    //block_num block_index = offset / BLOCK_SIZE;
    block_num block_no = 0;
    int n = BLOCK_SIZE / sizeof(block_num); // Number of entries per blocks
    if(block_index < INODE_NUM_DIRECT_BLOCKS){
        printf("Direct block.\n");
        block_no = node.direct_blocks[block_index];
        if(block_no == 0){
            if( allocate ){
                block_no = allocate_block();
                if(block_no == 0){
                    // No free blocks available
                    return 0;
                }
                node.direct_blocks[block_index] = block_no;
                write_inode(inode_num, &node);
            }
            else { // No new allocation asked
               printf("Block not allocated or asked to. in direct get_file_block_num");
               return 0;
            }
        }
    }
    else if(block_index < (INODE_NUM_DIRECT_BLOCKS + n)){ // Single indirect
        printf("Single Indirect block.\n");
        int index = block_index -  INODE_NUM_DIRECT_BLOCKS;
        block_num indirect_block_no = node.single_indirect_block;
        if(indirect_block_no != 0){
            block_no = get_block_in_list(indirect_block_no, index, allocate);
        }
        else { // indirect_block_no == 0
            if(allocate) {
                indirect_block_no = allocate_block_list();
                if(!indirect_block_no) return 0;
                block_no = get_block_in_list(indirect_block_no, index, allocate);

                node.single_indirect_block = indirect_block_no;
                write_inode(inode_num, &node);

            }
            else{
                return 0;
            }
        }
    }
    else if(block_index < (INODE_NUM_DIRECT_BLOCKS + n + n*n)){ // Double Indirect :O
        printf("Double Indirect block.\n");
        int dd_index = block_index - INODE_NUM_DIRECT_BLOCKS - n;
        int d_index = dd_index / n;
        int s_index = dd_index % n;
        struct block_list d_indirect_bl;
        block_num d_indirect_block_no = node.double_indirect_block;
        if(!d_indirect_block_no){ // No Double indirect block
            if(allocate){
                d_indirect_block_no = allocate_block_list();
                if(!d_indirect_block_no) return 0;
                node.double_indirect_block = d_indirect_block_no;
                write_inode(inode_num, &node);
            }
            else{
                return 0;
            }
        }

        read_block(&d_indirect_bl, d_indirect_block_no, 0, sizeof(struct block_list));
        block_num s_indirect_block_no = d_indirect_bl.list[d_index];
        if(!s_indirect_block_no){ // No Single indirect block
            if(allocate){
                s_indirect_block_no = allocate_block_list();
                if(!s_indirect_block_no) return 0;
                d_indirect_bl.list[d_index] = s_indirect_block_no;
                write_block_list(&d_indirect_bl, d_indirect_block_no);
            }
            else{
                return 0;
            }
        }
        block_no = get_block_in_list(s_indirect_block_no, s_index, allocate);

    }
    else if(block_index < (INODE_NUM_DIRECT_BLOCKS + n + n*n+ n*n*n)){ // Triple Indirect :O :O
        printf("Triple Indirect block.\n");
        int tt_index = block_index - INODE_NUM_DIRECT_BLOCKS - n - n*n;
        int t_index = tt_index / n*n;
        int d_index = (tt_index % (n*n)) / n;
        int s_index = (tt_index % (n*n)) % n;
        struct block_list t_indirect_bl;
        struct block_list d_indirect_bl;
        block_num t_indirect_block_no = node.triple_indirect_block;
        if(!t_indirect_block_no){ // No Double indirect block
            if(allocate){
                t_indirect_block_no = allocate_block_list();
                if(!t_indirect_block_no) return 0;
                node.triple_indirect_block = t_indirect_block_no;
                write_inode(inode_num, &node);
            }
            else{
                return 0;
            }
        }
        read_block(&t_indirect_bl, t_indirect_block_no, 0, sizeof(struct block_list));
        block_num d_indirect_block_no = t_indirect_bl.list[t_index];
        if(!d_indirect_block_no){ // No Double indirect block
            if(allocate){
                d_indirect_block_no = allocate_block_list();
                if(!d_indirect_block_no) return 0;
                t_indirect_bl.list[t_index] = d_indirect_block_no;
                write_block_list(&t_indirect_bl, t_indirect_block_no);
            }
            else{
                return 0;
            }
        }
        read_block(&d_indirect_bl, d_indirect_block_no, 0, sizeof(struct block_list));
        block_num s_indirect_block_no = d_indirect_bl.list[d_index];
        if(!s_indirect_block_no){ // No Single indirect block
            if(allocate){
                s_indirect_block_no = allocate_block_list();
                if(!s_indirect_block_no) return 0;
                d_indirect_bl.list[d_index] = s_indirect_block_no;
                write_block_list(&d_indirect_bl, d_indirect_block_no);
            }
            else{
                return 0;
            }
        }
        block_no = get_block_in_list(s_indirect_block_no, s_index, allocate);

    }
    else{
        printf("File Max Size Limit Reached!!!");
        block_no = 0;
    }
    printf("get_file_block_no returning:%ld\n", block_no);
    return block_no;
}


//int add_inode_entry(const char* filepath, block_num file_inum){
void add_inode_entry(const char* filepath, block_num file_inum){
    printf("\tin add_inode_entry\n");
    int len = strlen(filepath);
    char* fname = (char*) malloc(sizeof(char) * (len+1));
    strcpy(fname, filepath);
    char* pch = strrchr(filepath, '/');
    strncpy(fname, &filepath[pch - filepath + 1], len);

    struct directory dir;
    struct inode parent_node;

    block_num parent_inum = get_parent_inode_num(filepath);
    read_inode(parent_inum, &parent_node);
    //TODO: iterate to get last unfilled block and add entry there
    //block_num block_no = parent_node.direct_blocks[0];
    int lastindex = parent_node.last_filled_block_index;
    block_num block_no = get_file_block_num(lastindex, parent_inum, true);
    //block_num block_no = 1 + NUM_INODE_BLOCKS;
    if(block_no == 0){
        return;
        //block_no = allocate_block();
        //parent_node.direct_blocks[0] = block_no;
        //write_inode(parent_inum, &parent_node);
    }
    
    
    read_block(&dir, block_no, 0, sizeof(struct directory));
    
    int i;
    bool empty_slot = false;
    for(i=0; i<BLOCK_SIZE/NAMEI_ENTRY_SIZE; i++){
        if(dir.inode_num[i]==0){
            dir.inode_num[i] = file_inum;
            //dir.name[i][MAX_FILE_NAME_LEN-1] = '\0';
            strcpy(dir.name[i], &fname[0]);
            empty_slot = true;
            break;
        }
        continue;
    }
    if(!empty_slot){ // New block needed
        block_no = get_file_block_num(lastindex+1, parent_inum, true);
        initialize_dir_block(block_no);
        parent_node.last_filled_block_index = lastindex + 1;;
        write_inode(parent_inum, &parent_node);
        read_block(&dir, block_no, 0, sizeof(struct directory));
        dir.inode_num[0] = file_inum;
        strcpy(dir.name[0], &fname[0]);

    }
    write_block(&dir, block_no, 0, sizeof(struct directory));
    //struct directory dir2;
    //read_block(&dir2, block_no, 0, sizeof(struct directory));
    //printf("DIR DISK CONTENTS:%ld, %s\n", dir2.inode_num[0], dir2.name[0]);
    printf("INODE ENTRY ADDED TO PARENT.\n");
    //return 0;
}


block_num get_parent_inode_num(const char* filepath){
    //copy file path
    int len = strlen(filepath);
    char *fpath = (char *) malloc(sizeof(char)*(len+1));
    strcpy(fpath,filepath);
    if(fpath[len-1]=='/' && len>1){
        fpath[len-1] = '\0';
    }
    //get dir path
    char* pathlen = strrchr(fpath,'/');
    fpath[pathlen - fpath] = '\0';
    strncpy(fpath, filepath, pathlen - fpath);
    const char* dirpath = fpath;
    //return dir inode number
    return fs_namei(dirpath);
}


block_num fs_namei(const char* filep){
    char* fname;
    struct inode working_inode;
    char* filepath;
    int len = strlen(filep);
    block_num inode_no = 1;
    
    filepath = (char*) malloc(sizeof(char) * (len+1));
    strcpy(filepath, filep);
    fname = strtok(filepath,"/");
    
    if(strncmp(&filepath[0],"/",1)==0){
        printf("in fs_namei: path starts with root.%s\n",filepath);
        while(fname != NULL){
            printf("in directory: %s\n", fname);            
            read_inode(inode_no, &working_inode);
            struct directory dr;
            //loop through the direct blocks to find name
            int n = 0;
            int found=0;
            while(n <= working_inode.last_filled_block_index && !found){
                //block_num block_no = working_inode.direct_blocks[n];
                block_num block_no = get_file_block_num(n, inode_no, false);
                if(block_no == 0){
                    continue;
                    //printf("NO SUCH FILE!\n");
                    //return 0;
                }
                read_block(&dr, block_no, 0, sizeof(struct directory));
                int i;
                for(i=0; i<BLOCK_SIZE/NAMEI_ENTRY_SIZE; i++){
                    printf("\n----------dir file:%s.", dr.name[i]);
                    if( strcmp(fname,dr.name[i])==0 ){
                        inode_no = dr.inode_num[i];
                        found=1;
                        printf("PATH FOUND!!!\n");
                        break;
                    }
                }
                n++;
                if(!found){
                    printf("%s:PATH NOT FOUND!!!\n", filepath);
                    return 0;
                }
            }
            fname = strtok(NULL,"/");
        }
    }
    //printf("In fs_namei: %s: path not found\n", filepath);
    return inode_no;
}

int fs_create(const char *filepath, mode_t mode){
    if(filepath == NULL)
        return -1; 

    int len = strlen(filepath);
    
    char* fname = (char*) malloc(sizeof(char) * (len+1));
    strcpy(fname, filepath);
    char* pch = strrchr(filepath, '/');
    strncpy(fname, &filepath[pch - filepath + 1], len);


    struct inode node = {};
    block_num inode_num = allocate_inode(&node);
    printf("in fs_create: got inode no. %ld", inode_num);
    read_inode(inode_num, &node);
    //node.owner_id = 0 ;
    //node.group_id = 0;
    //node.type = 1;
    if(mode) node.mode = mode|S_IFREG;
    else{
        node.mode = 0 | S_IFREG;
        node.mode = node.mode | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
    }
    node.last_filled_block_index = -1;
    //node.file_modified = time(0);
    //node.direct_blocks[0] = 0; //TODO: this should not be needed
    //node.direct_blocks[0] = allocate_block();
    write_inode(inode_num, &node);
    
    // Make entry in parent dir
    add_inode_entry(filepath, inode_num);
    printf("File Created.\n");
    printf("fname is %s.\n", fname);
    return 0;
}

int fs_create_dir(const char *filepath, mode_t mode){
    if(filepath == NULL)
        return -1; //have to check how to return error number

    struct inode node = {};
    block_num inode_num = allocate_inode(&node);
    printf("in fs_create_dir: got inode no. %ld\n", inode_num);

    add_inode_entry(filepath, inode_num);
    block_num block_no = get_file_block_num(0, inode_num, true);

    // Add entries for . and ..
    initialize_dir_block(block_no);
    struct directory dir;
    read_block(&dir, block_no, 0, sizeof(struct directory));
    dir.inode_num[0] = inode_num;
    strcpy(dir.name[0], ".");
    dir.inode_num[1] = get_parent_inode_num(filepath);
    strcpy(dir.name[1], "..");
    write_block(&dir, block_no, 0, sizeof(struct directory));

    read_inode(inode_num, &node);
    node.last_filled_block_index = 0;
    node.owner_id = 121 ;
    node.group_id = 1;
    //node.type = 2;
    if(mode) node.mode = mode|S_IFDIR;
    else{
        node.mode = 0 | S_IFDIR;
        node.mode = node.mode | S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
    }
    //node.file_modified = time(0);
    write_inode(inode_num, &node);
    return 0;
}

int fs_open(const char* filepath){
    //check for errors
    //if file does not exist then create a new file
    //use namei to get inode
    //get the data block using inode
    //write back the data block and inode when done
    block_num inode_num = fs_namei(filepath);
    if(inode_num == 0){
        printf("file doesnt exist, creating...");
        fs_create(filepath, 0);
    }
    else{
        printf("do nothing");
    }
    
    return 0;
}

int fs_read(const char *filepath, char *buf, size_t count, off_t offset){
    /*check if user has permission to read
     if(inode->file_size < offset)
     return error;
     */
    block_num inode_num = 0;
    block_num block_no = 0;
    int read_count = 0;
    int read_offset = 0;
    char read_buff[BLOCK_SIZE];

    inode_num = fs_namei(filepath);
    if(inode_num == 0){
        printf("%s:file does not exist.\n", filepath);
        return -ENOENT;
    }
    struct inode node;
    read_inode(inode_num, &node);
    int lastindex = node.last_filled_block_index;

    int index = offset/BLOCK_SIZE; // Assuming linearly filled file
    /*int index = 0; 
    while(index < lastindex){
        block_no = get_file_block_num(index, inode_num, false);
        if(!block_no) continue;
        read_offset += BLOCK_SIZE;
    }
    read_offset = 0; */

    block_no = get_file_block_num(index, inode_num, false);
    offset = offset % BLOCK_SIZE; // offset within first block
    read_count = read_block(read_buff, block_no, offset, count);
    memcpy(buf, read_buff, read_count);
    read_offset += read_count;
    index++;

    while(index <= lastindex){ // File larger than one block
        block_no = get_file_block_num(index, inode_num, false);
        if(!block_no) continue;
        read_count = read_block(read_buff, block_no, 0, BLOCK_SIZE);
        memcpy(&buf[read_offset], read_buff, read_count);
        read_offset += read_count;
        if(read_count < BLOCK_SIZE) break;
        index++;
    }
    return read_offset;
}

int fs_write(const char* filepath, long offset, const char* buffer, size_t size){
    if(size <= 0) return -1;
    block_num inode_num = fs_namei(filepath);
    //mode_t mode = NULL; // TODO: Change this to defaults
    
    // Write expects file to be already created
    printf("File name:%s inode:%ld\n", filepath, inode_num);
    if(inode_num == 0){
        printf("%s : No such file exists!\n", filepath);
        return -ENOENT;
    }

    struct inode node;
    read_inode(inode_num, &node);
    if(node.file_size > 0){
        printf("File Already exists with non-zero size!!\n");
        return -1;
    }

    int num_blocks = ((size-1) / BLOCK_SIZE) + 1;
    block_num block_no = 0;

    int i;
    //long remaining = size;
    long buff_offset = 0;
    for( i=0; i<num_blocks-1; i++){
        block_no = get_file_block_num(i, inode_num, true);
        write_block(&buffer[buff_offset], block_no, 0, BLOCK_SIZE);
        buff_offset += BLOCK_SIZE;
    }
    size_t remaining = size - buff_offset;
    block_no = get_file_block_num(num_blocks-1, inode_num, true);
    size_t last_size = write_block(&buffer[buff_offset], block_no, 0, remaining);

    read_inode(inode_num, &node); // Always read just before modifying!!
    node.last_filled_block_index = num_blocks -1;
    node.file_size = size;
    
    write_inode(inode_num, &node);
    printf("file write complete.\n");
    if(last_size==remaining){
        return size;
    }
    return size;
}

int fs_getattr(const char* filepath, struct stat* stbuf){
    int res = 0;

    memset(stbuf, 0, sizeof(struct stat));
    if (strcmp(filepath, "/") == 0) { // Path is a directory TODO:currently only root. do for any dir
        printf("getattr path dir:%s\n", filepath);
        block_num inode_no = fs_namei(filepath);
        if(inode_no == 0){
            res = -ENOENT;
            return res;
        }
        struct inode node;
        read_inode(inode_no, &node);
        //stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_mode = node.mode;
        stbuf->st_nlink = 2;
        stbuf->st_size = node.file_size;
    } else if (filepath[0] == '/') { //Path is a file that starts with root
        printf("getattr path file:%s\n", filepath);
        block_num inode_no = fs_namei(filepath);
        if(inode_no == 0){
            res = -ENOENT;
            return res;
        }
        struct inode node;
        read_inode(inode_no, &node);
        //stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_mode = node.mode;
        stbuf->st_nlink = 1;
        stbuf->st_size = node.file_size;
    } else
        res = -ENOENT;

    return res;
}

int fs_readdir(const char *filepath, void *buf, fuse_fill_dir_t filler,
             off_t offset, struct fuse_file_info *fi)
{
    (void) offset;
    (void) fi;

    if (filepath[0] != '/')
        return -ENOENT;

    block_num inode_no = fs_namei(filepath);
    if(inode_no == 0){
        return -ENOENT;
    }
    struct inode node;
    read_inode(inode_no, &node);
    int n = 0;
    struct directory dir;

    //printf("Readdir last index of file %s:%ld",filepath,node.last_filled_block_index);
    while(n <= node.last_filled_block_index){
            block_num block_no = get_file_block_num(n, inode_no, false);
            //printf("readdir %s next - %dth block:%ld",filepath,n,block_no);
            if(block_no == 0){
                continue;
            }
            read_block(&dir, block_no, 0, sizeof(struct directory));
            int i = 0;
            struct stat stbuf;
            //stbuf.st_nlink = 1;
            //stbuf.st_size = 111;
            stbuf.st_mode = node.mode; //TODO: take this parameter from inode
            for(i=0; i<BLOCK_SIZE/NAMEI_ENTRY_SIZE; i++){
                //printf("\n file entry:%s.", dir.name[i]);
                if(dir.inode_num[i] != 0 ){
                    filler(buf, dir.name[i], &stbuf, 0);
                    //filler(buf, dir.name[i], NULL, 0);
                    //printf("file:%s\n", dir.name[i]);

                }    
            }
            n++;
        }
    //filler(buf, ".", NULL, 0);
    //filler(buf, "..", NULL, 0);
    //filler(buf, hello_path + 1, NULL, 0);

    return 0;
}
