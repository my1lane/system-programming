#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

int main(int argc, char *argv[]) {
    DIR* dir;
    struct dirent* dir_entry;
    struct stat dir_entry_stat;
    const char* current_dir_path = ".";
    int stat_result;
    int file_types_counts[] = {0, -2, 0, 0, 0, 0, 0};
    char* file_types[] = {
        "regulars",
        "directories",
        "blocks",
        "chars",
        "links",
        "fifos",
        "sockets",
    };
    size_t file_types_seize = sizeof(file_types) / sizeof(file_types)[0];

    if ((dir = opendir(current_dir_path)) == NULL) {
        perror("Error opening current directory");
        exit(1);
    }

    while ((dir_entry = readdir(dir)) != NULL) {
        if (stat(dir_entry->d_name, &dir_entry_stat) != 0 ) {
            printf("Error stat entry('%s')\n", dir_entry->d_name);
            continue;
        }

        if (S_ISREG(dir_entry_stat.st_mode)) {
            file_types_counts[0]++;
        } else if (S_ISDIR(dir_entry_stat.st_mode))    {
            file_types_counts[1]++;
        } else if (S_ISBLK(dir_entry_stat.st_mode))    {
            file_types_counts[2]++;
        } else if (S_ISCHR(dir_entry_stat.st_mode))    {
            file_types_counts[3]++;
        } else if (S_ISLNK(dir_entry_stat.st_mode))    {
            file_types_counts[4]++;
        } else if (S_ISFIFO(dir_entry_stat.st_mode))    {
            file_types_counts[5]++;
        } else if (S_ISSOCK(dir_entry_stat.st_mode))    {
            file_types_counts[6]++;
        }
    }

    for (size_t i = 0; i < file_types_seize; i++) {
        printf("%s files count - %d\n", file_types[i], file_types_counts[i]);
    }

    closedir(dir);

    return 0;
}
