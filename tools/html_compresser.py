import os
import minify_html

html_uncompressed_folder_local_path = "src\\Webpage\\html_uncompressed"
html_compressed_folder_local_path = "src\\Webpage\\html_compressed"
cur_dir = os.getcwd()

print("Cur dir '{}'".format(cur_dir))

print("Dir under cur dir")
for x in os.listdir(cur_dir):
    print(x)

# Prints the html page before and after minified
enable_hml_print = False

html_uncompressed_folder_full_path = os.path.join(os.path.dirname(cur_dir), html_uncompressed_folder_local_path)
html_compressed_folder_full_path = os.path.join(os.path.dirname(cur_dir), html_compressed_folder_local_path)

print("Uncompressed Folder path '{}'".format(html_uncompressed_folder_full_path))
print("Compressed Folder path '{}'".format(html_compressed_folder_full_path))
print("")

html_files = []
for file in os.listdir(html_uncompressed_folder_full_path):
    if file.endswith(".html"):
        html_files.append(file)

print("Found {} uncompressed HTML files".format(len(html_files)))
print("")

for html_file in html_files:

    html_uncompressed_folder_full_path_with_file = os.path.join(html_uncompressed_folder_full_path,html_file)
    html_file_content = ""
    html_file_content_minified = ""
    with open(html_uncompressed_folder_full_path_with_file) as f:
        html_file_content = f.read()
        f.close()

    if html_file_content != "":
        # minify html page
        html_file_content_minified = minify_html.minify(html_file_content, minify_js=True,minify_css=True, remove_processing_instructions=True)
        if enable_hml_print:
            print('='*10+" ORIGINAL "+'='*10)
            print(html_file_content)
            print('='*10+" MINIFIED "+'='*10)
            print(html_file_content_minified)

        html_compressed_folder_full_path_with_file = os.path.join(html_compressed_folder_full_path,html_file.replace(".html","_compressed.html"))


        # save html file to compressed folder
        with open(html_compressed_folder_full_path_with_file, 'w') as f:
            f.write(html_file_content_minified)
            f.close()

        uncompressed_file_size = os.stat(html_uncompressed_folder_full_path_with_file).st_size
        compressed_file_size = os.stat(html_compressed_folder_full_path_with_file).st_size

        print("Compressed HTML file '{}' reduced size by {:.2f}%".format(html_file, abs((compressed_file_size / uncompressed_file_size) * 100 - 100)))

    else:
        print("Error reading file '{}'".format(html_uncompressed_folder_full_path_with_file))
    