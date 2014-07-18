brain fuck
==========
yep, another brain fuck interpreter, as if there weren't enough already.
##building
* type ```make```
* figure the rest out

##usage
or you could just type ```-h```/```--help``` to get the same message
```
whatever you named the command (default bfuck or something): [options] <file | -p | -i>
options:
  -h  --help              hmmmmmmmmmm
  -d  --debug             run in debug mode
  -e  --eof   <num>       value to set when eof (default is to do nothing)
  -p  --prog <prog_str>   get program from argument instead of file
  -m  --mem  <cells | 0>  how many cells to allocate (default 256)
  -n  --noneg             don't allow cells to be negative
  -o  --output            output stripped program and exit
  -i  --stdin             get program from stdin until EOF
  -c  --compile           compile to C program

debug options:
  -s  --step  <step>      break at step (overrides -b)
  -b  --break <point>     put break point at position
```
##yeah, that's it
