#!/usr/bin/env python3

from errno import ENOENT, ENOMEM
import sys,os,subprocess,re,time,getopt, threading

class myThread(threading.Thread):
    def __init__(self, trainCmd, predictCmd, dataset, feature1, feature2, boostingName, boostingType, outputFile, writeLock):
        threading.Thread.__init__(self)
        self.trainCmd = trainCmd
        self.predictCmd = predictCmd
        self.dataset = dataset
        self.feature1 = feature1
        self.feature2 = feature2
        self.boostingName = boostingName
        self.boostingType = boostingType
        self.outputFile = outputFile
        self.writeLock = writeLock
    
    def run(self):
        run_on_dataset(self.trainCmd, self.predictCmd, self.dataset, self.outputFile, self.writeLock, self.feature1, self.feature2, self.boostingName, self.boostingType)

def run_on_dataset(trainCmd, predictCmd, dataset, outputFile, writeLock, feature1, feature2, boostingName, boostingType):
    accuracyRegex = re.compile("Accuracy = ([0-9.]+) \(([0-9]+) / ([0-9]+)\)")
    positivesRegex = re.compile("positive: ([0-9.]+) \(([0-9]+) / ([0-9]+)\)")
    negativesRegex = re.compile("negative: ([0-9.]+) \(([0-9]+) / ([0-9]+)\)")
    trainCmd[2] = dataset[0]
    output, retcode = launchPrgm(trainCmd)
    if retcode == 0:
        predictCmd[1] = dataset[1]
        predictCmd[2] = dataset[0] + ".model"
        output, retcode = launchPrgm(predictCmd)
        if retcode == 0:
            string = str(output[0])[1:].strip("'").replace("\\n", "\n")
            result = accuracyRegex.search(string)
            if result is not None:
                accuracy = result.group(1)
                success = result.group(2)
                total = result.group(3)
            presult = positivesRegex.search(string)
            if presult is not None:
                paccuracy = presult.group(1)
                psuccess = presult.group(2)
                ptotal = presult.group(3)
            nresult = negativesRegex.search(string)
            if nresult is not None:
                naccuracy = nresult.group(1)
                nsuccess = nresult.group(2)
                ntotal = nresult.group(3)
            if result is not None and presult is not None and nresult is not None:
                writeLock.acquire()
                outputFile.write("{0},{1},{2},{3},{4},{5},{6},{7},{8}, \"FEATURE1={9} FEATURE2={10} BOOSTINGTYPE={11} DATASET={12}\"\n".format(accuracy, success, total, paccuracy, psuccess, ptotal, naccuracy, nsuccess, ntotal, feature1, feature2, boostingName, re.sub("train", "", dataset[0])))
                writeLock.release()
                if outputFile is not sys.stdout:
                    print(accuracy, ",", success, ",", total, ",", paccuracy, ",", psuccess, ", ", ptotal, ",", naccuracy, ",", nsuccess, ",", ntotal, ", \"FEATURE1 =", feature1, "FEATURE2 =", feature2, "BOOSTINGTYPE =", boostingName, "DATASET =", re.sub("train", "", dataset[0]), "\"")
            else:
                sys.stderr.write("ERROR PARSING OUTPUT!!!\n")
                writeLock.acquire()
                outputFile.write("0,0,0,0,0,0,0,0,0, \"FEATURE1={0} FEATURE2={1} BOOSTINGTYPE={2} DATASET={3} STATUS=FAIL\"\n".format(feature1, feature2, boostingName, re.sub("train", "", dataset[0])))
                writeLock.release()
                if outputFile is not sys.stdout:
                    print("0,0,0,0,0,0,0,0,0, \"FEATURE1 =", feature1, "FEATURE2 =", feature2, "BOOSTINGTYPE =", boostingName, "DATASET =", re.sub("train", "", dataset[0]), "STATUS=FAIL\"")
        else:
            sys.stderr.write("PREDICTING FAILED!!!\n")
            rawoutput(output, "stdout")
            exit(-1)
    else:
        sys.stderr.write("TRAINING FAILED!!!\n")
        rawOutput(output, "stdout")
        exit(-1)
    

