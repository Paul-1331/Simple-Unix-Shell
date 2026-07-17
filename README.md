# MyShell
A modular Unix-like shell written in C.

## Features
- External command execution
- Built-in commands (`cd`, `exit`)
- Background execution (`&`)
- Single pipe support (`|`)
- Input (`<`) and output (`>`) redirection
- `SIGCHLD` signal handling for zombie process cleanup
- **Execution Time Measurements**: Measures and displays the execution time for foreground processes in seconds.
- **Command Suggestions**: Uses the Damerau-Levenshtein distance algorithm to dynamically suggest correct system commands when a typo occurs.

## Build
```bash
make
```

## Run
```bash
./myshell
```