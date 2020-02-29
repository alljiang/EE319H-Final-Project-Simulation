from PIL import Image, ImageSequence
import numpy as np
import os

#   Format (all big endian):
#
#   number of sequences/animations (2 bytes)
#       sequence name (ASCII) + '\n' (this is on the same line as above)
#
#       The following is all on one line, no spaces:
#       number of frames in sequence (1 byte)
#       width (w) of each frame (pixels) (2 bytes)
#       height (h) of each frame (pixels) (1 byte)
#
#       f elements each of size 3 bytes each describing the indexes of each frame (3*f bytes)
#
#       for each frame:
#           h+1 elements each of size 2 bytes each describing the indexes of each row (2*(h+1) bytes)
#               last element is basically the size of all the data
#           entire picture data:
#           (color index, 2 bytes) (number of repeated color, 2 bytes) (very long)

#   CONFIG
backgroundColor = 0x00FF00
imageDirectory = "./png-misc/"

# color index list
indexed_rgb = list()
colors = open("colors.txt", 'r')
colorsRawLine = colors.readline()
colorsRawLine = colorsRawLine[colorsRawLine.index("{") + 1:len(colorsRawLine)-3]  # trim out brackets
if len(colorsRawLine) != 0:
    colorsSplit = colorsRawLine.split(',')
    for c in colorsSplit:
        indexed_rgb.append(int(c))
colors.close()

# Output file

numFiles = len(os.listdir(os.getcwd() + imageDirectory))

for filename in os.listdir(os.getcwd() + imageDirectory):
    if filename.endswith(".png"):
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

                output.write((index & 0xFFFF).to_bytes(2, byteorder="big", signed=False))  # color

# output.write("\n".encode())

# output colors.txt file
colors = open("colors.txt", 'w')
colors.write('const uint32_t colors[' + str(len(indexed_rgb)) + '] = {')
colors.write(str(indexed_rgb[0]))
for i in range(1, len(indexed_rgb)):
    colors.write(',')
    colors.write(str(indexed_rgb[i]))

colors.write('};\n')
colors.close()