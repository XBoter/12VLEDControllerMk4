import os

html_compressed_folder_local_path = "src\\Webpage\\html_compressed"
transformed_c_folder_local_path = "src\\Webpage\\transformed_to_c"

cur_dir = os.getcwd()

found_src_folder_in_cur_dir = False

# Search for src folder
for folder in os.listdir(cur_dir):
    if folder == "src":
        found_src_folder_in_cur_dir = True
        break

if found_src_folder_in_cur_dir:
    transformed_c_folder_local_path_full_path = os.path.join(cur_dir, transformed_c_folder_local_path)
    html_compressed_folder_full_path = os.path.join(cur_dir, html_compressed_folder_local_path)
else:
    transformed_c_folder_local_path_full_path = os.path.join(os.path.dirname(cur_dir), transformed_c_folder_local_path)
    html_compressed_folder_full_path = os.path.join(os.path.dirname(cur_dir), html_compressed_folder_local_path)

print("Transformed C File Folder path '{}'".format(transformed_c_folder_local_path_full_path))
print("Compressed Folder path '{}'".format(html_compressed_folder_full_path))
print("")

# Create folder if missing
if not os.path.isdir(transformed_c_folder_local_path_full_path):
    os.mkdir(transformed_c_folder_local_path_full_path)
    print("Created missing dir '{}'".format(os.path.basename(transformed_c_folder_local_path_full_path)))
    print("")

if not os.path.isdir(html_compressed_folder_full_path):
    os.mkdir(html_compressed_folder_full_path)
    print("Created missing dir '{}'".format(os.path.basename(html_compressed_folder_full_path)))
    print("")

compressed_html_files = []
for file in os.listdir(html_compressed_folder_full_path):
    if file.endswith("_compressed.html"):
        compressed_html_files.append(file)

print("Found {} compressed HTML files".format(len(compressed_html_files)))
print("")

for compressed_html_file in compressed_html_files:

    html_compressed_folder_full_path_with_file = os.path.join(html_compressed_folder_full_path,compressed_html_file)
    compressed_html_file_content = ""
    with open(html_compressed_folder_full_path_with_file) as f:
        # minify puts all the compressed html code in line one
        compressed_html_file_content = f.readline()
        f.close()

    if compressed_html_file_content != "":
        
        transformed_c_folder_local_path_full_path_with_file = os.path.join(transformed_c_folder_local_path_full_path,compressed_html_file.replace("_compressed.html",".h"))

        # save html file to compressed fcompressed_html_file_contentolder
        with open(transformed_c_folder_local_path_full_path_with_file, 'w') as f:
            data = '#pragma once\nconst char {}[] PROGMEM = "{}";'.format(compressed_html_file.replace("_compressed.html",""),compressed_html_file_content)
            f.write(data)
            f.close()

        print("Transformed HTML file '{}' to C Header '{}'".format(compressed_html_file,compressed_html_file.replace("_compressed.html",".h")))

    else:
        print("Error reading file '{}'".format(html_compressed_folder_full_path_with_file))