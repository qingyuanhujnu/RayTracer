import os
import sys
import re
import time
import shutil
import filecmp

from lib import renderlib

def Render (binaryPath, configFile, resolution, algorithm, samples):
	configFileName = os.path.split (configFile)[1]
	print 'rendering: ' + configFileName + ' (' + algorithm + ')'
	tempConfigFile = 'temp.txt'
	renderlib.CopyFile (configFile, tempConfigFile)
	renderlib.ModifyResolution (tempConfigFile, resolution)
	
	tempResultFile = 'temp.png'
	renderlib.Render (binaryPath, tempConfigFile, tempResultFile, algorithm, samples, True)
	renderlib.DeleteFile (tempConfigFile)

	resultFileName = renderlib.GetFileNameFromParameters (configFileName, algorithm, samples, '')
	renderlib.DeleteFile (resultFileName)
	os.rename (tempResultFile, resultFileName)
	
def Main ():
	currentPath = os.path.dirname (os.path.abspath (__file__))
	os.chdir (currentPath)

	binaryPath = os.path.abspath ('../project/Release/CommandLine.exe')
	
	resolution = 400
	samples = 1
	for algorithm in ['pathtrace2']:
		Render (binaryPath, '../test/source/01_simple.txt', resolution, algorithm, samples)
		#Render (binaryPath, '../test/source/08_sphere_and_cylinder.txt', resolution, algorithm, samples)
		#Render (binaryPath, '../test/source/11_teapot.txt', resolution, algorithm, samples)
		#Render (binaryPath, '../test/source/17_depth_of_field.txt', resolution, algorithm, samples)
		#Render (binaryPath, '../test/source/18_various_shapes.txt', resolution, algorithm, samples)
		#Render (binaryPath, '../test/source/16_dragon.txt', resolution, algorithm, samples)
		#Render (binaryPath, '../test/source/12_bunny.txt', resolution, algorithm, samples)
	return
	
Main ()
