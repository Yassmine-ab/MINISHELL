#!/usr/bin/env python3
"""Smoke tests for minishell, driven through a real PTY.

The shell intentionally refuses to run when stdin is not a TTY, so the tests
allocate a pseudo-terminal. No third-party dependency is required.

Usage:
    python3 tests/smoke_test.py [path/to/minishell]
or:
    MINISHELL_BIN=./minishell_bonus python3 tests/smoke_test.py
"""
import fcntl
import os
import re
import select
import struct
import subprocess
import sys
import tempfile
import termios
import time

BIN = os.environ.get("MINISHELL_BIN", os.path.join(os.getcwd(), "minishell"))
PROMPT_MARK = "⋙"
TIMEOUT = float(os.environ.get("MINISHELL_TIMEOUT", "10"))

ANSI_RE = re.compile(rb"\x1b\[[0-9;?]*[A-Za-z]")
CTRL_RE = re.compile(r"[\x00-\x08\x0b-\x1f\x7f]")
USER = os.environ.get("USER", "user")
# A prompt is printed by readline before every input line; everything on the
# rest of that line is the terminal echo of what was typed.
MAIN_PROMPT_RE = re.compile(
    re.escape(USER) + r" in [^\n]*?" + PROMPT_MARK + r"\s*")
HEREDOC_PROMPT_RE = re.compile(r"heredoc > ")


def _become_session_leader():
    """Give the shell its own session/controlling TTY so ^C sends SIGINT."""
    os.setsid()
    fcntl.ioctl(0, termios.TIOCSCTTY, 0)


class Shell:
    def __init__(self, cwd=None):
        self.master, slave = os.openpty()
        # Wide terminal so readline never wraps long prompts/commands.
        winsize = struct.pack("HHHH", 50, 200, 0, 0)
        fcntl.ioctl(self.master, termios.TIOCSWINSZ, winsize)
        self.proc = subprocess.Popen(
            [BIN],
            stdin=slave,
            stdout=slave,
            stderr=slave,
            cwd=cwd or os.getcwd(),
            env={**os.environ, "TERM": "dumb"},
            preexec_fn=_become_session_leader,
            close_fds=True,
        )
        os.close(slave)
        self.pending = b""
        if not self._wait_prompt():
            raise RuntimeError("shell did not display a prompt")

    def _drain(self, timeout):
        deadline = time.monotonic() + timeout
        while time.monotonic() < deadline:
            readable, _, _ = select.select([self.master], [], [], 0.05)
            if not readable:
                continue
            try:
                chunk = os.read(self.master, 65536)
            except OSError:
                return False
            if not chunk:
                return False
            self.pending += chunk
            deadline = time.monotonic() + 0.1
        return True

    def _wait_prompt(self, timeout=TIMEOUT):
        deadline = time.monotonic() + timeout
        while time.monotonic() < deadline:
            if PROMPT_MARK.encode() in self.pending:
                return True
            if not self._drain(0.1) and self.proc.poll() is not None:
                return False
        return False

    def run(self, lines, timeout=TIMEOUT):
        """Send lines (one shell command per element) and return clean output."""
        if isinstance(lines, str):
            lines = [lines]
        self.pending = b""
        os.write(self.master, "".join(line + "\n" for line in lines).encode())
        ok = self._wait_prompt(timeout)
        raw, self.pending = self.pending, b""
        if not ok and self.proc.poll() is None:
            raise RuntimeError(f"timeout waiting for prompt after {lines!r}")
        return self._clean(raw, lines)

    @staticmethod
    def _clean(raw, sent):
        text = ANSI_RE.sub(b"", raw).decode("utf-8", "replace")
        text = CTRL_RE.sub("", text.replace("\r\n", "\n").replace("\r", "\n"))
        text = MAIN_PROMPT_RE.sub("", text)
        text = HEREDOC_PROMPT_RE.sub("", text)
        echoed = {line.strip() for line in sent}
        lines = [line.rstrip() for line in text.split("\n")]
        return "\n".join(
            line for line in lines if line.strip() and line.strip() not in echoed)

    def send_ctrl(self, char, wait=0.4):
        os.write(self.master, char)
        time.sleep(wait)
        self._drain(0.2)

    def close(self):
        if self.proc.poll() is None:
            try:
                os.write(self.master, b"exit\n")
            except OSError:
                pass
            try:
                self.proc.wait(timeout=3)
            except subprocess.TimeoutExpired:
                self.proc.kill()
                self.proc.wait()
        os.close(self.master)
        return self.proc.returncode


class Case:
    def __init__(self, name, lines, expected):
        self.name = name
        self.lines = lines if isinstance(lines, list) else [lines]
        self.expected = expected


