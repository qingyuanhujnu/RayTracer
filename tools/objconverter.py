import os
import sys
import re
import time
import shutil
import filecmp

def GetFileContent (filePath):
	file = open (filePath, 'rb')
	content = file.read ()
	file.close ()
	return content
	
def SetFileContent (filePath, content):
	file = open (filePath, 'wb')
	file.write (content)
	file.close ()

def GetVerticesAndTriangles (content, vertices, polygons):
	def GetVertexFromLine (line):
		splitted = line.split (' ')
		result = []
		for part in splitted:
			if part == 'v' or part == '':
				continue
			result.append (part)
		return result

	def GetPolygonFromLine (line):
		splitted = line.split (' ')
		result = []
		for part in splitted:
			if part == 'f' or part == '':
				continue
			result.append (part.split ('/')[0])
		return result

	splitted = content.split ('\n')
	for part in splitted:
		line = part.strip ()
		if line.startswith ('v '):
			vertex = GetVertexFromLine (line)
			vertices.append (vertex)
		elif line.startswith ('f '):
			polygon = GetPolygonFromLine (line)
			polygons.append (polygon)
	
def Main ():
	currentPath = os.path.dirname (os.path.abspath (__file__))
	os.chdir (currentPath)

	if len (sys.argv) != 4:
		print 'usage: objconverter <objFile> <resultFile> <scale>'
		return
		
	objFilePath = sys.argv[1]
	resultFilePath = sys.argv[2]
	scale = float (sys.argv[3])
	
	content = GetFileContent (objFilePath)
	vertices = []
	polygons = []
	GetVerticesAndTriangles (content, vertices, polygons)
	
	triangles = []
	for polygon in polygons:
		count = len (polygon)
		for i in range (0, count - 2):
			triangles.append ([polygon[0], polygon[(i + 1) % count], polygon[(i + 2) % count]])
	
	result = ''
	result += 'mesh\n'
	result += '\tvertices ' + str (len (vertices)) + '\n'
	for vertex in vertices:
		result += '\t\t ' + str (float (vertex[0]) * scale) + ' ' + str (float (vertex[1]) * scale) + ' ' + str (float (vertex[2]) * scale) + '\n'
	result += '\ttriangles ' + str (len (triangles)) + '\n'
	for triangle in triangles:
		result += '\t\t ' + str (int (triangle[0]) - 1) + ' ' + str (int (triangle[1]) - 1) + ' ' + str (int (triangle[2]) - 1) + ' <material> <curvegroup>\n'
	result += '\toffset 0 0 0\n'
	result += '\trotation 0 0 0\n'
	
	SetFileContent (resultFilePath, result)
	return
	
Main ()
