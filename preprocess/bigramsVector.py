import re, os, csv, string
import nltk, re, pprint
from nltk.corpus import stopwords 
from nltk import word_tokenize
from nltk.tokenize import RegexpTokenizer
from nltk.util import bigrams,trigrams
from nltk import FreqDist

def removeStopwords(features):
    stop = stopwords.words('english')
    features = [i for i in features if i not in stop]
    return features 

def extract(featureList, dir, fileout,n):
    tokenizer = RegexpTokenizer(r'\w+')

    docPos = {}
    docNeg = {}
    docFeatures = {}
    sentiment = "pos"
    for file in os.listdir(dir+sentiment):
        if file.endswith(".txt"):
            features = {}
            sentiment = "pos"
            fp = open(dir+sentiment+"/"+file, 'rb')
            doc = fp.read()
            tokens = [b for b in bigrams(removeStopwords(tokenizer.tokenize(doc)))]
            for word in featureList:
                if word in tokens:
                    features[word] = 1.0
                else:
                    features[word] = 0.0
            docPos[file] = ""
            docFeatures[file] = features

    sentiment = "neg"
    for file in os.listdir(dir+sentiment):
        if file.endswith(".txt"):
            features = {}
            sentiment = "neg"
            fp = open(dir+sentiment+"/"+file, 'rb')
            doc = fp.read()
            tokens = [b for b in bigrams(removeStopwords(tokenizer.tokenize(doc)))]
            for word in featureList:
                if word in tokens:
                    features[word] = 1.0
                else:
                    features[word] = 0.0
            docNeg[file] = ""
            docFeatures[file] = features
    f = FreqDist(featureList)
    featureList = [x for (x,f) in f.items()[:n]]
    allData = []

    for doc in docFeatures.keys():
        data = []
        count = 1
        if doc in docNeg.keys():
            val =['-1']
        if doc in docPos.keys():
            val =['1']
        for key in featureList:
            data.append("%s:%s" %(count, docFeatures[doc][key]))
            count +=1
        val.extend(data)
        allData.append(" ".join(val))
    # for doc in docFeaturesPos.keys():
    #     data =['+1']
    #     for key in featureList:
    #         data.append("%s:%s" %(count, docFeaturesPos[doc][key]))
    #         count +=1
    #     count = 1
    #     allData.append(" ".join(data))
    fVectorWriter = csv.writer(open(dir+fileout+".txt", 'wb'))
    for d in allData:
        print d
        fVectorWriter.writerow([d])

# 

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
            #tokens.extend(word_tokenize(doc))
            tokens.extend(tokenizer.tokenize(doc))
            fp.close()
    return tokens

dir = "/home/jch550/dev/JJboost/data/txt_sentoken/"
# print "extracting feautres..."
# featuresRaw = extractFeatures(dir)
# # print "cleaning features..."
# featuresClean = removeStopwords(featuresRaw)
# featuresBigrams = bigrams(featuresClean)
# # print "writing to file..."
# fListWriter = csv.writer(open(dir+"featureBigramsList.txt", 'w'))
# for f in featuresBigrams:
#     fListWriter.writerow([f])

features = open(dir+"featureBigramsList.txt", 'rb')
featuresList = features.read().split("\r\n")
featuresList = [b for b in featuresBigrams]
print "extracting features from documents..."
extract(featuresList, dir, "docs_train_bigrams", 500)
print "DONE."