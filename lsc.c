#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

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

#define MAX_TEXT_BUFFER 4096

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

	size_t longest_uid = 0;
	size_t longest_gid = 0;

	while ((element = readdir(directory)))
	{
		lstat(element->d_name, &file_information);

		struct passwd *pws = getpwuid(file_information.st_uid);
		struct group *grp = getgrgid(file_information.st_gid);

		if (strlen(pws->pw_name) > longest_uid)
		{
			longest_uid = strlen(pws->pw_name);
		}

		if (strlen(grp->gr_name) > longest_gid)
		{
			longest_gid = strlen(grp->gr_name);
		}
	}

	*uid_len = longest_uid;
	*gid_len = longest_gid;

	closedir(directory);
}

/**
 * @brief Returns the file permisssions of a file as a colored string.
 *
 * The returned string must be freed using free().
 *
 * TODO: Improve approach
 *
 * @param file file
 * @return char* colored permissions string
 */
char *get_file_permimssions_str(struct stat *file)
{
	// 7 chars for colors times 10 permission characters
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
	strcpy(perm_str + (6 * 2), (file->st_mode & S_IWUSR) ? write_perm : dash_perm);
	strcpy(perm_str + (6 * 3), (file->st_mode & S_IXGRP) ? exec_perm : dash_perm);
	strcpy(perm_str + (6 * 4), (file->st_mode & S_IRGRP) ? read_perm : dash_perm);
	strcpy(perm_str + (6 * 5), (file->st_mode & S_IWGRP) ? write_perm : dash_perm);
	strcpy(perm_str + (6 * 6), (file->st_mode & S_IXGRP) ? exec_perm : dash_perm);
	strcpy(perm_str + (6 * 7), (file->st_mode & S_IROTH) ? read_perm : dash_perm);
	strcpy(perm_str + (6 * 8), (file->st_mode & S_IWOTH) ? write_perm : dash_perm);
	strcpy(perm_str + (6 * 9), (file->st_mode & S_IXOTH) ? exec_perm : dash_perm);

	return perm_str;
}

/**
 * @brief Returns the size of a file as a string with its unit as a single letter.
 *
 * The returned string must be freed using free().
 *
 * TODO: Use a better approach, use multiples of two
 *
 * @param size size of the file
 * @return char* file size with the unit at the end
 */
char *get_file_size_str(off_t size)
{
	char *size_str = malloc(sizeof(char) * 8);

	if (size < 1000)
	{
		snprintf(size_str, sizeof(size_str), "%i " SHORT_BYTE, (int)size);
	}
	else if (size >= 1000 && size < 1000000)
	{
		snprintf(size_str, sizeof(size_str), "%0.1f " SHORT_KILOBYTE, (float)(size / 1000));
	}
	else if (size >= 1000000 && size < 1000000000)
	{
		snprintf(size_str, sizeof(size_str), "%0.1f " SHORT_MEGABYTE, (float)(size / 1000000));
	}
	else if (size >= 1000000000 && size < 1000000000000)
	{
		snprintf(size_str, sizeof(size_str), "%0.1f " SHORT_GIGABYTE, (float)(size / 1000000000));
	}
	else if (size >= 1000000000000 && size < 1000000000000000)
	{
		snprintf(size_str, sizeof(size_str), "%0.1f " SHORT_TERABYTE, (float)(size / 1000000000000));
	}
	else
	{
		snprintf(size_str, sizeof(size_str), "%0.1f " SHORT_PETABYTE, (float)(size / 1000000000000000));
	}

	return size_str;
}

/**
 * @brief Reads and prints a directory with colored output.
 *
 * @param directory_name directory name
 * @param show_directory_name whether the directory name should be shown
 * @return int 0 if no errors occurred, -1 otherwise
 */
