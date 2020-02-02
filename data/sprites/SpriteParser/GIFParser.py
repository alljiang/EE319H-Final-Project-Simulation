from PIL import Image, ImageSequence
import numpy as np
import os

# Output file
output = open("data.txt", "w")

for filename in os.listdir(os.getcwd()):
    if filename.endswith(".gif"):
        img = Image.open(filename)
        frames = np.array([np.array(frame.copy().convert('RGB').getdata(), dtype=np.uint8)
                          .reshape(frame.size[1], frame.size[0], 3, 1) for frame in ImageSequence.Iterator(img)])

        # Convert 8-bit color to 6-bit color
        for frame in frames:
            for row in frame:
                for pixel in row:
                    for rgb in pixel:
                        rgb[0] /= 4

        # Write to file
        output.write(f"{filename}: \n")
        frame_counter = 1
        for frame in frames:
            output.write("[")
            row_counter = 1
            for row in frame:
                output.write("[")
                pxl_counter = 1
                for pixel in row:
                    output.write("[")
                    rgb_counter = 1
                    for rgb in pixel:
                        output.write(str(rgb[0]))
                        if rgb_counter % len(pixel) != 0:
                            output.write(", ")
                            rgb_counter += 1
                    output.write("]")
                    if pxl_counter % len(row) != 0:
                        output.write(",\n")
                        pxl_counter += 1
                output.write("]")
                if row_counter % len(frame) != 0:
                    output.write(",\n\n")
                    row_counter += 1
            output.write("]\n")
        output.write(f"End of {filename}\n\n")


