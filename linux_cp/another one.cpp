#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#define BUF_LEN 1024

using namespace std;

// stitch the pathname
void cat_path(char * path, const char * str, char * name)
{
    strcpy(path, str);
    strcat(path, "/");
    strcat(path, name);
}

// traverse the directory
void dfs_dir(const char * str, const char * str2)
{
    DIR * dir;
    struct dirent * ptr;
    struct stat buf;
    char path[256], path2[256];

    dir = opendir(str);

    while((ptr = readdir(dir)) != NULL)     // traverse the directory
    {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)  // skip the directory itself
            continue;

        cat_path(path, str, ptr->d_name);
        cat_path(path2, str2, ptr->d_name);
        if (access(path2, F_OK) < 0)    // to copy
        {
            stat(path, &buf);
            if (ptr->d_type == DT_DIR)      // copy the directory
            {
                // create a directory
                mkdir(path2, buf.st_mode);

                // traverse the directory
                dfs_dir(path, path2);
            }
            else if (ptr->d_type == DT_REG)     // copy the regular file
            {
                char buff[BUF_LEN];
                int rv1 = -1, rv2 = -1;

                int fd = open(path, O_RDONLY);
                // create a file
                int fd2 = open(path2, O_RDWR|O_CREAT|O_TRUNC, buf.st_mode);

                // copy the file
                while ((rv1 = read(fd, buff, sizeof(buff))) != 0)
                {
                    if ((rv2 = write(fd2, buff, rv1)) < 0)
                    {
                        printf("%s writes failure\n", ptr->d_name);
                    }
                }

                if (fd > 0)
                    close(fd);
                if (fd2 > 0)
                    close(fd2);
            }
            else if (ptr->d_type == DT_LNK)     // copy the link file
            {
                char buff[BUF_LEN];
                int ret = readlink(path, buff, BUF_LEN-1);
                buff[ret] = '\0';
                symlink(buff, path2);
            }
            else
                continue;

            printf("Replication of '%s' succeeded!\n", ptr->d_name);
        }
        else {    // if exists
            printf("'%s' has existed.\n", ptr->d_name);
            if (ptr->d_type == DT_DIR)
            {
                // traverse its subdirectory
                dfs_dir(path, path2);
            }
        }
    }
    closedir(dir);
}

int main()
{
    const char * str = "/home/aba/Desktop/linux-5.19.10";
    const char * str2 = "/home/aba/Desktop/linux-5.19.10bak";

    struct stat buf;
    if (access(str2, F_OK) < 0)
	{
        stat(str, &buf);
        mkdir(str2, buf.st_mode);
	}

    dfs_dir(str, str2);
	return 0;
}
