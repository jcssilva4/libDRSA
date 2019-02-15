import DRSA
import os

# Read data
script_dir = os.path.dirname(__file__)
filename = "data_bankrupt"
print('reading' + filename + " data...")
relative_path = "../datafiles/" + filename + ".txt"
path = os.path.join(script_dir, relative_path)

filelines = []
filehandle= open(path, "r")
#filelines.extend(filehandle.readlines())
# get |U|, |C| and |D|
filehandle.readline() # get first comment
firstDataBlock = filehandle.readline()
[u,c,d] = firstDataBlock.split(",")
[u,c,d] = [int(u),int(c),int(d)] #convert from str to int
# get criteria preference direction
prefDir = [0]*c # initialize
filehandle.readline() # get second comment
secondDataBlock = filehandle.readline()
prefDir = secondDataBlock.split(",")
prefDir = list(map(int, prefDir)) #convert from str to in
# get intracriteria evaluation and classification
f = []
filehandle.readline() # get third comment
filelines.extend(filehandle.readlines())
for line in filelines:
    thisDataBlock = line.split("\t")
    thisDataBlock = list(map(float, thisDataBlock)) # convert from str to float
    f.append(thisDataBlock)
#print(f)
print ("done!")

# find decision rules using DRSA
# problem: you need to build two module functions: one to initialize DRSA and another to find rules (DONE!)
DRSA.initialize(u, c, prefDir, f)
DRSA.findRules("domlem")
print ("allright!")