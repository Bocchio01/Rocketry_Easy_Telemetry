import os
import shutil


def clean_latex_files(directory):
    # Remove LaTeX-generated files with specific extensions
    extensions = ['.aux', '.log', '.out', '.toc', '.fdb_latexmk',
                  '.fls', '.synctex.gz', '.gz', '.bbl', '.blg', '.bcf', '.run.xml']
    for filename in os.listdir(directory):
        if any(filename.endswith(ext) for ext in extensions):
            file_path = os.path.join(directory, filename)
            os.remove(file_path)
            print(f"Deleted file: {file_path}")


def copy_file(source_dir, destination_dir, file_name):
    source_file = os.path.join(source_dir, file_name)
    destination_file = os.path.join(destination_dir, file_name)
    shutil.copyfile(source_file, destination_file)
    print(f"Copied file from {source_file} to {destination_file}")


# Example usage
directory = './'
source_dir = './'
destination_dir = 'C:/Users/Bocchio/Documents/GitHub/Bocchio_WebSite_Utils/ERT'
file_to_copy = 'Official Announcement.pdf'

clean_latex_files(directory)
# copy_file(source_dir, destination_dir, file_to_copy)
