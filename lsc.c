#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

#define PROGRAM_NAME "lsc"

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

#define READ_PERM "r"
#define WRITE_PERM "w"
#define EXEC_PERM "x"

#define DIR_PERM "d"
#define LINK_PERM "l"
#define DEV_PERM "c"
#define BLOCK_PERM "b"
#define SOCK_PERM "s"
#define PIPE_PERM "p"
#define DASH_PERM "-"

#define TIME_FORMAT "%e %b %H:%M"

/**
 * Represents a row of a listing.
 */
/*typedef struct Entry
{
	char permission_string[10];
	char user_name[255];
	char group_name[255];
	char size_string[5];
	char date_string[13];
	char file_name[255];
	off_t size;
} Entry;*/

/**
 * @brief Helper function for sorting alphabetically.
 *
 * @param str1 first string
 * @param str2 second string
 * @return int difference between the strings
 */
/*int alpha_sort(const void *str1, const void *str2)
{
	return (strcmp((char *)str1, (char *)str2));
}*/

/**
 * @brief Returns the length longest user ID and group ID of a directory
 *	  with a given directory name.
 *
 * @param directory_name directory name
 * @param uid_len pointer to save the length of the longest user ID to
 * @param gid_len pointer to save the length of the longest group ID to
 */
void get_longest_uid_and_gid_name(char *directory_name, int *uid_len, int *gid_len)
{
	DIR *directory = opendir(directory_name);
	struct dirent *element;
	struct stat file_information;
	char *longest_uid = malloc(sizeof(char) * 255);
	char *longest_gid = malloc(sizeof(char) * 255);

	while ((element = readdir(directory)))
	{
		lstat(element->d_name, &file_information);

		struct passwd *pws = getpwuid(file_information.st_uid);
		struct group *grp = getgrgid(file_information.st_gid);

		if (strlen(pws->pw_name) > strlen(longest_uid))
		{
			strcpy(longest_uid, pws->pw_name);
		}

		if (strlen(grp->gr_name) > strlen(longest_gid))
		{
			strcpy(longest_gid, grp->gr_name);
		}
	}

	*uid_len = strlen(longest_uid);
	*gid_len = strlen(longest_gid);

	closedir(directory);

	free(longest_uid);
	free(longest_gid);
}

/**
 * @brief Returns the file permisssions of a file as a colored string.
 *
 * The returned string must be freed using free().
 *
 * @param file file
 * @return char* colored permissions string
 */
