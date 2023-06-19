#include<stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <utime.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <math.h>
#include <stdlib.h>
#define tf "-tf"
#define cf "-cf"
#define rf "-rf"
#define xf "-xf"
typedef struct tar_one
{                              /* byte offset */
  char name[100];               /*   0 */
  char mode[8];                 /* 100 */
  char uid[8];                  /* 108 */
  char gid[8];                  /* 116 */
  char size[12];                /* 124 */
  char mtime[12];               /* 136 */
  char chksum[8];               /* 148 */
  char typeflag;                /* 156 */
  char linkname[100];           /* 157 */
  char magic[6];                /* 257 */
  char version[2];              /* 263 */
  char uname[32];               /* 265 */
  char gname[32];               /* 297 */
  char devmajor[8];             /* 329 */
  char devminor[8];             /* 337 */
  char prefix[155];             /* 345 */
  char test[12];                             /* 500 */
}tar_one;

void null_fill(int num,char* str)
{
    int i = 0;
    while(i < num)
    {
        str[i] = 0;
        i++;
    }
}

void tar_fill_null(tar_one* tar)
{
    null_fill(sizeof(tar->name), tar->name);
    null_fill(sizeof(tar->mode), tar->mode);
    null_fill(sizeof(tar->uid), tar->uid);
    null_fill(sizeof(tar->gid), tar->gid);
    null_fill(sizeof(tar->size), tar->size);
    null_fill(sizeof(tar->mtime), tar->mtime);
    null_fill(sizeof(tar->chksum), tar->chksum);
    tar->typeflag = '0';
    null_fill(sizeof(tar->linkname), tar->linkname);
    null_fill(sizeof(tar->magic), tar->magic);
    null_fill(sizeof(tar->version), tar->version);
    null_fill(sizeof(tar->uname), tar->uname);
    null_fill(sizeof(tar->gname), tar->gname);
    null_fill(sizeof(tar->devmajor), tar->devmajor);
    null_fill(sizeof(tar->devminor), tar->devminor);
    null_fill(sizeof(tar->prefix), tar->prefix);
    null_fill(sizeof(tar->test), tar->test);
}

void rever_string(char* str)
{
    int len,tmpry;
    len = strlen(str);
    for(int i = 0; i < len/2;i++)
    {
        tmpry = str[i];
        str[i]=str[len - 1 - i];
        str[len - 1 - i] = tmpry;
    }
}
char* dec_to_octal(int n)
{
    // array to store octal number
    char* octalNum = (char*)malloc(sizeof(char)* 200);
  
    // counter for octal number array
    int i = 0;
    while (n != 0) {
  
        // storing remainder in octal array
        octalNum[i] = n % 8 + '0';
        n = n / 8;
        i++;
    }
  
    // printing octal number array in reverse order
    rever_string(octalNum);
    return octalNum;
}

int sum_index(char* str)
{
    int n = 0;
    for(int i = 0; str[i] != '\0'; i++)
    {
        n += str[i];
    }
    return n;
}

int my_chksum(tar_one *index)
{
        int count1 = 0;
        
        count1 += sum_index(index->gid);
        count1 += sum_index(index->size);
        count1 += sum_index(index->uname);
        count1 += sum_index(index->mtime);
        count1 += sum_index(index->magic);
        count1 += sum_index(index->gname);
        count1 += sum_index(index->name);
        count1 += sum_index(&index->typeflag);
        count1 += sum_index(index->mode);
        count1 += sum_index(index->uid);
        count1 += 256;
        return count1;
}

char* my_file_writer(char* str)
{
    struct stat a;
    stat(str,&a);

    char* str4 = (char*)malloc(sizeof(char)* 2000);

    int count2 = open(str, O_RDONLY);
    read(count2,str4,a.st_size);
    return str4;
}