int read_directory(char *directory_name, bool show_directory_name)
{
	// Old directory to change back to at the very end
	char *old_cwd = malloc(sizeof(char) * MAX_TEXT_BUFFER);
	getcwd(old_cwd, sizeof(char) * MAX_TEXT_BUFFER);

	// Full directory from which ls should be called
	char *cwd = malloc(sizeof(char) * MAX_TEXT_BUFFER);

	if (*directory_name == '/')
	{
		strcpy(cwd, directory_name);
	}
	else
	{
		getcwd(cwd, sizeof(char) * MAX_TEXT_BUFFER);
		strcat(cwd, "/");
		strcat(cwd, directory_name);
	}

	DIR *directory;

	if (chdir(cwd) != 0 || (directory = opendir(cwd)) == NULL)
	{
		chdir(old_cwd);
		free(old_cwd);
		free(cwd);

		return -1;
	}

	struct dirent *element;
	int number_of_files = 0;

	while ((element = readdir(directory)))
	{
		number_of_files++;
	}

	rewinddir(directory);

	int longest_uid = 0;
	int longest_gid = 0;

	get_longest_uid_and_gid_name(cwd, &longest_uid, &longest_gid);

	if (show_directory_name)
	{
		printf(ANSI_COLOR_MAGENTA "%s:\n", directory_name);
	}

	struct stat file_information;

	while ((element = readdir(directory)))
	{
		lstat(element->d_name, &file_information);

		struct passwd *pws = getpwuid(file_information.st_uid);
		struct group *grp = getgrgid(file_information.st_gid);

		int file_modified_time_str_len = 20;
		char file_modified_time_str[file_modified_time_str_len];

		struct tm *file_modified_time;
		file_modified_time = localtime(&file_information.st_mtime);

		strftime(file_modified_time_str, file_modified_time_str_len, TIME_FORMAT, file_modified_time);

		char *file_size_str = get_file_size_str(file_information.st_size);
		char *file_perm_str = get_file_permimssions_str(&file_information);

		char *symlink_buffer;
		ssize_t symlink_size = 0;

		if (S_ISLNK(file_information.st_mode))
		{
			symlink_buffer = malloc(sizeof(char) * MAX_TEXT_BUFFER);
			symlink_size = readlink(element->d_name, symlink_buffer, sizeof(char) * MAX_TEXT_BUFFER);
		}

		printf(ANSI_COLOR_MAGENTA "%s ", file_perm_str);
		printf(ANSI_COLOR_YELLOW "%-*s ", longest_uid + 2, pws->pw_name);
		printf(ANSI_COLOR_BLUE "%-*s ", longest_gid + 2, grp->gr_name);
		printf(ANSI_COLOR_RED "%7s ", file_size_str);
		printf(ANSI_COLOR_GREEN "%s ", file_modified_time_str);
		printf(ANSI_COLOR_MAGENTA "%s", element->d_name);

		if (S_ISLNK(file_information.st_mode))
		{
			printf(ANSI_COLOR_RESET " -> ");
			printf(ANSI_COLOR_RED "%.*s", (int)symlink_size, symlink_buffer);
			free(symlink_buffer);
		}

		printf("\n" ANSI_COLOR_RESET);

		free(file_size_str);
		free(file_perm_str);
	}

	if (show_directory_name)
	{
		printf("\n");
	}

	closedir(directory);

	chdir(old_cwd);
	free(old_cwd);
	free(cwd);

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
	int return_code = 0;
	if (argc < 2)
	{
		if (read_directory(".", 0) != 0)
		{
			printf(ANSI_COLOR_RESET PROGRAM_NAME ": Error with the current directory.\n");
			return_code = 1;
		}
	}
	else if (argc == 2)
	{
		if (read_directory(*(argv + 1), 0) != 0)
		{
			printf(ANSI_COLOR_RESET PROGRAM_NAME ": Error with the entered directory: %s\n", *(argv + 1));
			return_code = 1;
		}
	}
	else
	{
		for (int i = 1; i < argc; i++)
		{
			if (read_directory(*(argv + i), 1) != 0)
			{
				printf(ANSI_COLOR_RESET PROGRAM_NAME ": Error with the entered directory: %s\n", *(argv + i));
				return_code = 1;
			}
		}
	}

	return return_code;
}
