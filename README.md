# dvswitch
# fork of commit 1d73d4a which compiles and runs on ubuntu 16.04 i686/amd64(tested)
# reason 2015 mailinglist activity shows the dvswitch project is dead and removed from current ubuntu/debian repositories!
# attempt to continue/add missing "half-assed" features to this beautifull robust software package that honours field interlacing!

#TODO
*add compile instructions for a ubuntu 16.04 mini.iso based install with tasksel "Ubuntu MATE minimal installation + openssh-server + other system defaults tasksel packages (don't know name anymore)" 

*create list of usefull/stable patches and commit them accordingly by git(hub) rules:
  https://github.com/yoe/dvswitch/commit/79b63feef6f7a27c2f962d5a7efffa4f9d657f57       #fix memleak
  https://github.com/jnweiger/dvswitch/commit/b0eff96d68fafaa3704426c5d4ff3c0391b921ef  #icon for lost sources
  https://github.com/jnweiger/dvswitch/commit/82355bfd0861dd4553899a460b8b415ec86b2e3b  #mouse clickable preview thumbs
  https://github.com/jnweiger/dvswitch/commit/0d6e549b26f81096038f703000316605f51e5b67  #title safe area activated by flag
  https://github.com/jnweiger/dvswitch/commit/95838c05470adf363fee02af7fb791df075255f1  #title safe default off
  http://lists.alioth.debian.org/pipermail/dvswitch-devel/2011-August/000480.html       #save independent source streams
  myself dif.c                                                                          #show full D1 720 pixels vs 702/704 "active"
  myself ?                                                                              #adjustable titlesafe 16/9 crop from 4/3
  myself ?                                                                              #limit warning/error message flood console
  
*little firewire debug hinst "kernel modules / lspci / dmesg / IRQ interupts sharing / etc"

*merge/patch dvpause
  https://github.com/Jeija/dvpause
