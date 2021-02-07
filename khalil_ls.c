#include <stdlib.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <sys/stat.h> 
#include <dirent.h> 
#include <string.h> 
#include <sys/dir.h> 
#include <sys/types.h> 
#include <pwd.h> 
#include <grp.h> 
#include <time.h> 
#include <locale.h> 
 

int main(int argc, char * argv[]) { 

    char * dir_path;    
    if(argc < 2)
    {
        dir_path = ".";
        list_files(dir_path);
    }    
    else{
        int i = 1;
        while (i < argc)
        {
            dir_path = argv[i];            
            list_files(dir_path);
            i += 1;
        }
    }    
    return 0; 
}

void list_files(const char* dir_path) { 
    struct dirent * file; 
    struct stat file_stat; 
    char str_temp[255]; 
    DIR * dir_chosen;    
    struct passwd pwent, * pwentp; 
    struct group grp, * grpt;
    
    dir_chosen = opendir(dir_path);
    if(dir_chosen == NULL){
        printf("ls: cannot access '%s' No such file or directory\n", dir_path);
        return;
    }
    printf("\nDirectory '%s'\n\n", dir_path);

    //file=readdir(dir_chosen) does not give accurate result for absolute path
    while(file=readdir(dir_chosen))
    {
        stat(file->d_name, &file_stat);
        format_permissions(file_stat.st_mode);
        printf(" %ld", file_stat.st_nlink);
        if (!getpwuid_r(file_stat.st_uid, &pwent, str_temp, sizeof(str_temp), &pwentp)) 
            printf(" %s", pwent.pw_name); 
        else
            printf(" %d", file_stat.st_uid); 
 
        if (!getgrgid_r (file_stat.st_gid, &grp, str_temp, sizeof(str_temp), &grpt)) 
            printf(" %s", grp.gr_name); 
        else 
            printf(" %d", file_stat.st_gid);
        if(file_stat.st_size == 0)
        {
            printf(" %s", "0000");
        }
        else{
            printf(" %ld", file_stat.st_size);
        }
        struct tm * timeinfo = localtime (&file_stat.st_mtime);
        char str_time1[30];
        strftime(str_time1, 30, "%b %d %H:%M", timeinfo);
        printf(" %s %s\n", str_time1, file->d_name); 
    } 
}

void format_permissions(mode_t st) { 
    char perms[10]; 
    if(st && S_ISREG(st)) perms[0]='-'; 
    else if(st && S_ISDIR(st)) perms[0]='d'; 
    else if(st && S_ISFIFO(st)) perms[0]='|'; 
    else if(st && S_ISSOCK(st)) perms[0]='s'; 
    else if(st && S_ISCHR(st)) perms[0]='c'; 
    else if(st && S_ISBLK(st)) perms[0]='b'; 
    else perms[0]='l';  // S_ISLNK 
    perms[1] = (st && S_IRUSR) ? 'r':'-'; 
    perms[2] = (st && S_IWUSR) ? 'w':'-'; 
    perms[3] = (st && S_IXUSR) ? 'x':'-'; 
    perms[4] = (st && S_IRGRP) ? 'r':'-'; 
    perms[5] = (st && S_IWGRP) ? 'w':'-'; 
    perms[6] = (st && S_IXGRP) ? 'x':'-'; 
    perms[7] = (st && S_IROTH) ? 'r':'-'; 
    //following does not exactly match with default output of ls -al
    perms[8] = (st && S_IWOTH) ? 'w':'-'; 
    perms[9] = (st && S_IXOTH) ? 'x':'-'; 
    //perms[10] = '\0'; 
    printf("%s", perms); 
}
