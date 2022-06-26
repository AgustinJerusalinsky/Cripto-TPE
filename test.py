import os

STEGS = ["LSB1", "LSB4", "LSBI"]
ENC = ["NONE", "AES128", "AES192", "AES256", "DES"]
MODES = ["ECB", "CFB", "OFB", "CBC"]
password = "testing"
carrier = "ejemplo2022/lado.bmp"
secret = "ejemplo2022/itba.png"

def execute(steg, enc, mode):
   
    print("trying with steg: " + steg + " enc: " + enc + " mode: " + mode)
    
    command = f"./stegobmp --embed --steg {steg} -p {carrier} --in {secret} --out test_embed"
    if enc != "NONE":
        command += f" -a {enc} -m {mode} --pass {password}"
     
     #embed secret
    error = os.system(command + " > /dev/null")
    if error != 0:
        print("Error embedding")
        print(steg, enc, mode)
        exit(1)
    
    command = f"./stegobmp --extract --steg {steg} -p test_embed --in {secret} --out test_extract"
    if enc != "NONE":
        command += f" -a {enc} -m {mode} --pass {password}"
    #extract secret
    error = os.system(command + " --extract > /dev/null")
    if error != 0:
        print("Error extracting")
        print(steg, enc, mode)
        exit(1)
    
    #compare
    error = os.system(f"cmp {secret} test_extract.{secret.split('.')[-1]}")
    if error != 0:
        print("Error comparing")
        print(steg, enc, mode)
        exit(1)

    

for steg in STEGS:
    for enc in ENC:
        if enc == "NONE":
            execute(steg, enc, "")
        else:
            for mode in MODES:
                execute(steg, enc, mode)

os.remove("test_embed")
os.remove(f"test_extract.{secret.split('.')[-1]}")
