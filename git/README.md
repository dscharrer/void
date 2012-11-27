
## commit.template

This is a commit template that makes it easier to stay within the recommended line length for the title and body of commit messages in any editor.

To use it, copy it somewhere like `~/.gitcommit` and then configure git:

    $ cp commit.template ~/.gitcommit
    $ git config --global commit.template ~/.gitcommit

Omit the `--global` parameter if you only want to use the template for a specific repository.

## commit.nanorc

git commit message syntax highlighting for nano based on http://milianw.de/code-snippets/git-commit-message-highlighting-in-nano

To use it, copy it somewhere like `~/.gitcommit.nanorc` and then configure it in `~/.nanorc`:

    include "~/.gitcommit.nanorc"

## pre-commit-add-files

This is a pre-commit hook that checks if there are any untracked files that are not covered by `.gitignore` and aborts the commit of any are found.

To use it, simply copy (or link) the file to `.git/hooks/pre-commit` in the repository you want to use it for.

If you ever don't want to add all untracked files right away, you can pass the `--no-verify` option to git. (or temporarily disable the script by renaming `.git/hooks/pre-commit`)
