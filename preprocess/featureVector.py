import re, os, csv, string
import nltk, re, pprint
from nltk.corpus import stopwords 
from nltk import word_tokenize
from nltk.tokenize import RegexpTokenizer
from nltk import FreqDist

def removeStopwords(features):
    stop = stopwords.words('english')
    features = [i for i in features if i not in stop]
    return features 


def extract(featureList, dir,n):
    tokenizer = RegexpTokenizer(r'\w+')

    docFeaturesPos = {}
    docFeaturesNeg = {}
    sentiment = "pos"
    for file in os.listdir(dir+sentiment):
        if file.endswith(".txt"):
            features = {}
            sentiment = "pos"
            fp = open(dir+sentiment+"/"+file, 'rb')
            doc = fp.read()
            tokens = tokenizer.tokenize(doc)
            for word in featureList:
                if word in tokens:
                    features[word] = 1.0
                else:
                    features[word] = 0.0
            docFeaturesPos[file] = features

    sentiment = "neg"
    for file in os.listdir(dir+sentiment):
        if file.endswith(".txt"):
            features = {}
            sentiment = "neg"
            fp = open(dir+sentiment+"/"+file, 'rb')
            doc = fp.read()
            tokens = tokenizer.tokenize(doc)
            for word in featureList:
                if word in tokens:
                    features[word] = 1.0
                else:
                    features[word] = 0.0
            docFeaturesNeg[file] = features

    f = FreqDist(featureList)
    featureList = [x for (x,f) in f.items()[:n]]

    allData = []

    count = 1
    for doc in docFeaturesPos.keys():
        data =['+1']
        for key in featureList:
            data.append("%s:%s" %(count, docFeaturesPos[doc][key]))
            count +=1
        count = 1
        allData.append(" ".join(data))

    count = 1
    for doc in docFeaturesNeg.keys():
        data =['-1']
        for key in featureList:
            data.append("%s:%s" %(count, docFeaturesNeg[doc][key]))
            count +=1
        count = 1
        allData.append(" ".join(data))

    fVectorWriter = csv.writer(open(dir+"docs_train.txt", 'wb'))
    for d in allData:
        fVectorWriter.writerow([d])



def extractFeatures(dir):
    docs = []         
    featureList = []
    tokenizer = RegexpTokenizer(r'\w+')

    sentiment = "pos"

    for file in os.listdir(dir+sentiment):
        if file.endswith(".txt"):
            fp = open(dir+sentiment+"/"+file, 'r')
            doc = fp.read()
            tokens = tokenizer.tokenize(doc)
            fp.close()

    sentiment = "neg"

    for file in os.listdir(dir+sentiment):
        if file.endswith(".txt"):
            fp = open(dir+sentiment+"/"+file, 'r')
            doc = fp.read()
            tokens.extend(tokenizer.tokenize(doc))
            fp.close()
    return tokens


dir = "C:/Users/jhsu/development/JJBoost/data/txt_sentoken/"
print "extracting feautres..."
featuresRaw = extractFeatures(dir)
print "cleaning features..."
featuresClean = removeStopwords(featuresRaw)
print "writing to file..."
fListWriter = csv.writer(open(dir+"featureList.txt", 'w'))
for f in featuresClean:
    fListWriter.writerow([f])

features = open(dir+"featureList.txt", 'rb')
featuresList = features.read().split('\r\n')
print "extracting features from documents..."
extract(featuresList, dir,500)
print "DONE."