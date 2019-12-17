TODO:
1. deal with ^D
2. write a better parser
3. use readline lib
4. handle colors


For colors, check out this link:
http://www.lihaoyi.com/post/BuildyourownCommandLinewithANSIescapecodes.html

Feature testing:
1. bash: multiple input redirect == last one
```bash
ruoshui@Known:~/code/systems/p1_shell/test$ wc < bashin.l
  84  217 1508
ruoshui@Known:~/code/systems/p1_shell/test$ wc < out.txt
0 0 0
ruoshui@Known:~/code/systems/p1_shell/test$ wc < ls
 8  8 58
ruoshui@Known:~/code/systems/p1_shell/test$ wc < ls < bashin.l
  84  217 1508
ruoshui@Known:~/code/systems/p1_shell/test$ wc < ls < calc.l < bashin.l
  84  217 1508```

```bash
ruoshui@Known:~/code/systems/p1_shell$ grep h 0< out
ast.h
bashin.l
cmds.h
colors.h
parser.h
shell.c
shell.h
utils.h
ruoshui@Known:~/code/systems/p1_shell$ grep h 1< out
jkh 
hljkh
jklasjdfh
grep: write error: Bad file descriptor
ruoshui@Known:~/code/systems/p1_shell$ grep h 2< out
asdfh
asdfh
jkalshdh
jkalshdh
jaklsdfh
jaklsdfh
ruoshui@Known:~/code/systems/p1_shell$ cat out
ast.h
bashin.l
cmds.h
colors.h
parser.h
shell.c
shell.h
utils.h```

2. bash:
  - redirection modifies the previous program's I/O behavior
  - < means replace stdin
  - > means write, default: 1 (stdout)
  - >> = append, default: 1 (stdout)

3. redirection & | together: > on the first cmd will happen to the first command, > on the second command happens to the whole thing
  - for "<" this shouldn't really matter

- IMPORTANT:
  - Use the O_CLOEXEC flag in open so closing fds after exec is not needed

IMPLEMENTED FEATURES:

- "parse double quotes like this" (no support for ' ')
- ignore  arbitrary     spaces
  - so: 'ls>out'=='ls > out'=='ls >out'=='ls> out'=='ls >    out'
- redirect > and >> with file descriptor support
  - like: gcc 2>> out.log
- multiple conflicting > redirects will resort to the last one, but creating all of the files

Usage Notes:
- no support for "[cmd] [num]< [file]", only "[cmd] < [file]"
  - i.e. you can only redirect in from stdin
- no support for "... [num]>>&[num] ...", only "[num]>&[num]", like "2>&1"
  - i.e. redirecting out to a fd only works for ">", not ">>"
