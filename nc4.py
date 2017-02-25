#!/usr/bin/env python

from __future__ import print_function
import subprocess,os,Tkinter,re

def console():
	cmd = subprocess.Popen("ssh admin@roborio-1425-frc.local \"tail -f -n0 ../lvuser/FRC_UserProgram.log\"", shell=True, stdout=subprocess.PIPE)
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

labels = {}

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

lines = []
labels = []
i = 0
for l in console():
	l = l.strip()
	if len(l) >= 4:
		if lines.count(l[0:3]) == 0:
			lines.append(l[0:3])
			labels.append(Tkinter.StringVar())
			labels[i].set(l)
			Tkinter.Label(top, textvariable=labels[i], justify=Tkinter.RIGHT, wraplength=1000).grid(row=i, column=0)
			i += 1
		else:
			labels[lines.index(l[0:3])].set(l)
		top.update()
