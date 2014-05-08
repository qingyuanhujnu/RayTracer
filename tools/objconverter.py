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

def GetGeometry (content, vertices, normals, polygons):
	def GetVertexFromLine (line):
		splitted = line.split (' ')
		result = []
		for part in splitted:
			if part == 'v' or part == '':
				continue
			result.append (part)
		return result

	def GetNormalFromLine (line):
		splitted = line.split (' ')
		result = []
		for part in splitted:
			if part == 'vn' or part == '':
				continue
			result.append (part)
		return result

	def GetPolygonFromLine (line):
		splitted = line.split (' ')
		result = []
		for part in splitted:
			if part == 'f' or part == '':
				continue
			splittedPart = part.split ('/')
			vertex = splittedPart[0]
			normal = 0
			if len (splittedPart) >= 2:
				normal = splittedPart[2]
			result.append ([vertex, normal])
		return result

	splitted = content.split ('\n')
	for part in splitted:
		line = part.strip ()
		if line.startswith ('v '):
			vertex = GetVertexFromLine (line)
			vertices.append (vertex)
		elif line.startswith ('vn '):
			normal = GetNormalFromLine (line)
			normals.append (normal)
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
	normals = []
	polygons = []
	GetGeometry (content, vertices, normals, polygons)
	
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
	result += '\tnormals ' + str (len (normals)) + '\n'
	for normal in normals:
		result += '\t\t ' + str (float (normal[0])) + ' ' + str (float (normal[1])) + ' ' + str (float (normal[2])) + '\n'
	result += '\ttriangles ' + str (len (triangles)) + '\n'
	for triangle in triangles:
		result += '\t\t ' + str (int (triangle[0][0]) - 1) + ' ' + str (int (triangle[1][0]) - 1) + ' ' + str (int (triangle[2][0]) - 1)
		result += ' ' + str (int (triangle[0][1]) - 1) + ' ' + str (int (triangle[1][1]) - 1) + ' ' + str (int (triangle[2][1]) - 1)
		result += ' <material> <curvegroup>\n'
	result += '\toffset 0 0 0\n'
	result += '\trotation 0 0 0\n'
	
	SetFileContent (resultFilePath, result)
	return
	
Main ()
