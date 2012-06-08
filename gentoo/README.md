
## update

Catch-all update script for the paranoid gentoo user:

1. Sync main tree
2. Sync layman overlays (if installed)
3. Update eix cache (if installed)
4. Update portage
5. Update config files
6. Update @world
7. Update preserved packages
8. Update config files
9. Clean unused dependencies
10. Fix .la files (if lafilefixer is installed)
11. Run revdep-rebuild (if installed)
12. Run prelink (if installed)
13. Moo
