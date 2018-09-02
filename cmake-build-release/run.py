import os, sys

try:
    os.remove("result.txt")
except:
    pass

for x in range(1, 16):
    os.system("prod_cons.exe {0} {1} >> result.txt".format(x, 16 - x))

