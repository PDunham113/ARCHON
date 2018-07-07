# ARCHON
**A**ddressable **R**GB LED **CON**troller *(The **'h'** is nonexistent)*


The ultimate goal is to control an arbitrary number of addressable (WS2812 or
similar) from an ATMega328p, with a robust serial protocol to communicate to a
IoT-friendly mainboard (Raspberry Pi Zero W or similar).

Current state is somewhat less than that (setting up tools, etc.)

## Folders

### hardware (v0.0.0)
Contains EAGLE files for the ARCHON controller board

### software (v0.0.0)
Contains firmware for ARCHON

## Setup
Working with the hardware requires an installation of Autodesk EAGLE (v8.5.2 or
later), as well as a copy of Sparkfun's EAGLE library. Installation is left up
to the user.

Working with the software requires a number of (non-Windows-friendly) tools. To
automate the installation process and facilitate the ability to work on
multiple projects at once, a Vagrantfile is maintained with all the required
tools and dependencies.

### Installing Vagrant
You will need:
-   Vagrant [(HashiCorp)][vagrant_install]
-   VirtualBox [(Oracle)][virtualbox_install]
    -   Other VMs are available, but not actively supported by this project
-   VirtualBox Extensions [(Same place)][virtualbox_install]

The Vagrantfile used here is split into 3 parts.
-   `Vagrantfile`
    -   Deals with general configuration of VM instance
-   `Vagrantfile.project`
    -   Deals with project-specific tools and dependencies
-   `Vagrantfile.local`
    -   Deals with user preferences and machine-specific settings. As this is
        user-specific, Git will ignore this. A template
        (`Vagrantfile.local.example`) is included. Rename and enjoy!

To boot the VM, navigate to the project folder and run the following commands:
```
vagrant up
vagrant ssh
```

#### Common Errors
-   Ensure that virtualization extensions are enabled (you may need to check
    your BIOS)


[vagrant_install]: https://www.vagrantup.com/downloads.html
[virtualbox_install]: https://www.virtualbox.org/wiki/Downloads
