import os
import sys
import re
import time
import shutil
import filecmp

# easy_install pil

from lib import renderlib
import Image

def Render (binaryPath, configFile, resolution, algorithm, samples, cameraOffset, rotation = None, postFix = ''):
	configFileName = os.path.split (configFile)[1]

	tempPath = 'temp.txt'
	renderlib.CopyFile (configFile, tempPath)
	if resolution != 0:
		renderlib.ModifyResolution (tempPath, resolution)
	if rotation != None:
		renderlib.ModifyRotation (tempPath, rotation)

	camera = renderlib.GetCameraParameters (configFile)
	renderlib.OffsetEyePosition (camera, -cameraOffset / 2.0)
	renderlib.ModifyCameraParameters (tempPath, camera)
	fileName1 = renderlib.GetFileNameFromParameters (configFileName, algorithm, samples, 'stereo1' + postFix)
	renderlib.Render (binaryPath, tempPath, fileName1, algorithm, samples, True)
	
	camera = renderlib.GetCameraParameters (configFile)
	renderlib.OffsetEyePosition (camera, cameraOffset / 2.0)
	renderlib.ModifyCameraParameters (tempPath, camera)
	fileName2 = renderlib.GetFileNameFromParameters (configFileName, algorithm, samples, 'stereo2' + postFix)
	renderlib.Render (binaryPath, tempPath, fileName2, algorithm, samples, True)
	
	renderlib.DeleteFile (tempPath)
	
	resultFileName = renderlib.GetFileNameFromParameters (configFileName, algorithm, samples, 'stereo' + postFix)
	resultFileName.replace ('.png', '.jpg')
	image1 = Image.open (fileName1)
	image2 = Image.open (fileName2)
	width, height = image1.size

	newWidth = width / 2
	image1 = image1.resize ((newWidth, height), Image.ANTIALIAS)
	image2 = image2.resize ((newWidth, height), Image.ANTIALIAS)

	resultImage = Image.new ('RGBA', (width, height), (255, 255, 255, 255))
	resultImage.paste (image1, (0, 0, newWidth, height))
	resultImage.paste (image2, (newWidth, 0, 2 * newWidth, height))
	resultImage.save (resultFileName)
	
def Main ():
	currentPath = os.path.dirname (os.path.abspath (__file__))
	os.chdir (currentPath)

	binaryPath = os.path.abspath ('../project/Release/CommandLine.exe')
	
	algorithm = 'pathtrace2'
	samples = 64
	resolution = 0
	cameraOffset = 0.1
	
	rotation = 0
	while rotation < 360:
		Render (binaryPath, 'testfiles/teapot.txt', resolution, algorithm, samples, cameraOffset, rotation, '_' + str (rotation))
		rotation += 1
	
	#Render (binaryPath, 'testfiles/dragon.txt', resolution, algorithm, samples, cameraOffset)
	#Render (binaryPath, '../test/source/05_rotated_cylinder.txt', resolution, algorithm, samples, cameraOffset)
	#Render (binaryPath, '../test/source/08_sphere_and_cylinder.txt', resolution, algorithm, samples, cameraOffset)
	#Render (binaryPath, '../test/source/18_various_shapes.txt', resolution, algorithm, samples, cameraOffset)
	#Render (binaryPath, '../test/source/11_teapot.txt', resolution, algorithm, samples, cameraOffset)
	#Render (binaryPath, '../test/source/16_dragon.txt', resolution, algorithm, samples, cameraOffset)
	#Render (binaryPath, '../test/source/12_bunny.txt', resolution, algorithm, samples, cameraOffset)	

Main ()
