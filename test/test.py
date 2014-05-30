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
		
def RayTrace (binaryPath, configFile, resultFile, algorithm, samples):
	command = binaryPath + ' --config "' + configFile + '" --result "' + resultFile + '" --algorithm ' + algorithm + ' --samples ' + str (samples)
	return os.system (command)

def EqualFile (aFile, bFile):
	return filecmp.cmp (aFile, bFile)

def Test (binaryPath, sourceFolder, resultFolder, referenceFolder, configFile, algorithm, samples, tabPosition):
	def WriteStartMessage (message, tabPosition):
		while len (message) < tabPosition:
			message += ' '
		sys.stdout.write (message)
		
	def WriteEndMessage (message, time):
		sys.stdout.write (message + ' (' + str (time) + ')\n')

	WriteStartMessage (configFile, tabPosition)

	start = time.time ()
	rayTraceResult = 1
	rayTraceResult = RayTrace (binaryPath, os.path.join (sourceFolder, configFile), GetImageFileName (resultFolder, configFile), algorithm, samples)
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

	testCases = [
		['01_simple.txt', 'raytrace', 9],
		['02_cylinder.txt', 'raytrace', 9],
		['03_sphere.txt', 'raytrace', 9],
		['04_simple_rotated.txt', 'raytrace', 9],
		['05_rotated_cylinder.txt', 'raytrace', 9],
		['06_rectangles.txt', 'raytrace', 9],
		['07_solids.txt', 'raytrace', 9],
		['08_sphere_and_cylinder.txt', 'raytrace', 9],
		['09_torus.txt', 'raytrace', 9],
		['10_facing.txt', 'raytrace', 9],
		['11_teapot.txt', 'raytrace', 9],
		['12_bunny.txt', 'raytrace', 9],
		['13_one_light.txt', 'raytrace', 9],
		['14_two_lights.txt', 'raytrace', 9],
		['15_refraction.txt', 'raytrace', 9],
		['16_dragon.txt', 'raytrace', 9],
		['17_depth_of_field.txt', 'raytrace', 9],
		['18_various_shapes.txt', 'raytrace', 9],
		['19_not_square.txt', 'raytrace', 9]
	]
	
	maxFileNameLength = 0
	for testCase in testCases:
		currentLength = len (testCase[0])
		if currentLength > maxFileNameLength:
			maxFileNameLength = currentLength
	
	success = True
	elapsedTime = 0
	for testCase in testCases:
		result = Test (binaryPath, sourceFolder, resultFolder, referenceFolder, testCase[0], testCase[1], testCase[2], maxFileNameLength + 1)
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
