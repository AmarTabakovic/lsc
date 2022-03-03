#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

#define ANSI_COLOR_BLACK "\x1b[30m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define SHORT_BYTE "B"
#define SHORT_KILOBYTE "K"
#define SHORT_MEGABYTE "M"
#define SHORT_GIGABYTE "G"
#define SHORT_TERABYTE "T"
#define SHORT_PETABYTE "P"

typedef struct Entry
{
	char permission_string[10];
	char user_name[255];
	char group_name[255];
	char size_string[5];
	long size;
	char date_string[13];
	char file_name[255];
} Entry;

int alpha_sort(const void *str1, const void *str2)
{
	return (strcmp((char *)str1, (char *)str2));
}

int get_longest_uid_name()
{
}

int get_longest_gid_name()
{
}

char *get_file_permimssions(struct stat *file)
{
	char *perm_str = malloc(sizeof(char) * 7 * 10);

	char *read_perm = ANSI_COLOR_YELLOW "r";
	char *write_perm = ANSI_COLOR_GREEN "w";
	char *exec_perm = ANSI_COLOR_RED "x";
	char *dash_perm = ANSI_COLOR_BLACK "-";
	char *dir = ANSI_COLOR_BLUE "d";
	char *link = ANSI_COLOR_MAGENTA "l";

	if (S_ISDIR(file->st_mode))
	{
		strcpy(perm_str, dir);
	}
	else if (S_ISLNK(file->st_mode))
	{
		strcpy(perm_str, link);
	}
	else
	{
		strcpy(perm_str, dash_perm);
	}

	strcpy(perm_str + 6, (file->st_mode & S_IRUSR) ? read_perm : dash_perm);
	strcpy(perm_str + 12, (file->st_mode & S_IWUSR) ? write_perm : dash_perm);
	strcpy(perm_str + 18, (file->st_mode & S_IXGRP) ? exec_perm : dash_perm);
	strcpy(perm_str + 24, (file->st_mode & S_IRGRP) ? read_perm : dash_perm);
	strcpy(perm_str + 30, (file->st_mode & S_IWGRP) ? write_perm : dash_perm);
	strcpy(perm_str + 36, (file->st_mode & S_IXGRP) ? exec_perm : dash_perm);
	strcpy(perm_str + 42, (file->st_mode & S_IROTH) ? read_perm : dash_perm);
	strcpy(perm_str + 48, (file->st_mode & S_IWOTH) ? write_perm : dash_perm);
	strcpy(perm_str + 54, (file->st_mode & S_IXOTH) ? exec_perm : dash_perm);

	return perm_str;
}

char *get_file_size(off_t size)
{
	char *size_str = malloc(sizeof(char) * 6);

	if (size < 1000)
	{
		sprintf(size_str, "%i %s", (int)size, SHORT_BYTE);
	}
	else if (size >= 1000 && size < 1000000)
	{
		sprintf(size_str, "%0.1f %s", (float)size / 1000, SHORT_KILOBYTE);
	}
	else if (size >= 1000000 && size < 1000000000)
	{
		sprintf(size_str, "%0.1f %s", (float)size / 1000000, SHORT_MEGABYTE);
	}
	else if (size >= 1000000000 && size < 1000000000000)
	{
		sprintf(size_str, "%0.1f %s", (float)size / 1000000000, SHORT_GIGABYTE);
	}
	else if (size >= 1000000000000 && size < 1000000000000000)
	{
		sprintf(size_str, "%0.1f %s", (float)size / 1000000000000, SHORT_TERABYTE);
	}
	else
	{
		sprintf(size_str, "%0.1f %s", (float)size / 1000000000000000, SHORT_PETABYTE);
	}

	return size_str;
}

void read_directory(char *directory_name)
{
	DIR *directory = opendir(directory_name);
	struct dirent *element;
	struct stat file_information;

	chdir(directory_name);

	while ((element = readdir(directory)))
	{
		stat(element->d_name, &file_information);

		struct passwd *pws;
		struct group *grp;

		pws = getpwuid(file_information.st_uid);
		grp = getgrgid(file_information.st_gid);

		char file_modified_time_str[20];
		struct tm *file_modified_time;
		file_modified_time = localtime(&file_information.st_mtime);
		strftime(file_modified_time_str, 20, "%d %b %H:%M", file_modified_time);

		char *file_size = get_file_size(file_information.st_size);
		char *file_perm = get_file_permimssions(&file_information);

		printf("%s%s ", ANSI_COLOR_MAGENTA, file_perm);
		printf("%s%s ", ANSI_COLOR_YELLOW, pws->pw_name);
		printf("%s%s ", ANSI_COLOR_BLUE, grp->gr_name);
		printf("%s%7s ", ANSI_COLOR_RED, file_size);
		printf("%6s%s ", ANSI_COLOR_GREEN, file_modified_time_str);
		printf("%s%s ", ANSI_COLOR_MAGENTA, element->d_name);
		printf("\n");

		free(file_size);
		free(file_perm);
	}

	closedir(directory);
}

int main(int argc, char *argv[])
{
	char *directory_to_read;

	if (argc < 2)
	{
		directory_to_read = ".";
	}
	else if (argc > 2)
	{
		printf("You can only enter 1 directory\n");
		return 1;
	}
	else
	{
		directory_to_read = argv[1];
	}

	read_directory(directory_to_read);

	return 0;
}
