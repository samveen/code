# Samveen's Example OS

This is my Bachelor's thesis project which helped me learn a lot of things, the
most important 3 of which were:
1. I knew jack shit.
2. To get work done, I could remedy point #1 as required.
3. I should've paid more attention to the books instead of complaining about 
   my stupid college's lack of qualified teaching staff and infrastructure.

## Parts

- `seos` - Kernel, command interpeter, hello world example
- `seos_fs` - Floppy file system image generator
- `requirements` software requirements to build and run SEOS in a Windows XP environment

## License

The SEOS source is released under the HIRE ME/PAY ME License (Modified 2 Clause BSD License).
See LICENSE file for details.

Windows XP is a trademark of MS.

The contents of `requirements` are copyright of their respective owners.

## Caveat Emptor

1. The code in this repo is OLD. The C and C++ standards have changed a
   lot since this code was written, and most modern compilers will refuse
   to compile the C/C++ parts of it.
2. This is real mode code.
3. Vim only: All code is `fileformat=dos`
