import os
import sys
import re
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
	command = binaryPath + ' 200 200 1.0 "' + os.path.join (sourceFolder, configFile) + '" "' + GetImageFileName (resultFolder, configFile) + '"'
	return os.system (command)

def EqualFile (aFile, bFile):
	return filecmp.cmp (aFile, bFile)

def Test (binaryPath, sourceFolder, resultFolder, referenceFolder, configFile):
	sys.stdout.write (configFile + '...')
	if RayTrace (binaryPath, sourceFolder, resultFolder, configFile) != 0:
		print 'ray tracing failed'
		return 1
	referenceFile = GetImageFileName (referenceFolder, configFile)
	resultFile = GetImageFileName (resultFolder, configFile)
	if not os.path.exists (referenceFile):
		print 'no reference file'
		return 1
	if not os.path.exists (resultFile):
		print 'no result file'
		return 1
	if not EqualFile (referenceFile, resultFile):
		print 'failed'
		return 1
	print 'succeeded'
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
	
	binaryPath = os.path.abspath ('../project/Release/RayTracer.exe')
	if not os.path.exists (binaryPath):
		print 'no release binary found'
		return

	failCount = 0
	failCount += Test (binaryPath, sourceFolder, resultFolder, referenceFolder, 'config01.txt')
	failCount += Test (binaryPath, sourceFolder, resultFolder, referenceFolder, 'config02.txt')
	failCount += Test (binaryPath, sourceFolder, resultFolder, referenceFolder, 'config03.txt')
	
	if failCount == 0:
		DeleteFolder (resultFolder)
		print 'all tests succeeded'
		return
	
	print 'one or more tests failed'
	return
	
Main ()
