import os
import sys
import re
import math

def DeleteFile (filePath):
	if os.path.exists (filePath):
		os.remove (filePath)

def Render (binaryPath, configFile, resultFile, algorithm, samples, verbose):
	command = binaryPath
	command += ' --config "' + configFile + '"'
	command += ' --result "' + resultFile + '"'
	command += ' --algorithm ' + algorithm
	command += ' --samples ' + str (samples)
	if verbose:
		command += ' --verbose'
	return os.system (command)

def ModifyResolution (originalConfigFile, tempConfigFile, resolution):
	file = open (originalConfigFile, 'rb')
	content = file.read ()
	file.close ()
	content = re.sub ('xresolution \d+', 'xresolution ' + str (resolution), content)
	content = re.sub ('yresolution \d+', 'yresolution ' + str (resolution), content)
	file = open (tempConfigFile, 'wb')
	file.write (content)
	file.close ()

def GetCameraParameters (configFile):
	file = open (configFile, 'rb')
	content = file.read ()
	file.close ()
	found = re.search ('eye ([\d\.]+) ([\d\.]+) ([\d\.]+)', content)
	eye = [float (found.group (1)), float (found.group (2)), float (found.group (3))]
	found = re.search ('center ([\d\.]+) ([\d\.]+) ([\d\.]+)', content)
	center = [float (found.group (1)), float (found.group (2)), float (found.group (3))]
	found = re.search ('up ([\d\.]+) ([\d\.]+) ([\d\.]+)', content)
	up = [float (found.group (1)), float (found.group (2)), float (found.group (3))]
	camera = {
		'eye' : eye,
		'center' : center,
		'up' : up
	}
	return camera

def ModifyCameraParameters (originalConfigFile, tempConfigFile, camera):
	file = open (originalConfigFile, 'rb')
	content = file.read ()
	file.close ()
	content = re.sub ('eye ([\d\.]+) ([\d\.]+) ([\d\.]+)', 'eye ' + str (camera['eye'][0]) + ' ' + str (camera['eye'][1]) + ' ' + str (camera['eye'][2]), content)
	content = re.sub ('center ([\d\.]+) ([\d\.]+) ([\d\.]+)', 'center ' + str (camera['center'][0]) + ' ' + str (camera['center'][1]) + ' ' + str (camera['center'][2]), content)
	content = re.sub ('up ([\d\.]+) ([\d\.]+) ([\d\.]+)', 'up ' + str (camera['up'][0]) + ' ' + str (camera['up'][1]) + ' ' + str (camera['up'][2]), content)
	file = open (tempConfigFile, 'wb')
	file.write (content)
	file.close ()
	
def VecSub (a, b):
	return [a[0] - b[0], a[1] - b[1], a[2] - b[2]]

def VecMult (a, b):
	return [a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]]

def Normalize (a):
	length = math.sqrt (a[0] * a[0] + a[1] * a[1] + a[2] * a[2])
	return [a[0] / length, a[1] / length, a[2] / length]

def Offset (vec, direction, distance):
	normal = Normalize (direction);
	return [vec[0] + normal[0] * distance, vec[1] + normal[1] * distance, vec[2] + normal[2] * distance]
	
def OffsetEyePosition (camera, offset):
	eyeToCenter = Normalize (VecSub (camera['center'], camera['eye']))
	xDirection = Normalize (VecMult (eyeToCenter, camera['up']));
	camera['eye'] = Offset (camera['eye'], xDirection, offset)
