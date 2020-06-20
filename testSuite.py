#!/usr/bin/env python3

import subprocess
import sys

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def differenceLists(firstList, secondList):
    app = [x for x in firstList if x not in secondList]
    ref = [x for x in secondList if x not in firstList]
    return app, ref

def parseList(output, app=False):
    output = output.stdout.decode("utf-8")
    output = output.replace("},{\"", "},,{\"")
    if (app):
        output = output.split("\n", 1)[1]
    output = output.split("[",1)[1]
    output = output.split("]", 1)[0]
    list_output = output.split(',,')
    return list_output

def main(dist, word, path_app, path_ref, path_dict_app, path_dict_ref):
    output_app = subprocess.run(['echo approx {0} {1} | ./{2} {3}'.format(dist, word, path_app, path_dict_app)], capture_output=True, shell=True)
    list_output_app = parseList(output_app, True)

    output_ref = subprocess.run(['echo approx {0} {1} | ./{2} {3}'.format(dist, word, path_ref, path_dict_ref)],
                                capture_output=True, shell=True)
    list_output_ref = parseList(output_ref)

    app, ref = differenceLists(list_output_app, list_output_ref)

    print("\n App difference : \n")
    print(f"{bcolors.FAIL}{bcolors.OKGREEN}")
    print(app)
    print(bcolors.ENDC)
    print("\n Ref difference : \n")
    print(f"{bcolors.OKGREEN}")
    print(ref)
    print(bcolors.ENDC)


if __name__ == '__main__':
    main('1', 'test', 'main', 'TextMiningAppRef', 'dict.bin', 'dict2.bin')
    #main(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5], sys.argv[6])
