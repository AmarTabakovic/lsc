# lsc
Custom `ls` implementation with colors in C. Inspired by [Natls](https://github.com/willdoescode/nat)

## Preview
![Preview](docs/preview.png)

## Usage

### Print contents of current directory

```bash
$ ./lsc 
```

### Print contents of a given directory
```bash
$ ./lsc [directory_name]
```

### Print contents of multiple directories
```bash
$ ./lsc [directory_name_1] [directory_name_2]
```

## Todo
- Options and flags
	- `-l` for displaying informations about the file
	- `-s` for sorting according to the file size
	- `-d` for displaying the created date instead of modified date
	- and more
- File names as a parameter (instead of just directories)


