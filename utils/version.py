#!/usr/bin/python

import optparse, os, string

parser = optparse.OptionParser()

parser.add_option('-a', '--major',
    action="store_true", dest="major",
    help="Increment Major Version", default=False)
parser.add_option('-i', '--minor',
    action="store_true", dest="minor",
    help="Increment Minor Version", default=False)

options, args = parser.parse_args()

def changeFile():
    dirname = os.getcwd()
    path = "../"
    if "/condor/utils" not in dirname:
        path = "./"

    path = path + "src/api.h"
    content = ""
    result = ""
    with open(path) as f:
        content = f.readlines()

        whichType = "BUG"

        if options.major: 
            whichType = "MAJOR"
        elif options.minor:
            whichType = "MINOR"

        oldLine = ''
        newLine = ''

        for line in content:
            if whichType in line:
                num = line[line.index(whichType) + len(whichType) + 1:]
                num = num.strip()
                num = int(num) + 1
                oldLine = line
                newLine = "\t#define " + whichType + " " + str(num) + "\n"
                break

        result = result.join(content)
        result = result.replace(oldLine, newLine)

    with open(path, "w") as f:
        f.write(result)

if __name__ == "__main__":
    changeFile()