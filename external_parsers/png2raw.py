from PIL import Image
import numpy as np
from sys import argv
from sys import argv


def parse(file):
    image = Image.open(file).convert('RGBA')
    image = np.asarray(image, dtype='int64')
    width = image.shape[1]
    height = image.shape[0]
    dumpFile = open(argv[2],"w")
    dumpFile.write("#ifndef "+str(argv[2]).upper().replace('.', '_')+"\n#define "+str(argv[2]).upper().replace('.', '_')+"\n#include <stdint.h>" + "\n#define "+argv[1].split(".")[0].upper()+"WIDTH " + str(width) + "\n#define "+argv[1].split(".")[0].upper()+"HEIGHT " + str(height) + "\nstatic const uint32_t "+argv[1].split(".")[0]+"["+argv[1].split(".")[0].upper()+"HEIGHT]["+argv[1].split(".")[0].upper()+"WIDTH] = {\n")
    firstLine=True
    for line in image:
        if (firstLine):
            dumpFile.write("\t{ ")
            firstLine = False
        else:
            dumpFile.write(",\n\t{")
        firstPixel = True
        for pixel in line:
            #ARGB
            newPixel = (pixel[2])
            newPixel += (pixel[1]) << 8
            newPixel += (pixel[0]) << 16           
            if (firstPixel):
                dumpFile.write("0x{:X}".format(newPixel))
                firstPixel = False
            else:
                dumpFile.write(", " + "0x{:X}".format(newPixel))
        dumpFile.write("}")
            
    dumpFile.write("\n };\n")
    dumpFile.write("\n\n#endif")
    dumpFile.close()

parse(argv[1])