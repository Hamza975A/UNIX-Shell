
# Custom UNIX Shell

A simple shell called wrdsh (weird shell) written in C.

## Authors

- [Hamza Aziz](https://github.com/Hamza975A)
- [Tanmay Pathak](https://github.com/tanmay-pathak)


## Documentation

- We implemented a shell which prints a prompt "wrdsh>" and can execute commands given by users.
- For each prompt, the user can give a single UNIX command or they can give multiple UNIX commands, using pipes and redirection.
- For each command inputed by the user, a process is created to execute that command (commands are executed using execvp()).
- The shell implements pipes and stdout redirection reading right to left rather than left to right.
- Whenever the characters “c”, “m”, “p”, and “t” appear in the stdout to the user, these characters are duplicated.
- Incorrect usage of redirection, pipes, and syntax errors in user input, is dealt with by the shell program.
- STDIN is not supported

## Assumptions

- Maximum number of commands in one user input can be 100.
- Maximum characters for individual command can be 100.
- Filename to redirect standard output would have a maximum length of 100 characters.
- User has permissions to read & write on a file.
- Standard output presented to the user would be of max length 1000 and with duplication of each character it would be of length 2000.