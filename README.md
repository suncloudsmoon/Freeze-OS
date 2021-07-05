# Freeze-OS
![Freeze OS Logo](https://github.com/suncloudsmoon/Freeze-OS/blob/master/Freeze.png)
Freeze OS is a cross-platform operating system emulator that runs on top of an interpreter called the Freeze interpreter. The operating system code is basically written in the Freeze programming
language that is passed to the Freeze interpreter. The idea is to skip instances where the operating system needs to handle low level operators and focus on higher level stuff,
like malware analysis, AI, and others.

# How it works
This project's concept is an expansion of the idea that interpreters are virtual machines. But, interpreters still can
interact with the real operating system via file operations, etc. To combat this, a file written to "C:\Game" will be written instead to "%LOCALAPPDATA%\Freeze\VM #1\C\Game".

#Features
- Can print stuff
- Can access variables
- Can create a for loop (nested for loops don't work yet because of a "end" bug)

# Progress
1) Almost done with the if loop and method stuff
