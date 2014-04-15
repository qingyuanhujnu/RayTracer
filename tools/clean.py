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

def DeleteReleaseDebug (folderPath):
	DeleteFolder (os.path.join (folderPath, 'Release'))
	DeleteFolder (os.path.join (folderPath, 'Debug'))
		
def Main ():
	currentPath = os.path.dirname (os.path.abspath (__file__))
	os.chdir (currentPath)

	DeleteReleaseDebug (os.path.join ('..', 'project'))
	DeleteReleaseDebug (os.path.join ('..', 'project', 'RayTracer'))
	DeleteReleaseDebug (os.path.join ('..', 'project', 'CommandLine'))
	DeleteFolder (os.path.join ('..', 'project', 'UserInterface', 'obj'))
	DeleteFile (os.path.join ('..', 'project', 'RayTracer.sdf'))
	return
	
Main ()
