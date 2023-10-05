from PIL import Image

# Open the PNG file



image = Image.open("palette.png")
print(image.getpixel((40,0)))
# Get the color palette
# palette = image.getpalette()

# # Create an empty list for the RGB values
# rgb_list = []

# # Loop through the palette and convert each color to RGB
# print(palette)
# for i in range(0, len(palette), 3):
#     r = palette[i]
#     g = palette[i+1]
#     b = palette[i+2]
#     rgb_list.append((r, g, b))

# # Print the RGB list
# print("RGB list:", rgb_list)