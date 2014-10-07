import os
import zipfile
import glob
import ntpath

def zipdir(path, zip):
    for root, dirs, files in os.walk(path):
        for file in files:
            zip.write(os.path.join(root, file))

def copyAllOfType(zipf, pattern, outputdir, recursive=False) :
    for f in glob.glob(pattern) :
        if f.endswith('~') :
            continue

        zipf.write(f, os.path.join(outputdir, ntpath.basename(f)), zipfile.ZIP_DEFLATED)
        
        if recursive :
            copyAllOfType(zipf, os.path.join(f, "*"), os.path.join(outputdir, ntpath.basename(f)), True)

def makeLevelPack(name) :
	print 'Creating level pack for', name

	dataPath = name

        files = []

	zipf = zipfile.ZipFile(name + '.zip', 'w')

        # copy xml file
        copyAllOfType(zipf, os.path.join(dataPath, "*.xml"), '')
        # copy language
        copyAllOfType(zipf, os.path.join(dataPath, 'res/*'), 'res', True)
	zipf.close()

# set this as working dir
working_dir = os.path.dirname(os.path.realpath(__file__))
os.chdir(working_dir)

if __name__ == '__main__':
    for path in os.listdir(working_dir) :
        if not os.path.isfile(os.path.join(working_dir, path)) :
            makeLevelPack(path)

