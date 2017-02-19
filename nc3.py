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

def add_variable(var, val, labelcounter, colnumber):
	Tkinter.Label(top, text=var, justify=Tkinter.LEFT).grid(row=labelcounter, column=colnumber)
	labels[var] = Tkinter.StringVar()
	labels[var].set(val)
	Tkinter.Label(top, textvariable=labels[var], justify=Tkinter.RIGHT).grid(row=labelcounter, column=colnumber + 1)
	labelcounter += 1

class Token(object):
	def __init__(self,t,l):
		self.text = t
		self.level = l

colnumber = 0
labelcounter = 0
for l in console():
	l=l.strip()
	print(l)
	level=0
	tokens=[Token("",level)]
	i=0
	last_ci=-1;
	open_seperators=["X"]
	for ci in range(len(l)):
		if l[ci]==':' or l[ci]=='[' or l[ci]=='(': print(l[ci] + str(ci) + " | " + str(last_ci) + " | " + str(open_seperators))
		if l[ci]=='[' or (l[ci]==':' and not open_seperators[-1]==':') or (l[ci]=='(' and not (open_seperators[-1]==':' and not (ci - last_ci > 1))):
			# print(l[ci] + " | " + str(ci) + " |  " + str(last_ci))
			level+=1
			i+=1
			last_ci=ci
			open_seperators.append(l[ci])
			tokens.append(Token("",level))
		elif l[ci]==']' or l[ci]==')' or (l[ci]==' ' and open_seperators[-1]==':' and (ci - last_ci > 1)):
			# print(l[ci] + " | " + str(ci) + " |  " + str(last_ci))
			level-=1
			i+=1
			last_ci=ci
			del open_seperators[-1]
			tokens.append(Token("",level))
		else:
			tokens[i].text+=l[ci]
	for token in tokens:
		token.text=token.text.strip()
		print(token.text + ", " + str(token.level))
	"""
	for i in range(len(values)):
		if names[i] not in labels:
			add_variable(names[i], values[i], labelcounter, colnumber)
			labelcounter+=1
			if labelcounter > 40:
				labelcounter = 0
				colnumber += 2
		elif len(values) % 2 == 0:
			labels[names[i]].set(values[i])
		top.update()
	"""
