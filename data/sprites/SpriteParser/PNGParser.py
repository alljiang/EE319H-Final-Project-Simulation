from PIL import Image, ImageSequence
import numpy as np
import os

#   Format (all big endian):
#
#
#


#   CONFIG
backgroundColor = 0x00FF00
imageDirectory = "./png/"
imageFilename = "winp2val"

# color index list
indexed_rgb = list()

# Output file

numFiles = len(os.listdir(os.getcwd() + imageDirectory))

for filename in os.listdir(os.getcwd() + imageDirectory):
    if filename.endswith(".png") and filename.split('.')[0] == imageFilename:
        output = open(filename.split('.')[0] + ".txt", "wb")
        img = Image.open(imageDirectory + filename)

        frame = np.array([np.array(frame.copy().convert('RGB').getdata(), dtype=np.uint8)
                          .reshape(frame.size[1], frame.size[0], 3) for frame in ImageSequence.Iterator(img)])[0]

        output.write((int(len(frame[0])) & 0xFFFF).to_bytes(2, byteorder="big", signed=False))
        output.write(int(len(frame) & 0xFF).to_bytes(1, byteorder="big", signed=False))

        for row in frame:
            for i, pixel in enumerate(row):
                red = int(pixel[0])
                green = int(pixel[1])
                blue = int(pixel[2])
                rgb_int = red << 16 | green << 8 | blue

                index = 0

                if not indexed_rgb.__contains__(rgb_int):
                    indexed_rgb.append(rgb_int)
                index = indexed_rgb.index(rgb_int)

                output.write((index & 0xFFFFFF).to_bytes(2, byteorder="big", signed=False))  # color

# output.write("\n".encode())

# output colors.txt file
colors = open("colors_"+imageFilename+".h", 'w')
colors.write('#include <stdint.h>\n'
             '#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_COLORS'+imageFilename+'_H\n' +\
             '#define EE319K_FINAL_PROJECT_INITIAL_TESTING_COLORS'+imageFilename+'_H\n\n' +\
'const uint32_t colors_'+imageFilename+'[' + str(len(indexed_rgb)) + '] = {')
colors.write(str(indexed_rgb[0]))
for i in range(1, len(indexed_rgb)):
    colors.write(',')
    colors.write(str(indexed_rgb[i]))

colors.write('};\n\n')
colors.write('#endif //EE319K_FINAL_PROJECT_INITIAL_TESTING_COLORS'+imageFilename+'_H\n')
colors.close()