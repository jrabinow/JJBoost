#!/usr/bin/env python3

from errno import ENOENT, ENOMEM
import subprocess

def main():
    types = {"discrete":"0", "real":"1", "gentle":"2", "modest":"3" }
    featureSet1 = [ "MADABOOST", "ETABOOST", "LOGITBOOST", None ]
    featureSet2 = [ "EARLY_TERMINATION", None ]
    baseFileName = "AdaBoost_orig.h"
    compileCmd = [ "make", "-j" ]

    for feature1 in featureSet1:
        for feature2 in featureSet2:
            genNextFile(baseFileName, feature1, feature2)
            output, retcode = launchPrgm(compileCmd, "stdout")
            rawOutput(output, "stdout")
            exit(0)

def genNextFile(filename, feature1, feature2):
    print("lol")

def launchPrgm(launchArgs, outputFileName):
    outputLocal = None	# Have to declare variables here to humor python...
    retCodeLocal = ENOENT	# error code: invalid path
    if outputFileName == "stdout":
        outputFile = subprocess.PIPE
    else:
        outputFile = open(outputFileName, "w")
    try:
        proc = subprocess.Popen(launchArgs, stdin=subprocess.PIPE, stdout=outputFile, stderr=subprocess.PIPE, shell=False)
            # non-blocking function.
    except OSError as error:
        if error.errno == ENOENT:	# incorrect path or program name
            stderr.write("Error: failed to execute {0} : {1}\n".format(launchArgs[0], error))
            exit(-1)
        else:
            stderr.write("Unknown error : {0}\n".format(error))
            exit(-1)
    try:
        outputLocal = proc.communicate()
        retCodeLocal = proc.wait()
    except KeyboardInterrupt:
        terminateProcess(proc, "Error: program execution was interrupted by user.\n")
        exit(-1)
    return (outputLocal, retCodeLocal)

def rawOutput(output, scriptOutput):
	out = [output[0], output[1]]	# This may look rather stupid, but python raises a TypeError if you dare assign values to a tuple
	if output[0] is None:
		out[0] = " NO OUTPUT ON STDOUT"
	if str(output[1]) == "b''":
		out[1] = " NO OUTPUT ON STDERR"
	if scriptOutput == "stdout":
		print("Printing raw output from program.\n--------------- stdout ---------------\n\n{0}".format(str(out[0])[1:].strip("'").replace("\\n", "\n")),
		"\n\n--------------- stderr ---------------\n\n{0}\n".format(str(out[1])[1:].strip("'").replace("\\n", "\n")))
	else:
		with open(scriptOutput, "a") as outputFile:
			outputFile.write("""Printing raw output from program.\n--------------- stdout ---------------\n\n{0}\n
\n--------------- stderr ---------------\n\n{1}\n""".format(str(out[0])[1:].strip("'").replace("\\n", "\n"), str(out[1])[1:].strip("'").replace("\\n", "\n")))


if __name__ == "__main__":
    main()
