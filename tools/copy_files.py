# Inspired by: https://github.com/arendst/Tasmota/blob/development/pio/name-firmware.py
# Thanks Theo & Jason2866 :)

Import('env')
import os
import shutil

OUTPUT_DIR = "build_output{}".format(os.path.sep)

def copy_to_build_output(sourcedir, variant, file_suffix):
    in_file = "{}{}".format(variant, file_suffix)
    full_in_file = os.path.join(sourcedir, in_file)
    firmware_name = in_file = "{}{}".format(str(sourcedir).split(os.path.sep)[2], file_suffix)

    if os.path.isfile(full_in_file):
        if ".bin" in file_suffix:
            out_file = "{}bin{}{}".format(OUTPUT_DIR, os.path.sep, firmware_name)
        else:
            out_file = "{}debug{}{}".format(OUTPUT_DIR, os.path.sep, firmware_name)

        if os.path.isfile(out_file):
            os.remove(out_file)

        print("\u001b[33m in file : \u001b[0m  {}".format(full_in_file))
        
        print("\u001b[33m copy to: \u001b[0m  {}".format(out_file))
        shutil.copy(full_in_file, out_file)


def bin_elf_copy(source, target, env):
    variant = env['PROGNAME']
    split_path = str(source[0]).rsplit(os.path.sep, 1)

    # Create a dump of the used build environment
    with open('{}{}{}.env.txt'.format(split_path[0], os.path.sep, variant), 'w') as outfile:
        outfile.write(env.Dump())
        outfile.close()
    
    # check if output directories exist and create if necessary
    if not os.path.isdir(OUTPUT_DIR):
        os.mkdir(OUTPUT_DIR)

    for d in ['bin', 'debug']:
        if not os.path.isdir("{}{}".format(OUTPUT_DIR, d)):
            os.mkdir("{}{}".format(OUTPUT_DIR, d))

    for suff in [".elf", ".bin", ".bin.gz", "-factory.bin", ".env.txt"]:
        copy_to_build_output(split_path[0], variant, suff)


env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", [bin_elf_copy])
