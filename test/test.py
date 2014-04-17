import os
import sys
import re
import time
import shutil
import filecmp

def DeleteFolder (folderPath):
	if os.path.exists (folderPath):
		shutil.rmtree (folderPath)
	
def CreateFolder (folderPath):
	if not os.path.exists (folderPath):
		os.mkdir (folderPath)

def GetImageFileName (folder, configFile):
	return os.path.join (folder, configFile + '.png')
		
def RayTrace (binaryPath, sourceFolder, resultFolder, configFile):
	command = binaryPath + ' "' + os.path.join (sourceFolder, configFile) + '" "' + GetImageFileName (resultFolder, configFile) + '"'
	return os.system (command)

def EqualFile (aFile, bFile):
	return filecmp.cmp (aFile, bFile)

def Test (binaryPath, sourceFolder, resultFolder, referenceFolder, configFile):
	def WriteMessage (message, time):
		print message + ' (' + str (time) + ')'

	sys.stdout.write (configFile + '...')

	start = time.time ()
	rayTraceResult = RayTrace (binaryPath, sourceFolder, resultFolder, configFile)
	end = time.time ()
	elapsedTime = end - start

	if rayTraceResult != 0:
		WriteMessage ('ray tracing failed', elapsedTime)
		return 1

	referenceFile = GetImageFileName (referenceFolder, configFile)
	resultFile = GetImageFileName (resultFolder, configFile)
	if not os.path.exists (referenceFile):
		WriteMessage ('no reference file', elapsedTime)
		return 1
	if not os.path.exists (resultFile):
		WriteMessage ('no result file', elapsedTime)
		return 1
	if not EqualFile (referenceFile, resultFile):
		WriteMessage ('failed', elapsedTime)
		return 1
	WriteMessage ('succeeded', elapsedTime)
	return 0
	
def Main ():
	currentPath = os.path.dirname (os.path.abspath (__file__))
	os.chdir (currentPath)

	sourceFolder = os.path.abspath ('source')
	referenceFolder = os.path.abspath ('reference')
	resultFolder = os.path.abspath ('result')
	CreateFolder (referenceFolder)
	DeleteFolder (resultFolder)
	CreateFolder (resultFolder)
	
	binaryPath = os.path.abspath ('../project/Release/CommandLine.exe')
	if not os.path.exists (binaryPath):
		print 'no release binary found'
		return

	failCount = 0
	failCount += Test (binaryPath, sourceFolder, resultFolder, referenceFolder, 'config01.txt')
	failCount += Test (binaryPath, sourceFolder, resultFolder, referenceFolder, 'config02.txt')
	failCount += Test (binaryPath, sourceFolder, resultFolder, referenceFolder, 'config03.txt')
	failCount += Test (binaryPath, sourceFolder, resultFolder, referenceFolder, 'config04.txt')
	failCount += Test (binaryPath, sourceFolder, resultFolder, referenceFolder, 'config05.txt')
	
	if failCount == 0:
		DeleteFolder (resultFolder)
		print 'all tests succeeded'
		return
	
	print 'one or more tests failed'
	return
	
Main ()
