#!/usr/bin/env python

from __future__ import print_function
import subprocess,os,Tkinter,re

def console():
	cmd = subprocess.Popen("ssh admin@roborio-1425-frc.local \"tail -f -n0 /var/local/natinst/log/FRC_UserProgram.log\"", shell=True, stdout=subprocess.PIPE)
	for line in iter(cmd.stdout.readline, ""):
		yield line
	cmd.stdout.close()

"""
for l in console():
	if l.find("CLEAR_SCREEN")==0:
		os.system("clear")
	else:
		print(l,end="")
"""

top=Tkinter.Tk()

"""
def add_variable(var, labelcounter, colnumber):
	Tkinter.Label(top, text=var, justify=Tkinter.LEFT).grid(row=labelcounter, column=colnumber)
	labels[var] = Tkinter.StringVar()
	labels[var].set(val)
	Tkinter.Label(top, textvariable=labels[var], justify=Tkinter.RIGHT).grid(row=labelcounter, column=colnumber + 1)
	labelcounter += 1

class Token(object):
	def __init__(self,t,l):
		self.text = t
		self.level = l
"""


beginning_chars = 5;
lines = []
labeltexts = []
labels = []
i = 0

"""
def reset_lines():
	for label in labels:
		label.destroy();
	lines = []
	labeltexts = []
	labels = []
"""

# reset_lines()

for l in console():
	l = l.strip()
	if len(l) >= beginning_chars + 1:
		if lines.count(l[0:beginning_chars]) == 0:
			lines.append(l[0:beginning_chars])
			labeltexts.append(Tkinter.StringVar())
			labeltexts[i].set(l)
			labels.append(Tkinter.Label(top, textvariable=labeltexts[i], justify=Tkinter.LEFT, wraplength=1000).grid(row=i, column=0))
			i += 1
		else:
			labeltexts[lines.index(l[0:beginning_chars])].set(l)
		top.update()
