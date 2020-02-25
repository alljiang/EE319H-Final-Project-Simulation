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
#       h elements each of size 2 bytes each describing the indexes of each row (2*h bytes)
#       entire picture data:
#        (color index, 2 bytes) (number of repeated color, 2 bytes) (very long)

#   CONFIG
backgroundColor = 0x00FF00
characterName = "kirby"
imageDirectory = "/gif-" + characterName + "/"

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
output = open(characterName + ".txt", "wb")

numFiles = len(os.listdir(os.getcwd() + "/gif-kirby/"))
for filename in os.listdir(os.getcwd() + "/gif-kirby/"):
    if filename.endswith(".gif") or filename.endswith(".png"):
        img = Image.open("./gif-kirby/" + filename)
        frames = np.array([np.array(frame.copy().convert('RGB').getdata(), dtype=np.uint8)
                          .reshape(frame.size[1], frame.size[0], 3) for frame in ImageSequence.Iterator(img)])
        compressed_rgb = []

        frame_width = 0
        frame_height = 0

        allRowIndexes = []

        # output.write(str(numFiles).encode())  # human readable, debug purposes only
        output.write((int(numFiles) & 0xFF).to_bytes(2, byteorder="big", signed=False))

        # Convert 8-bit color to 6-bit color and store in index array
        for frame in frames:
            compressed_frame = []
            frame_height = len(frame)
            rowIndexes = []
            currentRowIndex = 0
            for row in frame:
                frame_width = len(row)
                compressed_row = []
                prev_color = -99
                line_len = 1
                rowIndexes.append(currentRowIndex)
                for i, pixel in enumerate(row):
                    red = int(pixel[0])
                    green = int(pixel[1])
                    blue = int(pixel[2])
                    rgb_int = red << 16 | green << 8 | blue
                    if rgb_int == backgroundColor:
                        rgb_int = 0xFFFFFFFF
                    if rgb_int == prev_color:
                        line_len += 1           # max line length = 255
                        if i == len(row)-1:
                            compressed_row.append([indexed_rgb.index(rgb_int), line_len])
                            currentRowIndex += 2
                    else:
                        if not indexed_rgb.__contains__(rgb_int):
                            indexed_rgb.append(rgb_int)
                        if i != 0:
                            compressed_row.append([indexed_rgb.index(prev_color), line_len])
                            currentRowIndex += 2
                        if i == len(row) - 1:
                            compressed_row.append([indexed_rgb.index(rgb_int), 1])
                            currentRowIndex += 2
                        prev_color = rgb_int
                        line_len = 1
                compressed_frame.append(compressed_row)
            compressed_rgb.append(compressed_frame)
            allRowIndexes.append(rowIndexes)

        #   Unreadable bytes, but much more efficient
        output.write((filename.split(".")[0] + "\n").encode())
        output.write((int(len(frames)) & 0xFF).to_bytes(1, byteorder="big", signed=False))
        output.write((int(frame_width) & 0xFFFF).to_bytes(2, byteorder="big", signed=False))
        output.write(int(frame_height & 0xFF).to_bytes(1, byteorder="big", signed=False))
        for i in range (0, len(frames)):
            frame = compressed_rgb[i]

            for index in allRowIndexes[i]:
                output.write((int(index) & 0xFFFF).to_bytes(2, byteorder="big", signed=False))

            for row in frame:
                for line in row:
                    output.write((int(line[0]) & 0xFFFF).to_bytes(2, byteorder="big", signed=False))    #   color
                    output.write((int(line[1]) & 0xFFFF).to_bytes(2, byteorder="big", signed=False))    #   quantity


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