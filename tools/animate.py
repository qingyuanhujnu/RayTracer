import os
import sys
import re
import time
import shutil
import filecmp

def DeleteFile (filePath):
	if os.path.exists (filePath):
		os.remove (filePath)

def CreateFolder (folderPath):
	if not os.path.exists (folderPath):
		os.mkdir (folderPath)		
		
def DeleteFolder (folderPath):
	if os.path.exists (folderPath):
		shutil.rmtree (folderPath)

def GetImageFileName (folder, configFile):
	return os.path.join (folder, configFile + '.png')	
	
def RayTrace (binaryPath, configPath, resultPath):
	command = binaryPath + ' "' + configPath + '" "' + resultPath + '"'
	return os.system (command)	

def WriteTempConfigFile (configPath, tempConfigPath, frame):
	file = open (configPath, 'rb')
	content = file.read ()
	file.close ()
	macros = re.findall (r'\{\{[a-z0-9\+\-\.]*\}\}', content)
	for macro in macros:
		macroContent = macro
		macroContent = macroContent[2:]
		macroContent = macroContent[:len (macroContent) - 2]
		splitted = macroContent.split ('+')
		val = splitted[0]
		inc = splitted[1]
		newVal = float (val) + float (frame) * float (inc)
		content = content.replace (macro, str (newVal))
	file = open (tempConfigPath, 'wb')
	file.write (content)
	file.close ()
	
def Main ():
	currentPath = os.path.dirname (os.path.abspath (__file__))
	os.chdir (currentPath)

	binaryPath = os.path.abspath ('../project/Release/CommandLine.exe')
	if not os.path.exists (binaryPath):
		print 'no release binary found'
		return
	
	resultFolder = os.path.abspath ('video')
	DeleteFolder (resultFolder)
	CreateFolder (resultFolder)
	
	configPath = os.path.abspath ('animconfig.txt')
	tempConfigPath = os.path.abspath ('temp.txt')
	for i in range (0, 100):
		print 'create frame image'
		WriteTempConfigFile (configPath, tempConfigPath, i)
		indexStr = str (i)
		while len (indexStr) < 5:
			indexStr = '0' + indexStr
		RayTrace (binaryPath, tempConfigPath, os.path.join (resultFolder, 'config' + indexStr + '.png'))
		DeleteFile (tempConfigPath)
	
	return
	
Main ()
