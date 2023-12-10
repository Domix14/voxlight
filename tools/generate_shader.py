import os
import sys
import logging

args = sys.argv

if len(args) < 2:
    logging.error("Invalid number of arguments")
    exit(1)

if("--help" in args):
    print("Usage: python generate_shaders.py  <shader_dir> <output_dir>")
    print("shader_dir: Directory containing shader files")
    print("output_dir: Directory to output generated header files")

shader_dir = args[1]
output_dir = args[2]

if not os.path.isdir(shader_dir):
    logging.error("Shader directory does not exist")
    exit(1)

if not os.path.isdir(output_dir):
    try:
        os.mkdir(output_dir)
    except OSError:
        logging.error("Failed to create output directory")



with open(f"{output_dir}/shaders.hpp", "w") as f:
    f.write("#pragma once\n")

    for dir_name in os.listdir(shader_dir):
        if not os.path.isdir(os.path.join(shader_dir, dir_name)):
            continue
        

        for file in os.listdir(os.path.join(shader_dir, dir_name)):
            if not file.endswith(".glsl"):
                continue

            shader_name = file.split(".")[0]

            file_path = os.path.join(shader_dir, dir_name, file)
            output_path = os.path.join(output_dir, shader_name + ".hpp")

            with open(file_path, "r") as sf:
                shader_data = sf.read()
                f.write(f"const char* {dir_name.upper()}_{shader_name.upper()}_SRC = R\"({shader_data})\";\n\n")
