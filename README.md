# minishell

[![CI](https://github.com/imyassmine/minishell/actions/workflows/ci.yml/badge.svg)](https://github.com/imyassmine/minishell/actions/workflows/ci.yml)
![Language](https://img.shields.io/badge/language-C-blue)
![License](https://img.shields.io/badge/license-GPL--3.0-green)

A small UNIX shell written from scratch in C: its own lexer, recursive-descent
parser, AST, executor, environment handling, signal management and a tiny
garbage collector. It reproduces a practical subset of `bash` — the kind of
shell you actually use day to day (pipes, redirections, heredocs, quotes,
variables, `&&` / `||`, grouped commands and wildcards).

This is a [42 School](https://42.fr) project, built by **yaabdall**
([@imyassmine](https://github.com/imyassmine)) & **besch**.

```console
$ make
$ ./minishell
yassabda in /home/yassabda/minishell ⋙ echo "Hello, 42!"
Hello, 42!
yassabda in /home/yassabda/minishell ⋙ cat << EOF | tr a-z A-Z
heredoc > minishell is alive
heredoc > EOF
MINISHELL IS ALIVE
yassabda in /home/yassabda/minishell ⋙ false && echo nope || echo fallback
fallback
yassabda in /home/yassabda/minishell ⋙ echo $?
0
yassabda in /home/yassabda/minishell ⋙ exit
```

---

## Table of contents

- [Features](#features)
- [Supported syntax](#supported-syntax)
- [Builtins](#builtins)
- [Build & run](#build--run)
- [Usage examples](#usage-examples)
- [Tests & CI](#tests--ci)
- [Project structure](#project-structure)
- [Design overview](#design-overview)
- [Scope & limitations](#scope--limitations)
- [License](#license)

---

## Features

**Mandatory part**

- Interactive prompt built on GNU Readline, with working command history.
- Lexer handling whitespace, quotes, operators and special characters.
- Recursive-descent parser producing an AST, then executed node by node.
- Commands resolved through `PATH`, plus relative and absolute paths.
- Single and double quotes, with expansions in the right places.
- Environment variables (`$VAR`) and the last exit status (`$?`).
- Pipelines with any number of commands (`cmd1 | cmd2 | cmd3`).
- Redirections: `<`, `>`, `>>` and heredocs `<<` (with or without expansion).
- The seven mandatory builtins: `cd`, `echo`, `env`, `exit`, `export`, `pwd`,
  `unset`.
- Signal handling: `Ctrl-C` interrupts a running command and gives
  `$? = 130`, `Ctrl-D` leaves the shell, `Ctrl-\` is ignored at the prompt.
- Exit statuses following shell conventions (`1`, `2`, `126`, `127`, `130`...).

**Bonus part**

- `&&` and `||` lists with short-circuit evaluation.
- Command grouping with parentheses `( ... )`.
- Wildcard expansion in the current directory, sorted and quote-aware,
  using `fnmatch(3)`.

**Engineering highlights**

- Automatic memory management: every allocation is registered in a custom
  **garbage collector** (`lib/garbage_collector/`) that releases a whole
  command context in one call instead of tracking each pointer by hand.
- No global state except one `volatile sig_atomic_t` signal flag.
- Shell-conformant statuses: a pipeline returns the status of its last
  command, a failed redirection aborts the command with status `1`, and
  `Ctrl-D` exits with the last exit status.
- Compiles cleanly with `-Wall -Wextra -Werror`.

---

## Supported syntax

| Syntax | Example | Supported |
| --- | --- | :---: |
| Simple command | `ls -la` | ✅ |
| `PATH` resolution | `grep -r foo .` | ✅ |
| Pipes | `cat file \| wc -l` | ✅ |
| Standard redirections | `sort < in > out` | ✅ |
| Append redirection | `echo line >> file` | ✅ |
| Heredoc | `cat << EOF` | ✅ |
| Quoted heredoc | `cat << 'EOF'` | ✅ |
| Variables | `echo $HOME` | ✅ |
| Exit status | `echo $?` | ✅ |
| Single / double quotes | `echo 'a b' "c d"` | ✅ |
| `&&` and `\|\|` | `make && ./minishell` | ✅ |
| Grouping | `(cd /tmp && pwd)` | ✅ |
| Wildcards (current directory) | `echo *.c` | ✅ |
| `;`, `$(( ))`, `${VAR:-x}`, `~`, `cd -` | — | 🚫 |

---

## Builtins

| Builtin | Behaviour |
| --- | --- |
| `cd` | Change directory (absolute, relative, or `$HOME` with no argument). |
| `echo` | Print its arguments, supports `-n`. |
| `env` | Print the environment. |
| `export` | Add or update variables; without argument prints the environment (sorted, quoted). |
| `pwd` | Print the current working directory. |
| `unset` | Remove variables from the environment. |
| `exit` | Exit the shell with the given status (validates numeric arguments). |

---

## Build & run

### Dependencies

- A C compiler (`gcc` or `clang`) and `make`
- GNU Readline **development headers**
- `python3` only if you want to run the automated tests

```console
# Debian / Ubuntu (and WSL)
$ sudo apt install build-essential libreadline-dev python3

# Fedora / RHEL
$ sudo dnf install gcc make readline-devel python3

# Arch
$ sudo pacman -S gcc make readline python
```

> On macOS, install a real readline with `brew install readline`; the system
> `libedit` is not a drop-in replacement. Linux (or WSL) is recommended.

### Commands

```console
$ make          # build ./minishell
$ make bonus    # build ./minishell_bonus
$ make re       # full rebuild
$ make clean    # remove object files
$ make fclean   # remove object files, library and binaries
$ make debug    # rebuild with -g3 and AddressSanitizer
$ make test     # build and run the automated test suite
$ make help     # list every target
```

The shell is **interactive by design**: it refuses to read commands from a
pipe/non-interactive stdin (`isatty(STDIN_FILENO)`) and exits with status `1`
in that case. Run it from a real terminal, or from the PTY-driven test suite.

---

## Usage examples

```console
yassabda in /tmp ⋙ pwd
/tmp
yassabda in /tmp ⋙ cd /home/yassabda/minishell && pwd
/home/yassabda/minishell
yassabda in /home/yassabda/minishell ⋙ export GREETING="hello world"
yassabda in /home/yassabda/minishell ⋙ echo "$GREETING"
hello world
yassabda in /home/yassabda/minishell ⋙ echo '$GREETING'      # no expansion
$GREETING
yassabda in /home/yassabda/minishell ⋙ wc -l < srcs/main.c
110
yassabda in /home/yassabda/minishell ⋙ false && echo nope || echo "status $?"
status 1
yassabda in /home/yassabda/minishell ⋙ cd srcs && echo *.c | wc -w
3
```

---

## Tests & CI

The repository ships with a small, dependency-free test suite that drives the
real binary through a pseudo-terminal:

```console
$ make test          # mandatory binary
$ make test_bonus    # bonus binary
$ make test_all      # both
```

It covers builtins, quoting, variable expansion, pipes and pipeline statuses,
redirections (including failure cases), heredoc, logic operators, exit statuses,
signals (`Ctrl-C`, `Ctrl-D`), wildcards and command grouping. Tests must use a
PTY because the shell intentionally rejects non-interactive input.

Memory checking is available through valgrind (install it first), with a
suppression file for the internal allocations of Readline:

```console
$ ./val.sh            # valgrind --leak-check=full, using .readline.supp
$ make debug          # or build with AddressSanitizer and run `make test`
```

The GitHub Actions workflow
([`.github/workflows/ci.yml`](.github/workflows/ci.yml)) runs on every push and
pull request:

1. **build** — compiles the mandatory and bonus binaries, then checks that
   `make fclean` leaves a clean tree;
2. **tests** — runs the PTY test suite against `./minishell` and
   `./minishell_bonus`;
3. **sanitizers** — rebuilds with AddressSanitizer and runs the suite again.

---

## Project structure

```text
.
├── .github/workflows/ci.yml   # GitHub Actions: build matrix, tests, ASan
├── includes/
│   └── minishell.h            # tokens, AST nodes, shell state, prototypes
├── lib/                       # home-made static library
│   ├── garbage_collector/     # automatic memory management
│   ├── includes/              # gc.h, get_next_line_bonus.h
│   ├── libft.h                # libc helpers + linked lists
│   ├── get_next_line.c        # line reader used by heredocs/continuations
│   ├── ft_printf.c            # formatted output helpers
│   ├── ft_*.c                 # string, memory and list utilities
│   └── Makefile               # builds lib.a
├── srcs/
│   ├── builtins/              # cd, echo, env, exit, export, pwd, unset
│   ├── environment/           # envp copy, lookup, update, add
│   ├── executor/              # execution, pipelines, redirections, expander
│   ├── initialization/        # shell state + prompt
│   ├── lexer/                 # tokenizer: words, quotes, operators, parens
│   ├── parser/                # recursive-descent parser -> AST
│   ├── signal/                # interactive / exec / heredoc signal modes
│   ├── main.c                 # REPL loop
│   └── utils.c                # errors, fd helpers, GC locking
├── tests/smoke_test.py        # PTY-driven functional tests
├── .readline.supp             # valgrind suppressions for Readline
├── val.sh                     # valgrind helper script
├── Makefile
└── LICENSE                    # GPL-3.0
```

---

## Design overview

1. **Lexer** (`srcs/lexer/`) walks the line character by character and emits a
   `t_token` array: words, operators (`|`, `&&`, `||`, parentheses) and
   redirections are typed. Quotes are handled while building words so their
   content can be expanded later, or not.
2. **Parser** (`srcs/parser/`) turns the token stream into an AST of
   `t_node`s with a recursive-descent grammar
   (`expression -> and/or -> command -> redirections`). Grouping and operator
   precedence fall out of the grammar.
3. **Expander & executor** (`srcs/executor/`) expand `$VAR`, `$?` and `*` at
   the right time, then walk the AST: commands are forked and `execve`d,
   builtins run in the shell (or in a child when part of a pipeline), and file
   descriptors are saved/restored around every redirection.
4. **Signals** (`srcs/signal/`) use dedicated modes for the interactive
   prompt, command execution and heredoc reading, so `Ctrl-C` behaves
   correctly in each context.
5. **Memory** (`lib/garbage_collector/`) every allocation is registered with
   its `t_gc`; command-scoped allocations are released in one call at the end
   of each loop iteration, and the environment itself is "locked" so it
   survives.

---

## Scope & limitations

This is an educational shell, not a `bash` clone. Outside of the implemented
scope: `;` separators, parameter expansion forms such as `${VAR:-default}`,
arithmetic `$((...))`, `~` expansion, `cd -`, aliases, job control (`&`,
`Ctrl-Z`, `jobs`, `fg`) and non-interactive/script mode. It is meant to be used
interactively in a POSIX terminal.

Wildcards expand in the **current directory only**: a pattern containing a `/`
is left untouched, which is the scope of the 42 bonus subject.

---

## License

Distributed under the **GNU General Public License v3.0**. See
[`LICENSE`](LICENSE) for the full text.

---

## Authors

- **yaabdall** — shell core: lexer, parser, executor, expansion, environment,
  prompt and the garbage collector
- **besch** — builtins, redirections, heredocs, signal handling and execution
  errors

Both authors worked together on the whole project; the 42 header of each source
file lists its main author.

42 Paris — project `minishell`, 2024.
