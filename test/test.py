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

def Test (binaryPath, sourceFolder, resultFolder, referenceFolder, increaseResolution, configFile, tabPosition):
	def IncreaseResolution (originalConfigFile, tempConfigFile):
		file = open (originalConfigFile, 'rb')
		content = file.read ()
		file.close ()
		content = re.sub ('xresolution \d+', 'xresolution 1000', content)
		content = re.sub ('yresolution \d+', 'yresolution 1000', content)
		file = open (tempConfigFile, 'wb')
		file.write (content)
		file.close ()

	def WriteStartMessage (message, tabPosition):
		while len (message) < tabPosition:
			message += ' '
		sys.stdout.write (message)
		
	def WriteEndMessage (message, time):
		sys.stdout.write (message + ' (' + str (time) + ')\n')

	WriteStartMessage (configFile, tabPosition)

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
	
	result = {
		'success' : False,
		'time' : elapsedTime
	}
	
	if rayTraceResult != 0:
		WriteEndMessage ('ray tracing failed', elapsedTime)
		return result

	referenceFile = GetImageFileName (referenceFolder, configFile)
	resultFile = GetImageFileName (resultFolder, configFile)
	if not os.path.exists (referenceFile):
		WriteEndMessage ('no reference file', elapsedTime)
		return result
	if not os.path.exists (resultFile):
		WriteEndMessage ('no result file', elapsedTime)
		return result
	if not EqualFile (referenceFile, resultFile):
		WriteEndMessage ('failed', elapsedTime)
		return result
	WriteEndMessage ('succeeded', elapsedTime)
	result['success'] = True
	return result
	
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

	fileNames = os.listdir (sourceFolder)
	maxFileNameLength = 0
	for fileName in fileNames:
		currentLength = len (fileName)
		if currentLength > maxFileNameLength:
			maxFileNameLength = currentLength
	
	success = True
	elapsedTime = 0
	for fileName in fileNames:
		result = Test (binaryPath, sourceFolder, resultFolder, referenceFolder, increaseResolution, fileName, maxFileNameLength + 1)
		if not result['success']:
			success = False
		elapsedTime += result['time']
	
	print ''
	print 'full time: ' + str (elapsedTime)
	if success:
		DeleteFolder (resultFolder)
		print 'all tests succeeded'
		return
	
	print 'one or more tests failed'
	return
	
Main ()
