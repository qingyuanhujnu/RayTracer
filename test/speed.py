import os
import sys
import re
import time
import shutil
import filecmp

def DeleteFile (filePath):
	if os.path.exists (filePath):
		os.remove (filePath)

def RayTrace (binaryPath, configFile, resultFile):
	command = binaryPath + ' "' + configFile + '" "' + resultFile + '"'
	return os.system (command)

def MeasureSpeed (binaryPath, sourceFolder, configFile):
	def IncreaseResolution (originalConfigFile, tempConfigFile):
		file = open (originalConfigFile, 'rb')
		content = file.read ()
		file.close ()
		content = content.replace ('xresolution 200', 'xresolution 1000')
		content = content.replace ('yresolution 200', 'yresolution 1000')
		file = open (tempConfigFile, 'wb')
		file.write (content)
		file.close ()

	sys.stdout.write (configFile + '...')
	originalConfigFile = os.path.join (sourceFolder, configFile)
	tempConfigFile = 'temp.txt'
	resultFile = configFile + '.png'
	
	IncreaseResolution (originalConfigFile, tempConfigFile)
	
	start = time.time ()
	rayTraceResult = RayTrace (binaryPath, tempConfigFile, resultFile)
	end = time.time ()
	elapsedTime = end - start

	DeleteFile (tempConfigFile)
	
	print elapsedTime
	return elapsedTime
	
def Main ():
	currentPath = os.path.dirname (os.path.abspath (__file__))
	os.chdir (currentPath)

	sourceFolder = os.path.abspath ('source')

	binaryPath = os.path.abspath ('../project/Release/CommandLine.exe')
	if not os.path.exists (binaryPath):
		print 'no release binary found'
		return

	elapsedTime = 0;
	elapsedTime += MeasureSpeed (binaryPath, sourceFolder, 'config02.txt')
	elapsedTime += MeasureSpeed (binaryPath, sourceFolder, 'config03.txt')
	elapsedTime += MeasureSpeed (binaryPath, sourceFolder, 'config04.txt')
	elapsedTime += MeasureSpeed (binaryPath, sourceFolder, 'config05.txt')
	elapsedTime += MeasureSpeed (binaryPath, sourceFolder, 'config08.txt')
	print 'all time: ' + str (elapsedTime)
	return
	
Main ()