def build_cases(tmpdir):
    for name in ("glob_one.md", "glob_two.md", ".hidden_test"):
        open(os.path.join(tmpdir, name), "w").close()
    os.mkdir(os.path.join(tmpdir, "alpha_dir"))
    return [
        Case("echo", "echo hello world", "hello world"),
        Case("echo -n", "echo -n no_newline", "no_newline"),
        Case("single quotes", "echo 'a  b'", "a  b"),
        Case("double quotes", 'echo "a  b"', "a  b"),
        Case("external command", "printf '%s\\n' a b c", "a\nb\nc"),
        Case("simple pipe", "echo hi | cat", "hi"),
        Case("pipe chain", "echo abc | grep -o b", "b"),
        Case("pipe upper", "echo abc | tr a-z A-Z", "ABC"),
        Case("stdout redirect", ["echo out > f.txt", "cat f.txt"], "out"),
        Case("append redirect", ["echo one > a.txt", "echo two >> a.txt",
                                 "cat a.txt"], "one\ntwo"),
        Case("stdin redirect", ["echo piped > in.txt", "cat < in.txt"], "piped"),
        Case("failed stdout redirect",
             ["echo nope > /no/such/dir/f", "echo $?"],
             "/no/such/dir/f: Failed to open file for writing\n1"),
        Case("failed stdin redirect",
             ["cat < /no/such/file", "echo $?"],
             "/no/such/file: No such file or directory\n1"),
        Case("and list", "true && echo yes", "yes"),
        Case("or list", "false || echo fallback", "fallback"),
        Case("and-or precedence", "false && echo bad || echo good", "good"),
        Case("exit status 0", ["true", "echo $?"], "0"),
        Case("exit status 1", ["false", "echo $?"], "1"),
        Case("unknown command", ["definitely_not_a_command_xyz", "echo $?"],
             "definitely_not_a_command_xyz: Command not found\n127"),
        Case("pipeline status is the last command",
             ["false | true", "echo $?"], "0"),
        Case("pipeline status over three stages",
             ["false | true | false", "echo $?"], "1"),
        Case("pwd", "pwd", tmpdir),
        Case("cd + pwd", ["cd /tmp", "pwd"], "/tmp"),
        Case("cd relative", ["cd alpha_dir", "pwd"],
             os.path.join(tmpdir, "alpha_dir")),
        Case("home expansion", "echo $HOME", os.environ.get("HOME", "")),
        Case("export/unset", ["export MS_TEST=42", "echo $MS_TEST",
                              "unset MS_TEST", "echo $MS_TEST"], "42"),
        Case("unset missing variable is silent",
             ["unset MS_NEVER_SET", "echo $?"], "0"),
        Case("quotes prevent expansion", "echo '$HOME'", "$HOME"),
        Case("quote flags do not leak between words",
             ["export MS_QUOTED=hello", "echo 'x' $MS_QUOTED"], "x hello"),
        Case("wildcard", "echo *.md | tr ' ' '\\n' | sort",
             "glob_one.md\nglob_two.md"),
        Case("wildcard is sorted", "echo glob_*.md",
             "glob_one.md glob_two.md"),
        Case("wildcard matches become separate arguments",
             "printf '%s\\n' glob_*.md", "glob_one.md\nglob_two.md"),
        Case("wildcard without match stays literal", "echo missing_*.zzz",
             "missing_*.zzz"),
        Case("quoted wildcard is not expanded",
             ['echo "glob_*.md"', "echo 'glob_*.md'"],
             "glob_*.md\nglob_*.md"),
        Case("wildcard matches hidden files with a leading dot", "echo .*",
             ".hidden_test"),
        Case("wildcard with a slash is left literal", "echo alpha_dir/*.c",
             "alpha_dir/*.c"),
        Case("variable expansion happens before wildcards",
             ["export EXT=md", "echo glob_*.$EXT"],
             "glob_one.md glob_two.md"),
        Case("heredoc", ["cat << EOF | tr a-z A-Z", "heredoc body", "EOF"],
             "HEREDOC BODY"),
        Case("nested group", "(echo grouped)", "grouped"),
    ]


def signal_cases():
    """Signal handling requires a controlling terminal, hence a live shell."""
    results = []

    shell = Shell(cwd="/tmp")
    try:
        shell._drain(0.3)
        os.write(shell.master, b"sleep 30\n")
        time.sleep(0.4)
        shell.send_ctrl(b"\x03", wait=0.5)
        shell.pending = b""
        out = shell.run(["echo $?"], timeout=5)
    finally:
        code = shell.close()
    results.append(("ctrl-c during command", out, code, "130", 0))

    shell = Shell(cwd="/tmp")
    try:
        shell._drain(0.3)
        shell.send_ctrl(b"\x03", wait=0.5)
        shell.pending = b""
        out = shell.run(["echo $?"], timeout=5)
    finally:
        code = shell.close()
    results.append(("ctrl-c at prompt", out, code, "130", 0))

    shell = Shell(cwd="/tmp")
    try:
        shell._drain(0.3)
        os.write(shell.master, b"false\n")
        time.sleep(0.3)
        shell._drain(0.3)
        os.write(shell.master, b"\x04")
        shell.proc.wait(timeout=5)
        code = shell.proc.returncode
    except subprocess.TimeoutExpired:
        shell.proc.kill()
        code = shell.proc.returncode
    finally:
        os.close(shell.master)
    results.append(("ctrl-d exits with the last status", "", code, None, 1))
    return results


def main():
    global BIN
    if len(sys.argv) > 1:
        BIN = os.path.abspath(sys.argv[1])
    failures = 0
    with tempfile.TemporaryDirectory() as tmpdir:
        cases = build_cases(tmpdir)
        for case in cases:
            shell = Shell(cwd=tmpdir)
            try:
                out = shell.run(case.lines)
            except Exception as exc:  # noqa: BLE001
                out = f"<harness error: {exc}>"
            finally:
                code = shell.close()
            if out == case.expected and code == 0:
                print(f"PASS  {case.name}")
            else:
                failures += 1
                print(f"FAIL  {case.name}")
                print(f"      expected: {case.expected!r}")
                print(f"      got:      {out!r} (exit {code})")
    signals = signal_cases()
    for name, out, code, expected_out, expected_code in signals:
        if expected_out is None:
            ok = code == expected_code
        else:
            ok = out == expected_out and code == expected_code
        if ok:
            print(f"PASS  {name}")
        else:
            failures += 1
            print(f"FAIL  {name}")
            print(f"      expected: {expected_out!r} / exit {expected_code}")
            print(f"      got:      {out!r} (exit {code})")
    total = len(cases) + len(signals)
    print(f"\n{total - failures}/{total} cases passed")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
