#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

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
#define SHORT_TERRABYTE "T"
#define SHORT_PETABYTE "P"

typedef struct Entry
{

} Entry;

int get_longest_uid_name()
{
}

int get_longest_gid_name()
{
}

char *get_file_permimssions()
{
}

char *get_file_size(off_t size)
{
	char *size_str = malloc(sizeof(char) * 6);

	if (size < 1000)
	{
		sprintf(size_str, "%i %s", (int)size, SHORT_BYTE);
	}
	else if (size >= 1000)
	{
		sprintf(size_str, "%0.1f %s", (float)size / 1000, SHORT_KILOBYTE);
	}
	else if (size >= 1000000)
	{
		sprintf(size_str, "%0.1f %s", (float)size / 1000000, SHORT_MEGABYTE);
	}
	else if (size >= 1000000000)
	{
		sprintf(size_str, "%0.1f %s", (float)size / 1000000000, SHORT_GIGABYTE);
	}
	else if (size >= 1000000000000)
	{
		sprintf(size_str, "%0.1f %s", (float)size / 1000000000000, SHORT_TERRABYTE);
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
		pws = getpwuid(file_information.st_uid);

		struct group *grp;
		grp = getgrgid(file_information.st_gid);

		char file_modified_time_str[20];
		struct tm *file_modified_time;
		file_modified_time = localtime(&file_information.st_mtime);
		strftime(file_modified_time_str, 20, "%d %b %H:%M", file_modified_time);

		char *file_size = get_file_size(file_information.st_size);

		printf("%s%s ", ANSI_COLOR_YELLOW, pws->pw_name);
		printf("%s%s ", ANSI_COLOR_BLUE, grp->gr_name);
		printf("%s%7s ", ANSI_COLOR_RED, file_size);
		printf("%6s%s ", ANSI_COLOR_MAGENTA, file_modified_time_str);
		printf("%s ", element->d_name);
		printf("\n");

		free(file_size);
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
