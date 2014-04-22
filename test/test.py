import os
import sys
import re
import time
import shutil
import filecmp

def DeleteFile (filePath):
	if os.path.exists (filePath):
		os.remove (filePath)

def DeleteFolder (folderPath):
	if os.path.exists (folderPath):
		shutil.rmtree (folderPath)
	
def CreateFolder (folderPath):
	if not os.path.exists (folderPath):
		os.mkdir (folderPath)

def GetImageFileName (folder, configFile):
	return os.path.join (folder, configFile + '.png')
		
def RayTrace (binaryPath, configFile, resultFile):
	command = binaryPath + ' "' + configFile + '" "' + resultFile + '"'
	return os.system (command)

def EqualFile (aFile, bFile):
	return filecmp.cmp (aFile, bFile)

def Test (binaryPath, sourceFolder, resultFolder, referenceFolder, increaseResolution, configFile):
	def IncreaseResolution (originalConfigFile, tempConfigFile):
		file = open (originalConfigFile, 'rb')
		content = file.read ()
		file.close ()
		content = re.sub ('xresolution \d+', 'xresolution 1000', content)
		content = re.sub ('yresolution \d+', 'yresolution 1000', content)
		file = open (tempConfigFile, 'wb')
		file.write (content)
		file.close ()

	def WriteMessage (message, time):
		print message + ' (' + str (time) + ')'

	sys.stdout.write (configFile + '...')

	start = time.time ()
	rayTraceResult = 1
	if increaseResolution:
		tempFileName = 'temp.txt'
		IncreaseResolution (os.path.join (sourceFolder, configFile), tempFileName)
		rayTraceResult = RayTrace (binaryPath, tempFileName, GetImageFileName (resultFolder, configFile))
		DeleteFile (tempFileName)
	else:
		rayTraceResult = RayTrace (binaryPath, os.path.join (sourceFolder, configFile), GetImageFileName (resultFolder, configFile))
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

	increaseResolution = False
	if len (sys.argv) == 2:
		if sys.argv[1] == 'speed':
			increaseResolution = True	
	
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
	failCount += Test (binaryPath, sourceFolder, resultFolder, referenceFolder, increaseResolution, 'config01.txt')
	failCount += Test (binaryPath, sourceFolder, resultFolder, referenceFolder, increaseResolution, 'config02.txt')
	failCount += Test (binaryPath, sourceFolder, resultFolder, referenceFolder, increaseResolution, 'config03.txt')
	failCount += Test (binaryPath, sourceFolder, resultFolder, referenceFolder, increaseResolution, 'config04.txt')
	failCount += Test (binaryPath, sourceFolder, resultFolder, referenceFolder, increaseResolution, 'config05.txt')
	failCount += Test (binaryPath, sourceFolder, resultFolder, referenceFolder, increaseResolution, 'config06.txt')
	failCount += Test (binaryPath, sourceFolder, resultFolder, referenceFolder, increaseResolution, 'config07.txt')
	failCount += Test (binaryPath, sourceFolder, resultFolder, referenceFolder, increaseResolution, 'config08.txt')
	failCount += Test (binaryPath, sourceFolder, resultFolder, referenceFolder, increaseResolution, 'config09.txt')
	
	if failCount == 0:
		DeleteFolder (resultFolder)
		print 'all tests succeeded'
		return
	
	print 'one or more tests failed'
	return
	
Main ()