int check(char* av[],int index)
{
    DIR *directory;
    struct dirent *entry;
        
    directory = opendir(".");

    if(directory == NULL)
    {
        printf(" There is no file.\n");
    }

    while((entry = readdir(directory)) != NULL)
    {
        if(entry->d_name != av[index + 1]) 
        {
            printf("my_tar: %s: Cannot stat: No such file or directory\n" , av[index + 1]);
            return -1;
        }
    }
    return 0;

}
int creation(int ac,char* av[],int index)
{
    creat(av[index], 000644);
    struct stat fs;
    stat(av[index + 1],&fs);
    tar_one tar;
    check(av,index);
    for(int fname = index + 1; fname < ac;fname++)
    {

        int file1 = open(av[index], O_WRONLY | O_APPEND);

        tar_fill_null(&tar);

        for(int i = 0; av[fname][i];i++)
        {
            tar.name[i] = av[fname][i]; 
        }

        char* mode = dec_to_octal(fs.st_mode);
        for(long unsigned int i = 0, ind = 0; i < sizeof(tar.mode); i++)
        {
            if(i == 0)
                tar.mode[i++] = '0';
            tar.mode[i] = mode[ind++];
        }

        char* uid = dec_to_octal(fs.st_uid);
        for(long unsigned int i = 0, ind = 0; i < sizeof(tar.uid); i++)
        {
            if(i == 0)
                tar.uid[i++] = '0';
            if(i == 1)
                tar.uid[i++] = '0';
            if(i == 2)
                tar.uid[i++] = '0';
            tar.uid[i] = uid[ind++];
        }

        char* gid = dec_to_octal(fs.st_gid);
        for(long unsigned int i = 0, ind = 0; i < sizeof(tar.gid); i++)
        {
            if(i == 0)
                tar.gid[i++] = '0';
            if(i == 1)
                tar.gid[i++] = '0';
            if(i == 2)
                tar.gid[i++] = '0';
            tar.gid[i] = gid[ind++];
        }

        char* size = dec_to_octal(fs.st_size);
        for(long unsigned int i = 0, ind = 0; i < sizeof(tar.size); i++)
        {
            if(i == 0)
                tar.size[i++] = '0';
            if(i == 1)
                tar.size[i++] = '0';
            if(i == 2)
                tar.size[i++] = '0';
            if(i == 3)
                tar.size[i++] = '0';
            if(i == 4)
                tar.size[i++] = '0';
            if(i == 5)
                tar.size[i++] = '0';
            tar.size[i] = size[ind++];
        }

        char* mtime = dec_to_octal(fs.st_mtim.tv_sec);
        for(long unsigned int i = 0, ind = 0; i < sizeof(tar.mtime); i++)
        {
            tar.mtime[i] = mtime[ind++];
        }

        strcpy(tar.magic, "ustar  ");
        strcpy(tar.uname, "docode");
        strcpy(tar.gname, "docode");

        char* chksum1 = dec_to_octal(my_chksum(&tar));
        for(long unsigned int i = 0, ind = 0; i < sizeof(tar.chksum);i++)
        {
            if(i == 0)
                tar.chksum[i++] = '0';
            tar.chksum[i] = chksum1[ind++];
            if(i == sizeof(tar.chksum) - 1)
                tar.chksum[i] = ' ';
        }

        char* str = (char*)malloc(sizeof(char) * 2000); 
        str = my_file_writer(av[fname]);
        
        long a = fs.st_size;
        long size2 = (a / 513 + 1) * 512;
        char content[a];
        null_fill(a, content);        
        
        if(write(file1, &tar, sizeof(tar))  < 0) {
                return 1;
            } 
            if(write(file1, str, size2) < 0){
                return 1;
            };

        close(file1);
    }
    return 0;
}