char *get_file_permimssions(struct stat *file)
{
	char *perm_str = malloc(sizeof(char) * 7 * 10);

	// TODO: Define symbolic constants or macros to avoid these variables
	char *read_perm = ANSI_COLOR_YELLOW READ_PERM;
	char *write_perm = ANSI_COLOR_GREEN WRITE_PERM;
	char *exec_perm = ANSI_COLOR_RED EXEC_PERM;
	char *dash_perm = ANSI_COLOR_BLACK DASH_PERM;

	char *dir_perm = ANSI_COLOR_BLUE DIR_PERM;
	char *link_perm = ANSI_COLOR_MAGENTA LINK_PERM;
	char *block_perm = ANSI_COLOR_CYAN BLOCK_PERM;
	char *dev_perm = ANSI_COLOR_MAGENTA DEV_PERM;
	char *sock_perm = ANSI_COLOR_BLUE SOCK_PERM;
	char *pipe_perm = ANSI_COLOR_CYAN PIPE_PERM;

	if (S_ISDIR(file->st_mode))
	{
		strcpy(perm_str, dir_perm);
	}
	else if (S_ISLNK(file->st_mode))
	{
		strcpy(perm_str, link_perm);
	}
	else if (S_ISBLK(file->st_mode))
	{
		strcpy(perm_str, block_perm);
	}
	else if (S_ISCHR(file->st_mode))
	{
		strcpy(perm_str, dev_perm);
	}
	else if (S_ISSOCK(file->st_mode))
	{
		strcpy(perm_str, sock_perm);
	}
	else if (S_ISFIFO(file->st_mode))
	{
		strcpy(perm_str, pipe_perm);
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

/**
 * @brief Returns the size of a file as a string with its unit as a single letter.
 *
 * The returned string must be freed using free().
 *
 * @param size size of the file
 * @return char* file size with the unit at the end
 */
char *get_file_size(off_t size)
{
	char *size_str = malloc(sizeof(char) * 8);

	if (size < 1000)
	{
		snprintf(size_str, sizeof(size_str), "%i " SHORT_BYTE, (int)size);
	}
	else if (size >= 1000 && size < 1000000)
	{
		snprintf(size_str, sizeof(size_str), "%0.1f " SHORT_KILOBYTE, (float)size / 1000);
	}
	else if (size >= 1000000 && size < 1000000000)
	{
		snprintf(size_str, sizeof(size_str), "%0.1f " SHORT_MEGABYTE, (float)size / 1000000);
	}
	else if (size >= 1000000000 && size < 1000000000000)
	{
		snprintf(size_str, sizeof(size_str), "%0.1f " SHORT_GIGABYTE, (float)size / 1000000000);
	}
	else if (size >= 1000000000000 && size < 1000000000000000)
	{
		snprintf(size_str, sizeof(size_str), "%0.1f " SHORT_TERABYTE, (float)size / 1000000000000);
	}
	else
	{
		snprintf(size_str, sizeof(size_str), "%0.1f " SHORT_PETABYTE, (float)size / 1000000000000000);
	}

	return size_str;
}

/**
 * @brief Reads and prints a directory with colored output.
 *
 * @param directory_name directory name
 * @param show_directory_name whether the directory name should be shown
 * @return int 0 if no errors occurred, 1 otherwise
 */
int read_directory(char *directory_name, int show_directory_name)
{
	char *cwd = malloc(sizeof(char) * 4096);
	getcwd(cwd, sizeof(char) * 4096);

	int chdir_code = 0;

	if (*directory_name != '/')
	{
		strcat(cwd, "/");
		strcat(cwd, directory_name);
		printf("%s", cwd);
		chdir_code = chdir(cwd);
	}
	else
	{
		strcpy(cwd, directory_name);
		chdir_code = chdir(directory_name);
	}

	if (chdir_code != 0)
	{
		return 1;
	}

	DIR *directory;
	if ((directory = opendir(cwd)) == NULL)
	{
		return 1;
	}

	struct dirent *element;
	struct stat file_information;
	int number_of_files = 0;

	while ((element = readdir(directory)))
	{
		number_of_files++;
	}

	rewinddir(directory);

	int longest_uid = 0;
	int longest_gid = 0;

	get_longest_uid_and_gid_name(cwd, &longest_uid, &longest_gid);

	if (show_directory_name == 1)
	{
		printf(ANSI_COLOR_MAGENTA "%s:\n", directory_name);
	}

	while ((element = readdir(directory)))
	{
		lstat(element->d_name, &file_information);

		struct passwd *pws = getpwuid(file_information.st_uid);
		struct group *grp = getgrgid(file_information.st_gid);

		char file_modified_time_str[20];
		struct tm *file_modified_time;
		file_modified_time = localtime(&file_information.st_mtime);
		strftime(file_modified_time_str, 20, TIME_FORMAT, file_modified_time);

		char *file_size = get_file_size(file_information.st_size);
		char *file_perm = get_file_permimssions(&file_information);

		char *symlink_buffer;
		ssize_t symlink_size = 0;

		if (S_ISLNK(file_information.st_mode))
		{
			symlink_buffer = malloc(sizeof(char) * 4096);
			symlink_size = readlink(element->d_name, symlink_buffer, sizeof(char) * 4096);
		}
		else
		{
			symlink_buffer = malloc(sizeof(char));
		}

		printf(ANSI_COLOR_MAGENTA "%s ", file_perm);
		printf(ANSI_COLOR_YELLOW "%-*s ", longest_uid + 2, pws->pw_name);
		printf(ANSI_COLOR_BLUE "%-*s ", longest_gid + 2, grp->gr_name);
		printf(ANSI_COLOR_RED "%7s ", file_size);
		printf(ANSI_COLOR_GREEN "%s ", file_modified_time_str);
		printf(ANSI_COLOR_MAGENTA "%s", element->d_name);

		if (S_ISLNK(file_information.st_mode))
		{
			printf(ANSI_COLOR_RESET " -> ");
			printf(ANSI_COLOR_RED "%.*s", (int)symlink_size, symlink_buffer);
		}

		printf("\n");

		free(file_size);
		free(file_perm);
		free(symlink_buffer);
	}

	if (show_directory_name == 1)
	{
		printf("\n");
	}

	printf(ANSI_COLOR_RESET);

	//chdir(cwd);
	free(cwd);
	closedir(directory);

	/*
	for (int i = 0; i < number_of_files; i++)
	{

	}
	*/

	return 0;
}

/**
 * @brief Main function.
 *
 * @param argc number of command line arguments
 * @param argv list command line arguments
 * @return int return code
 */
int main(int argc, char *argv[])
{
	char *directory_to_read;

	if (argc < 2)
	{
		if (read_directory(".", 0) != 0)
		{
			printf(ANSI_COLOR_RESET PROGRAM_NAME ": Error with the current directory.\n");
		}
	}
	else if (argc == 2)
	{
		if (read_directory(*(argv + 1), 0) != 0)
		{
			printf(ANSI_COLOR_RESET PROGRAM_NAME ": Error with the entered directory: %s\n", *(argv + 1));
		}
	}
	else
	{
		for (int i = 1; i < argc; i++)
		{
			char *current_dir = *(argv + i);
			if (read_directory(current_dir, 1) != 0)
			{
				printf(ANSI_COLOR_RESET PROGRAM_NAME ": Error with the entered directory: %s\n", current_dir);
			}
		}
	}

	return 0;
}
