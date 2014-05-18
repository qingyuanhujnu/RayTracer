import os
import sys
import re
import time
import shutil
import filecmp

def DeleteFile (filePath):
	if os.path.exists (filePath):
		os.remove (filePath)

def RayTrace (binaryPath, configFile, resultFile, algorithm, samples):
	command = binaryPath + ' --config "' + configFile + '" --result "' + resultFile + '" --algorithm ' + algorithm + ' --samples ' + str (samples) + ' --verbose'
	print command
	return os.system (command)

def Render (binaryPath, configFile, resolution, algorithm, samples):
	def ModifyResolution (originalConfigFile, tempConfigFile, resolution):
		file = open (originalConfigFile, 'rb')
		content = file.read ()
		file.close ()
		content = re.sub ('xresolution \d+', 'xresolution ' + str (resolution), content)
		content = re.sub ('yresolution \d+', 'yresolution ' + str (resolution), content)
		file = open (tempConfigFile, 'wb')
		file.write (content)
		file.close ()	

	configFileName = os.path.split (configFile)[1]
	print 'rendering: ' + configFileName + ' (' + algorithm + ')'
	tempConfigFile = 'temp.txt'
	ModifyResolution (configFile, tempConfigFile, resolution)
	
	tempResultFile = 'temp.png'
	RayTrace (binaryPath, tempConfigFile, tempResultFile, algorithm, samples)
	DeleteFile (tempConfigFile)

	resultFileName = configFileName + '_algorithm-' + algorithm + '_samples-' + str (samples) + '.png'
	DeleteFile (resultFileName)
	os.rename (tempResultFile, resultFileName)
	
def Main ():
	currentPath = os.path.dirname (os.path.abspath (__file__))
	os.chdir (currentPath)

	binaryPath = os.path.abspath ('../project/Release/CommandLine.exe')
	
	resolution = 400
	samples = 256
	for algorithm in ['raytrace', 'pathtrace2']:
		Render (binaryPath, '../test/source/01_simple.txt', resolution, algorithm, samples)
		Render (binaryPath, '../test/source/08_sphere_and_cylinder.txt', resolution, algorithm, samples)
		Render (binaryPath, '../test/source/11_teapot.txt', resolution, algorithm, samples)
		Render (binaryPath, '../test/source/16_dragon.txt', resolution, algorithm, samples)
	return
	
Main ()
