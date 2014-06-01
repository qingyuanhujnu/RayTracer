import os
import sys
import re
import time
import shutil
import filecmp

from lib import renderlib

def Main ():
	currentPath = os.path.dirname (os.path.abspath (__file__))
	os.chdir (currentPath)

	binaryPath = os.path.abspath ('../project/Release/CommandLine.exe')
	configPath = '..\\test\\source\\08_sphere_and_cylinder.txt'
	
	algorithm = 'raytrace'
	samples = 9
	resolution = 1000
	tempPath = 'temp.txt'
	
	renderlib.ModifyResolution (configPath, tempPath, resolution)
	renderlib.Render (binaryPath, tempPath, 'stereo_1.png', algorithm, samples, True)
	
	camera = renderlib.GetCameraParameters (tempPath)
	renderlib.OffsetEyePosition (camera, 0.1)
	renderlib.ModifyCameraParameters (tempPath, tempPath, camera)
	renderlib.Render (binaryPath, tempPath, 'stereo_2.png', algorithm, samples, True)
	renderlib.DeleteFile (tempPath)
	
Main ()
