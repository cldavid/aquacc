aquacc
======

Aquarium Control Center

Web interface for my DIY dosing unit (ds_motor) and GEMBIRD SIS-PM power distribution unit

- DIY Dosing unit graphical interface.
- Interface for Gembird SIS-PM PDU. 
- DS18b20 one wire temperature logging.
- RRD Temperature graphs.

David Cluytens <david.cluytens@gmail.com>

Install
=======

Before compiling, configure the proper compiler in the Makefile.
Set CC = YOUR-COMPILER

Save and quit the Makefile

Type make to compile.
Type make install to install the aquacc binary and scripts.
The following files are installed.
.* /usr/local/bin/aquacc
.* /usr/local/sbin/temp_client.py
.* /etc/init.d/aquacc