int adding_tar(int ac,char* av[],int index)
{
    struct stat fs;
    stat(av[index + 1],&fs);
    tar_one tar;

    for(int fname = index + 1; fname < ac;fname++)
    {
        int file1 = open(av[index], O_WRONLY | O_APPEND);

        tar_fill_null(&tar);

        for(int i = 0; av[fname][i];i++)
        {
            tar.name[i] = av[fname][i]; 
        }

        char* mode = dec_to_octal(fs.st_mode);
        for(long unsigned int i = 0, ind = 0; i < sizeof(tar.mode); i++)
        {
            if(i == 0)
                tar.mode[i++] = '0';
            tar.mode[i] = mode[ind++];
        }

        char* uid = dec_to_octal(fs.st_uid);
        for(long unsigned int i = 0, ind = 0; i < sizeof(tar.uid); i++)
        {
            if(i == 0)
                tar.uid[i++] = '0';
            if(i == 1)
                tar.uid[i++] = '0';
            if(i == 2)
                tar.uid[i++] = '0';
            tar.uid[i] = uid[ind++];
        }

        char* gid = dec_to_octal(fs.st_gid);
        for(long unsigned int i = 0, ind = 0; i < sizeof(tar.gid); i++)
        {
            if(i == 0)
                tar.gid[i++] = '0';
            if(i == 1)
                tar.gid[i++] = '0';
            if(i == 2)
                tar.gid[i++] = '0';
            tar.gid[i] = gid[ind++];
        }

        char* size = dec_to_octal(fs.st_size);
        for(long unsigned int i = 0, ind = 0; i < sizeof(tar.size); i++)
        {
            if(i == 0)
                tar.size[i++] = '0';
            if(i == 1)
                tar.size[i++] = '0';
            if(i == 2)
                tar.size[i++] = '0';
            if(i == 3)
                tar.size[i++] = '0';
            if(i == 4)
                tar.size[i++] = '0';
            if(i == 5)
                tar.size[i++] = '0';
            tar.size[i] = size[ind++];
        }

        char* mtime = dec_to_octal(fs.st_mtim.tv_sec);
        for(long unsigned int i = 0, ind = 0; i < sizeof(tar.mtime); i++)
        {
            tar.mtime[i] = mtime[ind++];
        }

        strcpy(tar.magic, "ustar  ");
        strcpy(tar.uname, "docode");
        strcpy(tar.gname, "docode");

        char* chksum1 = dec_to_octal(my_chksum(&tar));
        for(long unsigned int i = 0, ind = 0; i < sizeof(tar.chksum);i++)
        {
            if(i == 0)
                tar.chksum[i++] = '0';
            tar.chksum[i] = chksum1[ind++];
            if(i == sizeof(tar.chksum) - 1)
                tar.chksum[i] = ' ';
        }

        char* str = (char*)malloc(sizeof(char) * 2000); 
        str = my_file_writer(av[fname]);
        
        long a = fs.st_size;
        long size2 = (a / 513 + 1) * 512;
        char content[a];
        null_fill(a, content);        
        
        if(write(file1, &tar, sizeof(tar))  < 0) {
                return 1;
            } 
            if(write(file1, str, size2) < 0){
                return 1;
            };

        close(file1);
    }
    return 0;
}

void print_archieve(char* av[])
{
    tar_one tar;
    int file2 = open(av[2], O_RDONLY);
    for(;read(file2,&tar,512) > 0;)
    {
        if(strcmp(tar.uname, "docode") == 0)
        {
            puts(tar.name);
        }
    }
}
int	my_pow(int nb, int pow)
{
  int	p;
  int	result;

  p = 1;
  result = 1;
  if (pow < 0)
    return (0);
  if (pow == 0)
    return (1);
  while (p <= pow)
    {
      result = result * nb;
      p = p + 1;
    }
  return (result);
}
int OctalToDecimal(char* num)
{
    int number = atoi(num);
    int x = 0;
    int ans = 0;

    while (number > 0)
    {

        // obtaining the right-most digit of num
        int y = number % 10;

        // Since the right-most digits were previously scanned in the previous step, divide num by 10 to remove them.
        number /= 10;

        ans += y * my_pow(8, x);
        ++x;
    }

    return ans;
}

int extract(char* av[],int index){
   
   int fd = open(av[index], O_RDONLY);
   tar_one tar;
   for(;( read(fd, &tar, 512) ) > 0;)
   {
        creat(tar.name, OctalToDecimal(tar.mode));
        long size = OctalToDecimal(tar.size);
        int file = open(tar.name, O_WRONLY | O_APPEND);
        char fl_content[size];
        read(fd, fl_content, size);
        write(file, fl_content, size);
        
        int ok = (size/513 + 1) * 512;
        ok -= size;
        lseek(fd, ok, SEEK_CUR);
   }
   close(fd);
   return 0;
}
int main(int argc, char* argv[])
{  
    if(argc > 2)
    {
        if(strcmp(argv[1],cf) == 0)
        {
            creation(argc,argv,2);
        }
        if(strcmp(argv[1],rf) == 0)
        {
            adding_tar(argc,argv,2);
        }
        if(strcmp(argv[1],tf) == 0)
        {
            print_archieve(argv);
        }
        if(strcmp(argv[1],xf) == 0)
        {
            extract(argv,2);
        }
    }
    return 0;
}