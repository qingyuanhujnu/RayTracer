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
	configPath = 'teapot.txt'
	
	algorithm = 'raytrace'
	samples = 1
	tempPath = 'temp.txt'
	
	renderlib.Render (binaryPath, configPath, 'stereo_1.png', algorithm, samples, True)
	camera = renderlib.GetCameraParameters (configPath)
	renderlib.OffsetEyePosition (camera, 0.1)
	renderlib.ModifyCameraParameters (configPath, tempPath, camera)
	renderlib.Render (binaryPath, tempPath, 'stereo_2.png', algorithm, samples, True)
	renderlib.DeleteFile (tempPath)
	
Main ()
