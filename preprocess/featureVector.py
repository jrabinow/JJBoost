import re, os, csv, string
import nltk, re, pprint
from nltk.corpus import stopwords 
from nltk import word_tokenize
from nltk.tokenize import RegexpTokenizer


def removeStopwords(features):
    stop = stopwords.words('english')
    features = [i for i in features if i not in stop]
    return features 

def features():

    docs = []         
    featureList = []
    #Read the tweets one by one and process it
    dir = "/Users/jasminehsu/Downloads/review_polarity/txt_sentoken/"
    for file in os.listdir("/Users/jasminehsu/Downloads/review_polarity/txt_sentoken/pos"):
        if file.endswith(".txt"):
            sentiment = "pos"
            fp = open(dir+sentiment+"/"+file, 'r')
            doc = fp.read()
            #cleanText = processText(doc)
            featureVector = getFeatureVector(doc)
            featureList.extend(featureVector)
            docs.append((sentiment, featureVector));
            #end loop
            fp.close()

    for file in os.listdir("/Users/jasminehsu/Downloads/review_polarity/txt_sentoken/neg"):
        if file.endswith(".txt"):
            sentiment = "neg"
            fp = open(dir+sentiment+"/"+file, 'rb')
            doc = fp.read()
            #cleanText = processText(doc)
            featureVector = getFeatureVector(doc)
            featureList.extend(featureVector)
            docs.append((sentiment, featureVector));
            #end loop
            fp.close()

    fVectorWriter = csv.writer(open(dir+"featureVector.txt", 'wb'))
    for v in docs:
        fVectorWriter.writerow(v)


def extract(featureList):
    tokenizer = RegexpTokenizer(r'\w+')

    docFeaturesPos = {}
    docFeaturesNeg = {}
    dir = "/Users/jasminehsu/Downloads/review_polarity/txt_sentoken/"
    for file in os.listdir("/Users/jasminehsu/Downloads/review_polarity/txt_sentoken/pos"):
        if file.endswith(".txt"):
            features = {}
            docFeatureLen = 0
            sentiment = "pos"
            fp = open(dir+sentiment+"/"+file, 'rb')
            doc = fp.read()
            tokens = tokenizer.tokenize(doc)
            for word in featureList:
                if word in tokens:
                    features[word] = 1
                    docFeatureLen+=1
                else:
                    features[word] = 0
            docFeaturesPos[file] = features

    for file in os.listdir("/Users/jasminehsu/Downloads/review_polarity/txt_sentoken/neg"):
        if file.endswith(".txt"):
            features = {}
            docFeatureLen = 0
            sentiment = "neg"
            fp = open(dir+sentiment+"/"+file, 'rb')
            doc = fp.read()
            tokens = tokenizer.tokenize(doc)
            for word in featureList:
                if word in tokens:
                    features[word] = 1
                    docFeatureLen+=1
                else:
                    features[word] = 0
            docFeaturesNeg[doc] = features

    allData = []

    count = 0
    for doc in docFeaturesPos.keys():
        data =['+1']
        for key in featureList:
            data.append("%s:%s" %(count, docFeaturesPos[doc][key]))
            count +=1
        count = 0
        allData.append(" ".join(data))

    count = 0
    for doc in docFeaturesNeg.keys():
        data =['-1']
        for key in featureList:
            data.append("%s:%s" %(count, docFeaturesNeg[doc][key]))
            count +=1
        count = 0
        allData.append(" ".join(data))

    fVectorWriter = csv.writer(open(dir+"docs_train.txt", 'wb'))
    for d in allData:
        fVectorWriter.writerow([d])



def extractFeatures():
    docs = []         
    featureList = []
    tokenizer = RegexpTokenizer(r'\w+')

    #Read the tweets one by one and process it
    dir = "/Users/jasminehsu/Downloads/review_polarity/txt_sentoken/"
    for file in os.listdir("/Users/jasminehsu/Downloads/review_polarity/txt_sentoken/pos"):
        if file.endswith(".txt"):
            sentiment = "pos"
            fp = open(dir+sentiment+"/"+file, 'r')
            doc = fp.read()
            tokens = tokenizer.tokenize(doc)
            #tokens = word_tokenize(doc)
            fp.close()
    for file in os.listdir("/Users/jasminehsu/Downloads/review_polarity/txt_sentoken/neg"):
        if file.endswith(".txt"):
            sentiment = "neg"
            fp = open(dir+sentiment+"/"+file, 'r')
            doc = fp.read()
            #tokens.extend(word_tokenize(doc))
            tokens.extend(tokenizer.tokenize(doc))
            fp.close()
    return tokens


dir = "/Users/jasminehsu/Downloads/review_polarity/txt_sentoken/"
print "extracting feautres..."
featuresRaw = extractFeatures()
print "cleaning features..."
featuresClean = removeStopwords(featuresRaw)
print "writing to file..."
fListWriter = csv.writer(open(dir+"featureList.txt", 'w'))
for f in featuresClean:
    fListWriter.writerow([f])

features = open("/Users/jasminehsu/Downloads/review_polarity/txt_sentoken/featureList.txt", 'rb')
featuresList = features.read().split('\r\n')
print "extracting features from documents..."
extract(featuresList)
print "DONE."