
## commit.template

This is a commit template that makes it easier to stay within the recommended line length for the title and body of commit messages in any editor.

To use it, copy it somewhere like `~/.gitcommit` and then configure git:

    $ cp commit.template ~/.gitcommit
    $ git config --global commit.template ~/.gitcommit

Omit the `--global` parameter if you only want to use the template for a specific repository.

## pre-commit-add-files

This is a pre-commit hook that checks if there are any untracked files that are not covered by `.gitignore` and aborts the commit of any are found.

To use it, simply copy the file to `.git/hooks/pre-commit` in the repository you want to use it for.

If you ever don't want to add all untracked files right away, the script can be temporarily disabled by renaming `.git/hooks/pre-commit`.