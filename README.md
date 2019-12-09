TODO:
1. deal with ^D
2. write a better parser
3. use readline lib
4. handle colors


For colors, check out this link:
http://www.lihaoyi.com/post/BuildyourownCommandLinewithANSIescapecodes.html

Feature testing:
1. bash: multiple input redirect == last one
#### ruoshui@Known:~/code/systems/p1_shell/test$ wc < bashin.l 
####   84  217 1508
#### ruoshui@Known:~/code/systems/p1_shell/test$ wc < out.txt 
#### 0 0 0
#### ruoshui@Known:~/code/systems/p1_shell/test$ wc < ls
####  8  8 58
#### ruoshui@Known:~/code/systems/p1_shell/test$ wc < ls < bashin.l 
####   84  217 1508
#### ruoshui@Known:~/code/systems/p1_shell/test$ wc < ls < calc.l < bashin.l 
####   84  217 1508

