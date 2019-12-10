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

2. bash:
  - redirection modifies the previous program's I/O behavior
  - < means replace stdin
  - > means write, default: 1 (stdout)
  - >> = append, default: 1 (stdout)

3. redirection & | together: > on the first cmd will happen to the first command, > on the second command happens to the whole thing
  - for "<" this shouldn't really matter
