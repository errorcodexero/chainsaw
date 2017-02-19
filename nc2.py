#!/usr/bin/env python

from __future__ import print_function
import subprocess,os

def console():
	cmd = subprocess.Popen("ssh admin@roborio-1425-frc.local \"tail -f -n0 ../lvuser/FRC_UserProgram.log\"", shell=True, stdout=subprocess.PIPE)
	for line in iter(cmd.stdout.readline, ""):
		yield line
	cmd.stdout.close()

for l in console():
	if l.find("CLEAR_SCREEN")==0:
		os.system("clear")
	else:
		print(l,end="")
