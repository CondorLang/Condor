#!/usr/bin/python

import optparse, os, string

parser = optparse.OptionParser()

parser.add_option('-f', '--internal-files',
    action="store_true", dest="internalFiles",
    help="Internal files", default="spam")

options, args = parser.parse_args()

def getBytes(name):
    if os.path.isfile(name):
        fh = open(name, 'rb')
        ba = bytearray()
        try:
            ba = bytearray(fh.read())
        finally:
            fh.close

        fh.close()
        return ba;

def clean(name):
    if os.path.isfile(name):
        os.remove(name)

def toFile(fName, s):
    if string:
        f = open(fName,'wb')
        newFileByteArray = bytearray(s)
        f.write(newFileByteArray) # python will convert \n to os.linesep
        f.close() # you can omit in most cases as the destructor will call it

def toHex(s):
    lst = []
    for ch in s:
        hv = hex(ord(ch)).replace('0x', '')
        if len(hv) == 1:
            hv = '0'+hv
        lst.append(hv)
    
    return reduce(lambda x,y:x+y, lst)

def toOutputFile(hex, className):
    fileContents = '''
namespace Cobra{
namespace internal{
        static const char* '''
    fileContents += className
    fileContents += '''Bytes = "'''
    fileContents += hex
    fileContents += '''";
} // namespace internal
} // namespace Cobra
    '''
    return fileContents

def start(name, className):
    bytesname = string.replace(name, '.cb', '-bytes.h')
    clean(string.replace(name, '.cb', '.o'))
    clean(bytesname)
    s = getBytes(name)
    hexa = toHex(s.decode("utf-8"))
    out = toOutputFile(hexa, className)
    toFile(bytesname, out)

if __name__ == "__main__":
    if options.internalFiles:
        start('src/cobra/types/strings/string.cb', 'String')
        start('src/cobra/types/try_catch/exception.cb', 'Exception')
        start('src/cobra/types/array/array.cb', 'Array')

