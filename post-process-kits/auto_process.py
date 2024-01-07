import os
import re

# get path all file
def get_pwd_filename(path):
    filename_list = [filename for filename in os.listdir(path)]
    return filename_list

# path = r'./'
# for filename in os.listdir(path):
#     print(os.path.join(path, filename))

filename_list = get_pwd_filename('./')
# print(filename_list)

# rm nonsence files
command = 'rm -rf ./*.calculated ./*.parsed ./cesm.exe*'
os.system(command)


command = 'cp ../../bld/cesm.exe .'
os.system(command)
command = 'python process_addr.py cesm.exe'
os.system(command)

for filename in filename_list:
    if re.search('factrace.txt', filename) != None:
        if re.search('.parse', filename) == None and re.search('.calculated', filename) == None:
            # print(filename)
            # parse addr
            parsed_filename = filename + '.parsed'
            command = './parse    cesm.exe.terminal ' + '       ' + filename + ' ' + parsed_filename
            print(command)
            os.system(command)
            
            
# update file name list
filename_list = get_pwd_filename('./')

for filename in filename_list:
    if re.search('.parse', filename) != None:
        # cal self run time
        calculated_filename = filename + '.calculated' 
        command = './cal-self-runtime ' + '    ' + filename + '       ' + calculated_filename
        print(command)
        os.system(command)
        
command = 'ls -alh'
os.system(command)