def main():
    types = {"discrete":"0", "real":"1", "gentle":"2", "modest":"3" }
    featureSet1 = [ "MADABOOST", "ETABOOST", "LOGITBOOST", None ]
    featureSet2 = [ "EARLY_TERMINATION", None ]
    opts = list(range(2))
    compileCmd = [ "make", "-j", "-f", "Makefile" ]
    cleanCmd = ["make", "clean"]
    trainCmd = [ "./abtrain", "", "" ]
    predictCmd = [ "./abpredict", "", "" ]
    outputFile = sys.stdout

    try:
        optlist, args = getopt.getopt(sys.argv[1:], "o:")
        for opt in optlist:
            if opt[0] == '-o':
                outputFile = open(opt[1], "w")
    except getopt.GetoptError:
        sys.stderr.write("Error parsing options.\n")
        exit(-1)

    threadpool = list()
    datasets = argsToDataSets(args)
    writeLock = threading.Lock()

    outputFile.write("Accuracy,Success,Total,PositiveAccuracy,PositiveSuccess,PositiveTotal,NegativeAccuracy,NegativeSuccess,NegativeTotal,BoostParams\n")
    if outputFile is not sys.stdout:
        print("Accuracy,Success,Total,PositiveAccuracy,PositiveSuccess,PositiveTotal,NegativeAccuracy,NegativeSuccess,NegativeTotal,BoostParams")
    # select feature 1
    for feature1 in featureSet1:
        opts[0] = feature1
        # select feature 2
        for feature2 in featureSet2:
            opts[1] = feature2
            setEnvVariables(opts)
            # compile program
            launchPrgm(cleanCmd)
            output, retcode = launchPrgm(compileCmd)
            if retcode == 0:
                # select boosting type
                for boostingName, boostingType in types.items():
                    trainCmd[1] = "-t" + boostingType
                    # run on all datasets
                    for dataset in datasets:
                        newthread = myThread(trainCmd, predictCmd, dataset, feature1, feature2, boostingName, boostingType, outputFile, writeLock)
                        try:
                            newthread.start()
                        except:
                           sys.stderr.write("FAILED LAUNCHING THREAD!!! {0},{1},{2},{3}\n".format(feature1, feature2, boostingName, dataset[1]))
                        threadpool.append(newthread) 
                    time.sleep(1)
                    for t in threadpool:
                        t.join()
            else:
                sys.stderr.write("BUILD FAILED!!! {0},{1}\n".format(feature1, feature2))
                rawOutput(output, "stdout")
                exit(-1)
            launchPrgm(cleanCmd)

def setEnvVariables(options):
    cppflags = ""
    for opt in options:
        if opt is not None:
            cppflags += "-D" + opt + " "
    os.environ['CXXFLAGS'] = cppflags

def argsToDataSets(args):
    datasets = list()
    files = list()
    if len(args) < 1:
        print("Usage: ./gridsearch DATADIR")
        exit(0)

    for path in args:
        for dirname, subdirList, fileList in os.walk(path):
            for fname in fileList:
                files.append((fname, dirname + "/" + fname))
    files.sort()
    for i in range(len(files) - 1):
        if re.sub("test|train", "", files[i][0]) == re.sub("test|train", "", files[i+1][0]):
            if "train" in files[i][0] and "test" in files[i+1][0]:
                datasets.append((files[i][1], files[i+1][1]))
            elif "test" in files[i][0] and "train" in files[i+1][0]:
                datasets.append((files[i+1][1], files[i][1]))
    return datasets


def launchPrgm(launchArgs):
    outputLocal = None        # Have to declare variables here to humor python...
    retCodeLocal = ENOENT     # error code: invalid path
    try:
        proc = subprocess.Popen(launchArgs, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=False)
       # non-blocking function.
    except OSError as error:
        if error.errno == ENOENT:    # incorrect path or program name
            sys.stderr.write("Error: failed to execute {0} : {1}\n".format(launchArgs[0], error))
            exit(-1)
        else:
            sys.stderr.write("Unknown error : {0}\n".format(error))
            exit(-1)
    try:
        outputLocal = proc.communicate()
        retCodeLocal = proc.wait()
    except KeyboardInterrupt:
        terminateProcess(proc, "Error: program execution was interrupted by user.\n")
        exit(-1)
    return (outputLocal, retCodeLocal)



def rawOutput(output, scriptOutput):
    out = [output[0], output[1]]    # This may look rather stupid, but python raises a TypeError if you dare assign values to a tuple
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


def terminateProcess(proc, errMsg, killAttempt = 3):
    if killAttempt == 0:
        sys.stderr.write("Aborting attempts to kill program. Program PID {0} still running.\nExiting now...\n".format(proc.pid))
        exit(-1)
    else:
        try:
            sys.stderr.write(errMsg + "Sending SIGTERM to program.\n")
            proc.terminate()
            time.sleep(1)
            if proc.poll() is None:        # proc.poll() returns None if the program has NOT terminated.
                sys.stderr.write("\033[1;33mProgram took more than 1 second to quit. Sending SIGKILL to program.\033[0m\n")
                proc.kill()
                time.sleep(3)
                if proc.poll() is None:
                    sys.stderr.write("\033[0;31mERROR: UNABLE TO TERMINATE PROGRAM !!! ",
                            "PLEASE TERMINATE PROGRAM WITH PID {0} MANUALLY.\033[0m\n".format(proc.pid))
                else:
                    sys.stderr.write("Program killed successfully.\n")
            else:
                sys.stderr.write("Program exited successfully.\n")
        except KeyboardInterrupt:
            # prevents impatient users from unintentionally aborting kill because of lag, or because they were pressing random
            # keys on the keyboard
            if proc.poll() is None:
                sys.stderr.write("Please be patient, the program is being terminated.\nIf you really wished to kill this python ",
                        "wrapper off without exiting the program itself, open a terminal and kill python3.\n")
                terminateProcess(proc, errMsg, killAttempt-1)    # finish the job
            else:
                sys.stderr.write("Program killed successfully.\n");

if __name__ == "__main__":
    main()
