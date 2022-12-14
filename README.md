# Shell

### Shambhavi Jahagirdar
### Roll No : 2021111011

## Commands for defined processes
- ls, echo, pwd, cd, history, pinfo, discover run in foreground when called

### ->echo
- prints the user input after removing excess spaces

### ->ls 
- lists all the files and folders in current directory
- flag : -l : also print details of files and folders
         -a : also prints hidden files
- other arguments : ls x ==> ls in the directory x

### ->pinfo
- prints process information of current process / given pid

- includes: 
  - pid 
  - virtual memory
  - process status
  - executable path 

### ->discover 
- Searches and prints files in a directory hierarchy
- -d : searches for directories
- -f : searches for files

### ->history
- prints the last 10 recently executed commands
- stores last 20 commands in a file

### ->pwd
- prints the path of the current working directory


## System Commands

### -> Foreground :
- Other commands, written without '&' symbol, run as system commands in the foreground using execvp command
- The shell will create a new process and wait for it to finish, such that the new process runs in background

### -> Background : 
- If '&' symbol is specified after a command, then the process runs in the background
- The program maintains a list of processes running in the background
- The program also checks and prints the number of background processes finished (if any) after a command is entered

