from PIL import Image, ImageSequence
import numpy as np
import os

# Output file
output = open("data.txt", "w")

indexed_rgb = []

for filename in os.listdir(os.getcwd()):
    if filename.endswith(".gif"):
        img = Image.open(filename)
        frames = np.array([np.array(frame.copy().convert('RGB').getdata(), dtype=np.uint8)
                          .reshape(frame.size[1], frame.size[0], 3) for frame in ImageSequence.Iterator(img)])
        compressed_rgb = []
        # Convert 8-bit color to 6-bit color and store in index array
        for frame in frames:
            compressed_frame = []
            for row in frame:
                compressed_row = []
                for pixel in row:
                    rgb_int = 0
                    for rgb in pixel:
                        rgb /= 4
                        rgb_int += rgb
                        rgb_int *= 100
                    rgb_int /= 100
                    if not indexed_rgb.__contains__(rgb_int):
                        indexed_rgb.append(rgb_int)
                    compressed_row.append(indexed_rgb.index(rgb_int))
                compressed_frame.append(compressed_row)
            compressed_rgb.append(compressed_frame)

        # Write to file
        output.write(f"Colors: \n {indexed_rgb}\n\n")
        output.write(f"{filename}: \n")
        # frame_counter = 1
        for i, frame in enumerate(frames):
            output.write("[")
            # row_counter = 1
            for j, row in enumerate(frame):
                output.write("[")
                # pxl_counter = 1
                for k, pixel in enumerate(row):
                    output.write(str(compressed_rgb[i][j][k]))
                    if k != len(row)-1:
                        output.write(",\n")
                output.write("]")
                if j != len(frame)-1:
                    output.write(",\n\n")
            output.write("]\n")
        output.write(f"End of {filename}\n\n")



